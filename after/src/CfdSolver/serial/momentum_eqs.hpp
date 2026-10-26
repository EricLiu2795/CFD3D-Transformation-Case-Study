
#ifndef FLUID3D_MOMENTUM_EQS_HPP
#define FLUID3D_MOMENTUM_EQS_HPP

#include "Defines.hpp"

struct MomentumConstants {
    Real Re, GX, GY, GZ, alpha, beta;
};

void computeFghCpp(
        const MomentumConstants &constants,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T, Real *F, Real *G, Real *H, FlagType *Flag);

void computeRsCpp(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *F, Real *G, Real *H, Real *RS);

enum TemperatureHandling {
    TEMPERATURE_DISABLED, TEMPERATURE_ENABLED
};

void computeDtCpp(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W,
        TemperatureHandling temperatureHandling);

void computeUvwCpp(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *F, Real *G, Real *H, Real *P, FlagType *Flag);

void computeTemperatureCpp(
        Real Re, Real Pr, Real alpha,
        Real dt, Real dx, Real dy, Real dz,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T, Real *T_prev, FlagType *Flag);

#endif
