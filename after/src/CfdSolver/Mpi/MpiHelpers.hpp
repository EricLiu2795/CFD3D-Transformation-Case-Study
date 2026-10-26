
#ifndef FLUID3D_MPIHELPERS_HPP
#define FLUID3D_MPIHELPERS_HPP

#include "mpi.h"
#include "Defines.hpp"

struct MpiDecomposition {
    int partsAlongI, jSplit, kSplit;
};

struct MpiIndexRange {
    int lower, upper;
};

void mpiStartup(
        int argc, char **argv,
        const MpiDecomposition &decomposition, int imax, int jmax, int kmax,
        int &localRank, int &il, int &iu, int &jl, int &ju, int &kl, int &ku,
        int &peerL, int &peerR, int &peerD, int &peerU, int &peerB, int &peerF,
        int &rankCoordI, int &rankCoordJ, int &rankCoordK, int &partCount);

MpiIndexRange mpiAssignRange(int processCount, int localRank, int numElements);

void mpiSwapCellData(
        Real *PT, int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF,
        Real *haloOut, Real *haloIn, DCI_Status *status);

void mpiSwapVelocity(
        Real *U, Real *V, Real *W, int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF,
        Real *haloOut, Real *haloIn, DCI_Status *status);

void mpiShutdown();

#endif
