
#include <cstring>
#include <iostream>
#include "UvwOpencl.hpp"
#include "SorSolverOpencl.hpp"
#include "CfdSolverOpencl.hpp"
#include "../../Defines.hpp"

#include "CfdSolver/serial/momentum_eqs.hpp"
#include "CfdSolver/serial/poisson_sor.hpp"
#include "CfdSolver/serial/boundary_conditions.hpp"

const std::string openclSourceFolder = "../src/CfdSolver/Opencl/";

CfdSolverOpencl::CfdSolverOpencl(int gpuId, int platformIndex, int tileSizeX, int blockExtentY, int blockExtentZ, int blockExtent1D) {
    this->gpuId = gpuId;
    this->platformIndex = platformIndex;
    this->tileSizeX = tileSizeX;
    this->blockExtentY = blockExtentY;
    this->blockExtentZ = blockExtentZ;
    this->blockExtent1D = blockExtent1D;

    ClInterface::get()->initialize(CLContextConfig(platformIndex));
    ClInterface::get()->reportInfo();

    std::string prefixHeader = "";
#ifdef FP_FLOAT
    prefixHeader += "#define FP_FLOAT\n";
#else
    prefixHeader += "#define FP_DOUBLE\n";
#endif

    prefixHeader += importTextFile(openclSourceFolder + "OpenclFlags.hpp");

    context = ClInterface::get()->fetchContext();
    devices = ClInterface::get()->fetchDevices();
    if (devices.empty()) {
        std::cerr << "Fatal error in CfdSolverOpencl::CfdSolverOpencl: No supported OpenCL devices were found."
                  << std::endl;
        exit(1);
    }
    if (gpuId >= int(devices.size())) {
        std::cerr << "Error in CfdSolverOpencl::CfdSolverOpencl: Invalid device ID specified. Setting device ID to 0."
                  << std::endl;
        gpuId = 0;
    }

    ClInterface::get()->assignPrefixHeader(prefixHeader);
    gpuProgramBorderValues = ClInterface::get()->importProgramFromSourceFiles({
        openclSourceFolder + "BoundaryValuesOpencl.cl"});
    gpuProgramRelax = ClInterface::get()->importProgramFromSourceFiles({openclSourceFolder + "SorSolverOpencl.cl"});
    gpuProgramUvw = ClInterface::get()->importProgramFromSourceFiles({openclSourceFolder + "UvwOpencl.cl"});

    assignLeftRightBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignLeftRightBordersOpencl");
    assignDownUpBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignDownUpBordersOpencl");
    assignFrontBackBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignFrontBackBordersOpencl");
    assignInternalUBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignInternalUBordersOpencl");
    assignInternalVBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignInternalVBordersOpencl");
    assignInternalWBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignInternalWBordersOpencl");
    assignInternalTBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignInternalTBordersOpencl");
    assignDrivenCavityBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignDrivenCavityBordersOpencl");
    assignFlowOverStepBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignFlowOverStepBordersOpencl");
    assignSingleTowerBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignSingleTowerBordersOpencl");
    assignMountainBordersOpenclKernel = cl::Kernel(gpuProgramBorderValues, "assignMountainBordersOpencl");

    assignXYPlanesPressureBordersOpenclKernel = cl::Kernel(gpuProgramRelax, "assignXYPlanesPressureBordersOpencl");
    assignXZPlanesPressureBordersOpenclKernel = cl::Kernel(gpuProgramRelax, "assignXZPlanesPressureBordersOpencl");
    assignYZPlanesPressureBordersOpenclKernel = cl::Kernel(gpuProgramRelax, "assignYZPlanesPressureBordersOpencl");
    assignBorderConditionsPressureInDomainOpenclKernel = cl::Kernel(
            gpuProgramRelax, "assignBorderConditionsPressureInDomainOpencl");
    clonePressureOpenclKernel = cl::Kernel(gpuProgramRelax, "clonePressureOpencl");
    collapseSumOpenclKernelReal = cl::Kernel(gpuProgramRelax, "collapseSumOpenclKernelReal");
    collapseSumOpenclKernelUint = cl::Kernel(gpuProgramRelax, "collapseSumOpenclKernelUint");
    relaxSolverIterationOpenclKernel = cl::Kernel(gpuProgramRelax, "relaxSolverIterationOpencl");
    relaxSolverComputeResidualArrayOpenclKernel = cl::Kernel(gpuProgramRelax, "relaxSolverComputeResidualArrayOpencl");

    computeFghOpenclKernel = cl::Kernel(gpuProgramUvw, "computeFghOpencl");
    assignFBordersOpenclKernel = cl::Kernel(gpuProgramUvw, "assignFBordersOpencl");
    assignGBordersOpenclKernel = cl::Kernel(gpuProgramUvw, "assignGBordersOpencl");
    assignHBordersOpenclKernel = cl::Kernel(gpuProgramUvw, "assignHBordersOpencl");
    computeRsOpenclKernel = cl::Kernel(gpuProgramUvw, "computeRsOpencl");
    computeMaximumKernel = cl::Kernel(gpuProgramUvw, "computePeak");
    computeUvwOpenclKernel = cl::Kernel(gpuProgramUvw, "computeUvwOpencl");
    computeTemperatureOpenclKernel = cl::Kernel(gpuProgramUvw, "computeTemperatureOpencl");

#ifndef _TIMING_CL_
    queue = cl::DispatchQueue(context, devices[gpuId]);
#else
    queue = cl::DispatchQueue(context, devices[gpuId], CL_QUEUE_TIMING_ENABLE);
#endif

    size_t maxTeamSize;
    devices[gpuId].fetchInfo(CL_DEVICE_MAX_TEAM_SIZE, &maxTeamSize);

    teamSize1D = cl::GridRange(blockExtent1D);
    teamSize2D = cl::GridRange(tileSizeX, blockExtentY);
    teamSize3D = cl::GridRange(tileSizeX, blockExtentY, blockExtentZ);
    assert(size_t(tileSizeX) * size_t(blockExtentY) * size_t(blockExtentZ) <= maxTeamSize);
}

