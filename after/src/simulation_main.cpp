
#include <iostream>
#include <chrono>
#include <omp.h>
#include "CfdSolver/field_init.hpp"
#include "CfdSolver/cell_flags.hpp"
#include "CfdSolver/solver_interface.hpp"
#include "CfdSolver/serial/serial_solver.hpp"
#ifdef USE_MPI
#include "CfdSolver/Mpi/CfdSolverMpi.hpp"
#include "CfdSolver/Mpi/MpiHelpers.hpp"
#endif
#ifdef USE_CUDA
#include "CfdSolver/cuda/cuda_solver.hpp"
#endif
#ifdef USE_OPENCL
#include "CfdSolver/Opencl/CfdSolverOpencl.hpp"
#endif
#include "IO/text_file_utils.hpp"
#include "IO/cli_options.hpp"
#include "IO/progress_report.hpp"
#include "IO/case_config.hpp"
#include "IO/netcdf_output.hpp"
#include "IO/vtk_output.hpp"
#include "IO/path_writer.hpp"
#include "IO/scenario_builder.hpp"
#include "ParticleTracer/streamline_tracer.hpp"
const std::string resultDirectory = "output/";

const std::string caseDirectory = "../scenarios/";
const std::string curveDirectory = "lines/";
const std::string shapeDirectory = "geometry/";

int main(int argc, char *argv[]) {
    CfdSolver *fluidSolver;
    StatusReporter statusReporter;
    std::string resultWriterType;
    OutputFileRecorder *resultWriter = nullptr;
    LinearSystemSolverType linearSystemSolverType;
    bool computeFlowlines = false;
    std::vector<fpvec3> seedPoints;
    bool dataIsCurrent = true;
    bool shallWriteOutput = true;

#ifdef USE_MPI
    int localRank = 0, partCount = 1, peerL, peerR, peerD, peerU, peerB, peerF, rankCoordI, rankCoordJ, rankCoordK,
            il, iu, jl, ju, kl, ku;
#else
    int partCount = 1;
    int localRank = 0;
#endif
    int partsAlongI = 1, jSplit = 1, kSplit = 1;

    int tileSizeX, blockExtentY, blockExtentZ, blockExtent1D;
    int gpuId = 0;

    int openclPlatformIndex = 0;

    int imax, jmax, kmax, itermax, particleCount;
    Real Re, Pr, UI, VI, WI, PI, TI, GX, GY, GZ, tEnd, dtWrite,
            dt, dx, dy, dz, alpha, omg, tau, eps, beta, T_h, T_c;
    DomainGeometry geometry;
    bool useTemperature = true;
    std::string scenarioName, shapeName, caseFilename, shapeFilename, outputBaseName, backendName;
    parseOptions(
            argc, argv, scenarioName, backendName, resultWriterType, shallWriteOutput, linearSystemSolverType,
            particleCount, computeFlowlines, partsAlongI, jSplit, kSplit,
            tileSizeX, blockExtentY, blockExtentZ, blockExtent1D, gpuId, openclPlatformIndex);
    caseFilename = caseDirectory + scenarioName + ".dat";

#ifdef USE_MPI
    if (backendName == "mpi") {
        mpiStartup(argc, argv, {partsAlongI, jSplit, kSplit}, imax, jmax, kmax,
                localRank, il, iu, jl, ju, kl, ku,
                peerL, peerR, peerD, peerU, peerB, peerF,
                rankCoordI, rankCoordJ, rankCoordK, partCount);

        setThreadCount(1);
    }
#endif

    if (resultWriterType == "netcdf") {
        resultWriter = new NetCdfRecorder(partCount, localRank);
    } else if (resultWriterType == "vtk") {
        resultWriter = new VtkExporter(partCount, localRank);
    } else if (resultWriterType == "vtk-binary") {
        resultWriter = new VtkExporter(partCount, localRank, VTK_FORMAT_BINARY);
    } else if (resultWriterType == "vtk-ascii") {
        resultWriter = new VtkExporter(partCount, localRank, VTK_FORMAT_ASCII);
    } else if (resultWriterType.length() == 0) {
        resultWriter = new VtkExporter(partCount, localRank);
    } else {
        if (localRank == 0) {
            std::cerr << "Invalid output format." << std::endl;
        }
        exit(1);
    }

    loadCaseConfigFromFile(
            caseFilename, scenarioName, shapeName,
            tEnd, dtWrite, geometry,
            UI, VI, WI, PI, TI, GX, GY, GZ,
            Re, Pr, omg, eps, itermax, alpha, beta, dt, tau, useTemperature,
            T_h, T_c, imax, jmax, kmax, dx, dy, dz);
    fpvec3 domainOrigin = fpvec3(geometry.xOrigin, geometry.yOrigin, geometry.zOrigin);
    fpvec3 domainExtent = fpvec3(geometry.xLength, geometry.yLength, geometry.zLength);
    FlowlineIntegrator flowlineIntegrator;
    seedPoints = computeParticleSeedsForScenario(scenarioName, domainOrigin, domainExtent, particleCount);

    if (!useTemperature){
        T_c = 0.0;
        T_h = 0.0;
        beta = 0.0;
        Pr = 0.0;
        TI = 0.0;
    }

#ifdef FP_FLOAT
    eps *= 2.0f;
#endif

    shapeFilename = shapeDirectory + shapeName;
    outputBaseName = resultDirectory + scenarioName;
    if (localRank == 0) {
        std::cout << "Scenario name: " << scenarioName << std::endl;
        std::cout << "Scenario file: " << caseFilename << std::endl;
        std::cout << "Geometry file: " << shapeFilename << std::endl;
        std::cout << "Output file: " << outputBaseName << std::endl;
        std::cout << "Solver name: " << backendName << std::endl;
    }

    std::string fileExtension = resultWriter->getFileExtension();

    setupOutputDirectory(resultDirectory, curveDirectory, shapeDirectory);
    if (shallWriteOutput) {
        clearStaleOutputFiles(resultDirectory, outputBaseName, fileExtension);
    }

    int n = 0;
    Real t = 0;
    Real writeTimer = 0;

    Real *U, *V, *W, *P, *T;
    FlagType *Flag, *globalFlags;

#ifdef USE_MPI
    if (backendName == "mpi") {
        MpiIndexRange rangeI = mpiAssignRange(partsAlongI, rankCoordI, imax);
        MpiIndexRange rangeJ = mpiAssignRange(jSplit, rankCoordJ, jmax);
        MpiIndexRange rangeK = mpiAssignRange(kSplit, rankCoordK, kmax);
        il = rangeI.lower; iu = rangeI.upper;
        jl = rangeJ.lower; ju = rangeJ.upper;
        kl = rangeK.lower; ku = rangeK.upper;


        U = new Real[(iu - il + 4)*(ju - jl + 3)*(ku - kl + 3)];
        V = new Real[(iu - il + 3)*(ju - jl + 4)*(ku - kl + 3)];
        W = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 4)];
        P = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];
        T = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];
        Flag = new FlagType[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];
        globalFlags = new FlagType[(imax+2)*(jmax+2)*(kmax+2)];
    } else
