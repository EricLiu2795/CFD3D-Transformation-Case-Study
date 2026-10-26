
#ifndef FLUID3D_BOUNDARY_CONDITIONS_HPP
#define FLUID3D_BOUNDARY_CONDITIONS_HPP

#include <string>
#include "Defines.hpp"

struct GridDimensions {
    int imax, jmax, kmax;
};

void assignBorderValuesCpp(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag);

void assignScenarioBorderValuesCpp(
        const std::string &scenarioName,
        const GridDimensions &grid,
        Real *U, Real *V, Real *W,
        FlagType *Flag);

#endif