void CfdSolverOpencl::initialize(
        const std::string &scenarioName, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
        Real Re, Real Pr, Real omg, Real eps, int itermax, Real alpha, Real beta, Real dt, Real tau,
        Real GX, Real GY, Real GZ, bool useTemperature, Real T_h, Real T_c,
        int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
        Real *U, Real *V, Real *W, Real *P, Real *T, uint32_t *Flag) {
    this->scenarioName = scenarioName;
    this->linearSystemSolverType = linearSystemSolverType;
    this->shallWriteOutput = shallWriteOutput;
    this->Re = Re;
    this->Pr = Pr;
    this->omg = omg;
    this->eps = eps;
    this->itermax = itermax;
    this->alpha = alpha;
    this->beta = beta;
    this->dt = dt;
    this->tau = tau;
    this->GX = GX;
    this->GY = GY;
    this->GZ = GZ;
    this->useTemperature = useTemperature;
    this->T_h = T_h;
    this->T_c = T_c;
    this->imax = imax;
    this->jmax = jmax;
    this->kmax = kmax;
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;

    this->U = cl::Buffer(
            context, CL_MEM_FETCH_STORE | CL_MEM_CLONE_HOST_PTR,
            (imax+1)*(jmax+2)*(kmax+2)*sizeof(Real), (void*)U);
    this->V = cl::Buffer(
            context, CL_MEM_FETCH_STORE | CL_MEM_CLONE_HOST_PTR,
            (imax+2)*(jmax+1)*(kmax+2)*sizeof(Real), (void*)V);
    this->W = cl::Buffer(
            context, CL_MEM_FETCH_STORE | CL_MEM_CLONE_HOST_PTR,
            (imax+2)*(jmax+2)*(kmax+1)*sizeof(Real), (void*)W);
    this->P = cl::Buffer(
            context, CL_MEM_FETCH_STORE | CL_MEM_CLONE_HOST_PTR,
            (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real), (void*)P);
    this->T = cl::Buffer(
            context, CL_MEM_FETCH_STORE | CL_MEM_CLONE_HOST_PTR,
            (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real), (void*)T);
    this->Flag = cl::Buffer(
            context, CL_MEM_FETCH_ONLY | CL_MEM_CLONE_HOST_PTR,
            (imax+2)*(jmax+2)*(kmax+2)*sizeof(unsigned int), (void*)Flag);
    this->P_prev = cl::Buffer(context, CL_MEM_FETCH_STORE, (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real));
    this->T_prev = cl::Buffer(context, CL_MEM_FETCH_STORE, (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real));
    this->F = cl::Buffer(context, CL_MEM_FETCH_STORE, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real));
    this->G = cl::Buffer(context, CL_MEM_FETCH_STORE, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real));
    this->H = cl::Buffer(context, CL_MEM_FETCH_STORE, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real));
    this->RS = cl::Buffer(context, CL_MEM_FETCH_STORE, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real));

    int openclCollapseArrayUSize = divCeil((imax+1)*(jmax+2)*(kmax+2), blockExtent1D*2);
    int openclCollapseArrayVSize = divCeil((imax+1)*(jmax+2)*(kmax+2), blockExtent1D*2);
    int openclCollapseArrayWSize = divCeil((imax+1)*(jmax+2)*(kmax+2), blockExtent1D*2);
    int openclCollapseArrayResidualSize1 = divCeil(imax*jmax*kmax, blockExtent1D*2)*blockExtent1D*2;
    int openclCollapseArrayResidualSize2 = divCeil(imax*jmax*kmax, blockExtent1D*2);
    this->openclCollapseArrayU1 = cl::Buffer(context, CL_MEM_FETCH_STORE, openclCollapseArrayUSize*sizeof(Real));
    this->openclCollapseArrayU2 = cl::Buffer(context, CL_MEM_FETCH_STORE, openclCollapseArrayUSize*sizeof(Real));
    this->openclCollapseArrayV1 = cl::Buffer(context, CL_MEM_FETCH_STORE, openclCollapseArrayVSize*sizeof(Real));
    this->openclCollapseArrayV2 = cl::Buffer(context, CL_MEM_FETCH_STORE, openclCollapseArrayVSize*sizeof(Real));
    this->openclCollapseArrayW1 = cl::Buffer(context, CL_MEM_FETCH_STORE, openclCollapseArrayWSize*sizeof(Real));
    this->openclCollapseArrayW2 = cl::Buffer(context, CL_MEM_FETCH_STORE, openclCollapseArrayWSize*sizeof(Real));
    this->openclCollapseArrayResidual1 = cl::Buffer(
            context, CL_MEM_FETCH_STORE, openclCollapseArrayResidualSize1*sizeof(Real));
    this->openclCollapseArrayResidual2 = cl::Buffer(
            context, CL_MEM_FETCH_STORE, openclCollapseArrayResidualSize2*sizeof(Real));
    this->openclCollapseArrayNumCells1 = cl::Buffer(
            context, CL_MEM_FETCH_STORE, openclCollapseArrayResidualSize1*sizeof(unsigned int));
    this->openclCollapseArrayNumCells2 = cl::Buffer(
            context, CL_MEM_FETCH_STORE, openclCollapseArrayResidualSize2*sizeof(unsigned int));
}