#endif
    {
        U = new Real[(imax+1)*(jmax+2)*(kmax+2)];
        V = new Real[(imax+2)*(jmax+1)*(kmax+2)];
        W = new Real[(imax+2)*(jmax+2)*(kmax+1)];
        P = new Real[(imax+2)*(jmax+2)*(kmax+2)];
        T = new Real[(imax+2)*(jmax+2)*(kmax+2)];
        Flag = new FlagType[(imax+2)*(jmax+2)*(kmax+2)];
        globalFlags = Flag;
    }

    if (shapeName == "none") {
        initFlagNoBarriers(scenarioName, imax, jmax, kmax, globalFlags);
    } else {
        if (!fileIsPresent(shapeFilename)) {
            buildScenario(scenarioName, shapeFilename, imax, jmax, kmax);
        }
        initFlagFromShapeFile(scenarioName, shapeFilename, imax, jmax, kmax, globalFlags);
    }

#ifdef USE_MPI
    if (backendName == "mpi") {
        resultWriter->setMpiBounds(il, iu, jl, ju, kl, ku);

        for (int i = il - 1; i <= iu + 1; i++) {
            for (int j = jl - 1; j <= ju + 1; j++) {
                for (int k = kl - 1; k <= ku + 1; k++) {
                    Flag[(((i) - (il-1))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))]
                            = globalFlags[LINFLAG(i, j, k)];
                }
            }
        }

        initFieldsMpi(UI, VI, WI, PI, TI, il, iu, jl, ju, kl, ku, U, V, W, P, T, globalFlags);
    } else
