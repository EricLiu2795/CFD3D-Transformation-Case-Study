
#include <cstring>
#include <iostream>
#include "IO/cli_options.hpp"

void parseOptions(
        int argc, char *argv[], std::string &scenarioName, std::string &backendName,
        std::string &resultWriterType, bool &shallWriteOutput, LinearSystemSolverType &linearSystemSolverType,
        int &particleCount, bool &computeFlowlines, int &partsAlongI, int &jSplit, int &kSplit,
        int &tileSizeX, int &blockExtentY, int &blockExtentZ, int &blockExtent1D,
        int &gpuId, int &openclPlatformIndex) {
    scenarioName = "driven_cavity";
    backendName = "cpp";
    shallWriteOutput = true;
    resultWriterType = "vtk";
    linearSystemSolverType = LINEAR_SOLVER_JACOBI;
    particleCount = 500;
    computeFlowlines = false;
    partsAlongI = jSplit = kSplit = 1;
    tileSizeX = blockExtentY = 8;
    blockExtentZ = 4;
    blockExtent1D = tileSizeX * blockExtentY * blockExtentZ;
    openclPlatformIndex = 0;

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--scenario") == 0 && i != argc - 1) {
            scenarioName = argv[i+1];
        } else if (strcmp(argv[i], "--solver") == 0 && i != argc - 1) {
            backendName = argv[i+1];
        } else if (strcmp(argv[i], "--outputformat") == 0 && i != argc - 1) {
            resultWriterType = argv[i+1];
        } else if (strcmp(argv[i], "--numparticles") == 0 && i != argc - 1) {
            particleCount = std::stoi(argv[i+1]);
        } else if (strcmp(argv[i], "--linsolver") == 0 && i != argc - 1) {
            if (strcmp(argv[i+1], "jacobi") == 0) {
                linearSystemSolverType = LINEAR_SOLVER_JACOBI;
            } else if (strcmp(argv[i+1], "sor") == 0) {
                linearSystemSolverType = POISSON_RELAX;
            } else if (strcmp(argv[i+1], "gauss-seidel") == 0) {
                linearSystemSolverType = POISSON_GAUSS_SEIDEL;
            } else {
                std::cerr << "Specified invalid linear systems solver name." << std::endl;
                exit(1);
            }
            shallWriteOutput = strcmp(argv[i + 1], "false") != 0;
        } else if (strcmp(argv[i], "--output") == 0 && i != argc - 1) {
            shallWriteOutput = strcmp(argv[i + 1], "false") != 0;
        } else if (strcmp(argv[i], "--tracestreamlines") == 0 && i != argc - 1) {
            computeFlowlines = strcmp(argv[i + 1], "false") != 0;
        } else if (strcmp(argv[i], "--numproc") == 0 && i < argc - 3) {
            partsAlongI = std::stoi(argv[i+1]);
            jSplit = std::stoi(argv[i+2]);
            kSplit = std::stoi(argv[i+3]);
            i += 2;
        } else if (strcmp(argv[i], "--blocksize") == 0 && i < argc - 3) {
            tileSizeX = std::stoi(argv[i+1]);
            blockExtentY = std::stoi(argv[i+2]);
            blockExtentZ = std::stoi(argv[i+3]);
            blockExtent1D = tileSizeX*blockExtentY*blockExtentZ;
        } else if (strcmp(argv[i], "--gpu") == 0 && i < argc - 1) {
            gpuId = std::stoi(argv[i+1]);
        } else if (strcmp(argv[i], "--platformid") == 0 && i < argc - 1) {
            openclPlatformIndex = std::stoi(argv[i+1]);
        }
    }
}
