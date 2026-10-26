
#ifndef FLUID3D_MOMENTUM_KERNELS_HPP
#define FLUID3D_MOMENTUM_KERNELS_HPP

#include "Defines.hpp"

void calculateFghCuda(
        Real Re, Real GX, Real GY, Real GZ, Real alpha, Real beta,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ,
        Real *U, Real *V, Real *W, Real *T, Real *F, Real *G, Real *H, FlagType *Flag);

__global__ void calculateRsCuda(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *F, Real *G, Real *H, Real *RS);

void calculateDtCuda(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax, int blockExtent1D,
        Real *U, Real *V, Real *W,
        Real *cudaReduceBufferU1, Real *cudaFoldArrayU2,
        Real *cudaReduceBufferV1, Real *cudaFoldArrayV2,
        Real *cudaReduceBufferW1, Real *cudaFoldArrayW2,
        bool useTemperature);

__global__ void calculateUvwCuda(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *F, Real *G, Real *H, Real *P, FlagType *Flag);

__global__ void calculateTemperatureCuda(
        Real Re, Real Pr, Real alpha,
        Real dt, Real dx, Real dy, Real dz,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T, const Real *T_prev, FlagType *Flag);

#endif