#endif
    {
        initFields(UI, VI, WI, PI, TI, imax, jmax, kmax, U, V, W, P, T, globalFlags);
    }

    auto startTime = std::chrono::system_clock::now();

    if (shallWriteOutput) {
        resultWriter->setupRecorder(outputBaseName, {imax, jmax, kmax, dx, dy, dz, geometry.xOrigin, geometry.yOrigin, geometry.zOrigin});
        resultWriter->writeSnapshot(0, t, U, V, W, P, T, Flag);
    }


    if (backendName == "cpp") {
        fluidSolver = new FluidSolverCpp();
    }
#ifdef USE_MPI
    else if (backendName == "mpi") {
        fluidSolver = new CfdSolverMpi(
                il, iu, jl, ju, kl, ku, localRank, peerL, peerR, peerD, peerU, peerB, peerF);
    }
#endif
#ifdef USE_CUDA
    else if (backendName == "cuda") {
        if (linearSystemSolverType != LINEAR_SOLVER_JACOBI) {
            std::cerr << "Warning: CUDA solver was selected, but a linear solver different from Jacobi. "
                    << "Only the Jacobi solver is supported for CUDA." << std::endl;
        }
        fluidSolver = new FluidSolverCuda(
                gpuId, tileSizeX, blockExtentY, blockExtentZ, blockExtent1D);
    }
#endif
#ifdef USE_OPENCL
    else if (backendName == "opencl") {
        if (linearSystemSolverType != LINEAR_SOLVER_JACOBI) {
            std::cerr << "Warning: OpenCL solver was selected, but a linear solver different from Jacobi. "
                      << "Only the Jacobi solver is supported for OpenCL." << std::endl;
        }
        fluidSolver = new CfdSolverOpencl(
                gpuId, openclPlatformIndex, tileSizeX, blockExtentY, blockExtentZ, blockExtent1D);
    }
#endif
    else {
        std::cerr << "Fatal error: Unsupported solver name \"" << backendName << "\"." << std::endl;
        exit(1);
    }
    fluidSolver->initialize(
            scenarioName, linearSystemSolverType, shallWriteOutput,
            Re, Pr, omg, eps, itermax, alpha, beta, dt, tau, GX, GY, GZ, useTemperature,
            T_h, T_c, imax, jmax, kmax, dx, dy, dz, U, V, W, P, T, Flag);

    while (t < tEnd) {
        if (localRank == 0) {
            statusReporter.renderProgress(t, tEnd, 50);
        }

        fluidSolver->setBoundaryValues();
        fluidSolver->setBoundaryValuesScenarioSpecific();
        dt = fluidSolver->calculateDt();
        if (useTemperature) {
            fluidSolver->calculateTemperature();
        }
        fluidSolver->calculateFgh();
        fluidSolver->calculateRs();

        fluidSolver->executeSorSolver();
        fluidSolver->calculateUvw();
        dataIsCurrent = false;

        t += dt;
        writeTimer += dt;
        n++;
        if (writeTimer - dtWrite > 0) {
            if (shallWriteOutput) {
                if (!dataIsCurrent) {
                    fluidSolver->getDataForOutput(U, V, W, P, T);
                    dataIsCurrent = true;
                }
                resultWriter->writeSnapshot(n, t, U, V, W, P, T, Flag);
                if (localRank == 0) {
                    reportFileWritten(n, t, 50);
                }
            }
            writeTimer -= dtWrite;
        }
    }

    if (computeFlowlines) {
        if (!dataIsCurrent) {
            fluidSolver->getDataForOutput(U, V, W, P, T);
            dataIsCurrent = true;
        }
        Real flowlineDt = dt * Real(5.0);
        if (hasPrefix(scenarioName, "rayleigh_benard")) {
            flowlineDt *= Real(1000.0);
        }
        FlowCurves streamlines = flowlineIntegrator.trace(
                seedPoints, domainOrigin, domainExtent, flowlineDt, imax, jmax, kmax, dx, dy, dz, U, V, W, P, T);
        writePathsToObj(curveDirectory + scenarioName + "-streamlines.obj", streamlines);
        writePathsToBinary(curveDirectory + scenarioName + "-streamlines.binlines", streamlines);
    }


    auto endTime = std::chrono::system_clock::now();
    auto runDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    if (localRank == 0) {
        std::cout << "System time elapsed: " << (double(runDuration.count()) * 1e-6) << "s" << std::endl;
    }

    delete fluidSolver;
    delete resultWriter;
    delete[] U;
    delete[] V;
    delete[] W;
    delete[] P;
    delete[] T;
    if (Flag != globalFlags) {
        delete[] globalFlags;
    }
    delete[] Flag;

#ifdef USE_MPI
    if (backendName == "mpi") {
        mpiShutdown();
    }
#endif

    return 0;
}