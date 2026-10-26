
#ifndef FLUID3D_VELOCITYMPI_HPP
#define FLUID3D_VELOCITYMPI_HPP

#include "Defines.hpp"

void computeFluxMpi(
        Real Re, Real GX, Real GY, Real GZ, Real alpha, Real beta,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W, const Real *T, Real *F, Real *G, Real *H, FlagType *Flag);

void computeRsMpi(
        Real dx, Real dy, Real dz, Real dt, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        const Real *F, const Real *G, const Real *H, Real *RS);

void computeDtMpi(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W,
        bool useTemperature);

void computeVelocityMpi(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF, Real *haloOut, Real *haloIn,
        Real *U, Real *V, Real *W, const Real *F, const Real *G, const Real *H, const Real *P, FlagType *Flag);

void computeTemperatureMpi(
        Real Re, Real Pr, Real alpha,
        Real dt, Real dx, Real dy, Real dz,
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF, Real *haloOut, Real *haloIn,
        Real *U, Real *V, Real *W, Real *T, const Real *T_prev, FlagType *Flag);

#endif
