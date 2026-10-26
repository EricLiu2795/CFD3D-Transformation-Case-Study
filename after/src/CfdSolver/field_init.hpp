
#ifndef FLUID3D_FIELD_INIT_HPP
#define FLUID3D_FIELD_INIT_HPP

#include "Defines.hpp"

void initFields(
        Real UI, Real VI, Real WI, Real PI, Real TI, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *P, Real *T, FlagType *Flag);

void initFieldsMpi(
        Real UI, Real VI, Real WI, Real PI, Real TI, int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W, Real *P, Real *T, FlagType *Flag);

#endif