CfdSolverOpencl::~CfdSolverOpencl() {
}

void CfdSolverOpencl::setBoundaryValues() {
    cl::LaunchArgs launchArgsYZ(
            queue, cl::EmptyRange, ClInterface::get()->alignedExtent2D(kmax, jmax, teamSize2D), teamSize2D);
    auto assignLeftRightBordersOpencl =
            cl::KernelInvoker<Real, Real, int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
                    assignLeftRightBordersOpenclKernel);
    assignLeftRightBordersOpencl(launchArgsYZ, T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);

    cl::LaunchArgs launchArgsXZ(
            queue, cl::EmptyRange, ClInterface::get()->alignedExtent2D(kmax, imax, teamSize2D), teamSize2D);
    auto assignDownUpBordersOpencl =
            cl::KernelInvoker<Real, Real, int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
                    assignDownUpBordersOpenclKernel);
    assignDownUpBordersOpencl(launchArgsXZ, T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);

    cl::LaunchArgs launchArgsXY(
            queue, cl::EmptyRange, ClInterface::get()->alignedExtent2D(jmax, imax, teamSize2D), teamSize2D);
    auto assignFrontBackBordersOpencl =
            cl::KernelInvoker<Real, Real, int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
                    assignFrontBackBordersOpenclKernel);
    assignFrontBackBordersOpencl(launchArgsXY, T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);


    cl::LaunchArgs launchArgs3D(
            queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent3D(kmax, jmax, imax, teamSize3D), teamSize3D);
    auto assignInternalUBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(
            assignInternalUBordersOpenclKernel);
    assignInternalUBordersOpencl(launchArgs3D, imax, jmax, kmax, U, Flag);
    auto assignInternalVBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(
            assignInternalVBordersOpenclKernel);
    assignInternalVBordersOpencl(launchArgs3D, imax, jmax, kmax, V, Flag);
    auto assignInternalWBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(
            assignInternalWBordersOpenclKernel);
    assignInternalWBordersOpencl(launchArgs3D, imax, jmax, kmax, W, Flag);
    auto assignInternalTBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(
            assignInternalTBordersOpenclKernel);
    assignInternalTBordersOpencl(launchArgs3D, imax, jmax, kmax, T, Flag);
}

