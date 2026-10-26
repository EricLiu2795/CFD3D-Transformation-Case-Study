
#ifndef FLUID3D_BORDERVALUESMPI_HPP
#define FLUID3D_BORDERVALUESMPI_HPP

#include <string>
#include "Defines.hpp"

struct MpiSubdomainBounds {
    int il, iu, jl, ju, kl, ku;
};

void setBorderValuesMpi(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        const MpiSubdomainBounds &bounds,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag);

void setBorderValuesCaseSpecificMpi(
        const std::string &scenarioName,
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W,
        FlagType *Flag);

#endif
