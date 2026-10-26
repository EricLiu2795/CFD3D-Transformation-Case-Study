
#ifndef FLUID3D_POISSON_KERNELS_HPP
#define FLUID3D_POISSON_KERNELS_HPP

#include "Defines.hpp"

void sorSolverCuda(
        Real omg, Real eps, int itermax, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
        Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ, int blockExtent1D,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag,
        Real *cudaReduceBufferResidual1, Real *cudaReduceBufferResidual2,
        unsigned int *cudaReduceBufferCellCount1, unsigned int *cudaReduceBufferCellCount2);

#endif