void CfdSolverOpencl::setBoundaryValuesScenarioSpecific() {
    if (scenarioName == "driven_cavity") {
        cl::LaunchArgs launchArgsXZ(queue, cl::EmptyRange,
                ClInterface::get()->alignedExtent2D(kmax, imax + 1, teamSize2D), teamSize2D);
        auto assignDrivenCavityBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer>(
                assignDrivenCavityBordersOpenclKernel);
        assignDrivenCavityBordersOpencl(launchArgsXZ, imax, jmax, kmax, U);
    } else if (scenarioName == "flow_over_step") {
        cl::LaunchArgs launchArgsYZ(queue, cl::EmptyRange,
                ClInterface::get()->alignedExtent2D(kmax, jmax - (jmax / 2 + 1) + 1, teamSize2D), teamSize2D);
        auto assignFlowOverStepBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer, cl::Buffer>(
                assignFlowOverStepBordersOpenclKernel);
        assignFlowOverStepBordersOpencl(launchArgsYZ, imax, jmax, kmax, U, V, W);
    } else if (scenarioName == "single_tower") {
        cl::LaunchArgs launchArgsYZ(queue, cl::EmptyRange,
                ClInterface::get()->alignedExtent2D(kmax, jmax, teamSize2D), teamSize2D);
        auto assignSingleTowerBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer, cl::Buffer>(
                assignSingleTowerBordersOpenclKernel);
        assignSingleTowerBordersOpencl(launchArgsYZ, imax, jmax, kmax, U, V, W);
    } else if (scenarioName == "terrain_1" || scenarioName == "fuji_san" || scenarioName == "zugspitze") {
        cl::LaunchArgs launchArgsYZ(queue, cl::EmptyRange,
                ClInterface::get()->alignedExtent2D(kmax, jmax, teamSize2D), teamSize2D);
        auto assignMountainBordersOpencl =
                cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
                        assignMountainBordersOpenclKernel);
        assignMountainBordersOpencl(launchArgsYZ, imax, jmax, kmax, U, V, W, Flag);
    }
}

Real CfdSolverOpencl::calculateDt() {
    computeDtOpencl(
            Re, Pr, tau, dt, dx, dy, dz, imax, jmax, kmax, blockExtent1D,
            queue, teamSize1D, computeMaximumKernel, U, V, W,
            openclCollapseArrayU1, openclCollapseArrayU2,
            openclCollapseArrayV1, openclCollapseArrayV2,
            openclCollapseArrayW1, openclCollapseArrayW2,
            useTemperature);

    return dt;
}


void CfdSolverOpencl::calculateTemperature() {
    cl::Buffer temp = T;
    T = T_prev;
    T_prev = temp;

    cl::LaunchArgs launchArgs(
            queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent3D(kmax, jmax, imax, teamSize3D), teamSize3D);
    auto computeTemperatureOpencl = cl::KernelInvoker<Real, Real, Real, Real, Real, Real, Real, int, int, int,
            cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
            computeTemperatureOpenclKernel);
    computeTemperatureOpencl(launchArgs, Re, Pr, alpha, dt, dx, dy, dz, imax, jmax, kmax, U, V, W, T, T_prev, Flag);
}

