
#ifndef FLUID3D_RELAXSOLVERMPI_HPP
#define FLUID3D_RELAXSOLVERMPI_HPP

#include "Defines.hpp"

void relaxSolverMpi(
        int localRank, Real omg, Real eps, int itermax, LinearSystemSolverType linearSystemSolverType,
        bool shallWriteOutput, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF, Real *haloOut, Real *haloIn,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag);

#endif
