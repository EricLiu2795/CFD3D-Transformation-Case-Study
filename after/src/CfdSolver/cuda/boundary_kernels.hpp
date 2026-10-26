
#ifndef FLUID3D_BOUNDARY_KERNELS_HPP
#define FLUID3D_BOUNDARY_KERNELS_HPP

#include <string>
#include "Defines.hpp"

void setBoundaryValuesCuda(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag);

void setBoundaryValuesScenarioSpecificCuda(
        const std::string &scenarioName,
        int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ,
        Real *U, Real *V, Real *W,
        FlagType *Flag);

#endif