void CfdSolverOpencl::calculateFgh() {
    cl::LaunchArgs launchArgs3D(
            queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent3D(kmax, jmax, imax, teamSize3D), teamSize3D);
    auto computeFghOpencl = cl::KernelInvoker<Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, int, int,
            int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
            computeFghOpenclKernel);
    computeFghOpencl(
            launchArgs3D, Re, GX, GY, GZ, alpha, beta, dt, dx, dy, dz, imax, jmax, kmax, U, V, W, T, F, G, H, Flag);

    cl::LaunchArgs launchArgsYZ(queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent2D(kmax, jmax, teamSize2D), teamSize2D);
    auto assignFBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(assignFBordersOpenclKernel);
    assignFBordersOpencl(launchArgsYZ, imax, jmax, kmax, U, F);

    cl::LaunchArgs launchArgsXZ(queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent2D(kmax, imax, teamSize2D), teamSize2D);
    auto assignGBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(assignGBordersOpenclKernel);
    assignGBordersOpencl(launchArgsXZ, imax, jmax, kmax, V, G);

    cl::LaunchArgs launchArgsXY(queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent2D(jmax, imax, teamSize2D), teamSize2D);
    auto assignHBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(assignHBordersOpenclKernel);
    assignHBordersOpencl(launchArgsXY, imax, jmax, kmax, W, H);
}

void CfdSolverOpencl::calculateRs() {
    cl::LaunchArgs launchArgs(
            queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent3D(kmax, jmax, imax, teamSize3D), teamSize3D);
    auto computeRsOpencl =
            cl::KernelInvoker<Real, Real, Real, Real, int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
                    computeRsOpenclKernel);
    computeRsOpencl(launchArgs, dt, dx, dy, dz, imax, jmax, kmax, F, G, H, RS);
}


void CfdSolverOpencl::executeSorSolver() {
    relaxSolverOpencl(
            omg, eps, itermax, linearSystemSolverType, shallWriteOutput, dx, dy, dz, imax, jmax, kmax,
            tileSizeX, blockExtentY, blockExtentZ, blockExtent1D,
            queue, teamSize1D, teamSize2D, teamSize3D,
            P, P_prev, RS, Flag,
            openclCollapseArrayResidual1, openclCollapseArrayResidual2,
            openclCollapseArrayNumCells1, openclCollapseArrayNumCells2,
            assignXYPlanesPressureBordersOpenclKernel, assignXZPlanesPressureBordersOpenclKernel,
            assignYZPlanesPressureBordersOpenclKernel,
            assignBorderConditionsPressureInDomainOpenclKernel, clonePressureOpenclKernel,
            collapseSumOpenclKernelReal, collapseSumOpenclKernelUint,
            relaxSolverIterationOpenclKernel, relaxSolverComputeResidualArrayOpenclKernel);
}

void CfdSolverOpencl::calculateUvw() {
    cl::LaunchArgs launchArgs(
            queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent3D(kmax, jmax, imax, teamSize3D), teamSize3D);
    auto computeUvwOpencl = cl::KernelInvoker<Real, Real, Real, Real, int, int, int, cl::Buffer, cl::Buffer,
            cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(computeUvwOpenclKernel);
    computeUvwOpencl(launchArgs, dt, dx, dy, dz, imax, jmax, kmax, U, V, W, F, G, H, P, Flag);
}

void CfdSolverOpencl::getDataForOutput(Real *U, Real *V, Real *W, Real *P, Real *T) {
    queue.readBufferAsync(this->U, CL_NONBLOCKING, 0, sizeof(Real)*(imax+1)*(jmax+2)*(kmax+2), (void*)U);
    queue.readBufferAsync(this->V, CL_NONBLOCKING, 0, sizeof(Real)*(imax+2)*(jmax+1)*(kmax+2), (void*)V);
    queue.readBufferAsync(this->W, CL_NONBLOCKING, 0, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+1), (void*)W);
    queue.readBufferAsync(this->P, CL_NONBLOCKING, 0, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2), (void*)P);
    queue.readBufferAsync(this->T, CL_NONBLOCKING, 0, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2), (void*)T);
    queue.finish();
}
