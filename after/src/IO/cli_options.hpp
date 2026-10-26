
#ifndef FLUID3D_CLI_OPTIONS_HPP
#define FLUID3D_CLI_OPTIONS_HPP

#include <string>
#include "Defines.hpp"

class OutputFileRecorder;

void parseOptions(
        int argc, char *argv[], std::string &scenarioName, std::string &backendName,
        std::string &resultWriterType, bool &shallWriteOutput, LinearSystemSolverType &linearSystemSolverType,
        int &particleCount, bool &computeFlowlines, int &partsAlongI, int &jSplit, int &kSplit,
        int &tileSizeX, int &blockExtentY, int &blockExtentZ, int &blockExtent1D,
        int &gpuId, int &openclPlatformIndex);

#endif
