
#include <cmath>
#include "MpiHelpers.hpp"
#include "MacrosMpi.hpp"

void mpiStartup(
        int argc, char **argv,
        const MpiDecomposition &decomposition, int imax, int jmax, int kmax,
        int &localRank, int &il, int &iu, int &jl, int &ju, int &kl, int &ku,
        int &peerL, int &peerR, int &peerD, int &peerU, int &peerB, int &peerF,
        int &rankCoordI, int &rankCoordJ, int &rankCoordK, int &partCount)
{
    const int partsAlongI = decomposition.partsAlongI;
    const int jSplit = decomposition.jSplit, kSplit = decomposition.kSplit;

    DCI_Init(&argc, &argv);
    DCI_Comm_size(DCI_COMM_WORLD, &partCount);
    DCI_Comm_rank(DCI_COMM_WORLD, &localRank);

    assert(partCount == partsAlongI*jSplit*kSplit);

    if (localRank % partsAlongI == 0) {
        peerL = DCI_PROC_NULL;
    } else {
        peerL = localRank - 1;
    }

    if (localRank % partsAlongI == partsAlongI-1) {
        peerR = DCI_PROC_NULL;
    } else {
        peerR = localRank + 1;
    }

    if ((localRank % (partsAlongI*jSplit)) / partsAlongI == 0) {
        peerD = DCI_PROC_NULL;
    } else {
        peerD = localRank - partsAlongI;
    }

    if ((localRank % (partsAlongI*jSplit)) / partsAlongI == jSplit-1) {
        peerU = DCI_PROC_NULL;
    } else {
        peerU = localRank + partsAlongI;
    }

    if (localRank / (partsAlongI*jSplit) == 0) {
        peerB = DCI_PROC_NULL;
    } else {
        peerB = localRank - partsAlongI*jSplit;
    }

    if (localRank / (partsAlongI*jSplit) == kSplit-1) {
        peerF = DCI_PROC_NULL;
    } else {
        peerF = localRank + partsAlongI*jSplit;
    }


    rankCoordI = localRank % partsAlongI;
    rankCoordJ = (localRank % (partsAlongI*jSplit)) / partsAlongI;
    rankCoordK = localRank / (partsAlongI*jSplit);
}

MpiIndexRange mpiAssignRange(int processCount, int localRank, int numElements) {
    const int k = processCount;
    const int n = numElements;
    const int a = n / k;
    const int b = divCeil(n, k);
    const int pivotCount = a == b ? 0 : (n - k*b)/(a - b);

    if (localRank < pivotCount) {
        return {localRank * a + 1, (localRank + 1) * a};
    }
    return {pivotCount * a + (localRank - pivotCount) * b + 1,
            pivotCount * a + (localRank - pivotCount + 1) * b};
}

void mpiSwapCellData(
        Real *PT, int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF,
        Real *haloOut, Real *haloIn, DCI_Status *status) {
    int chunk;

    for (int j = jl; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(j - jl) * (ku - kl + 1) + (k - kl)] = PT[LINP(il,j,k)];
        }
    }
    chunk = (ju - jl + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerL, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerR, 1, DCI_COMM_WORLD, status);
    for (int j = jl; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            PT[LINP(iu + 1,j,k)] = haloIn[(j - jl) * (ku - kl + 1) + (k - kl)];
        }
    }


    for (int j = jl; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(j - jl) * (ku - kl + 1) + (k - kl)] = PT[LINP(iu,j,k)];
        }
    }
    chunk = (ju - jl + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerR, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerL, 1, DCI_COMM_WORLD, status);
    for (int j = jl; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            PT[LINP(il - 1,j,k)] = haloIn[(j - jl) * (ku - kl + 1) + (k - kl)];
        }
    }


    for (int i = il; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(i - il) * (ku - kl + 1) + (k - kl)] = PT[LINP(i,jl,k)];
        }
    }
    chunk = (iu - il + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerD, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerU, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            PT[LINP(i,ju + 1,k)] = haloIn[(i - il) * (ku - kl + 1) + (k - kl)];
        }
    }


    for (int i = il; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(i - il) * (ku - kl + 1) + (k - kl)] = PT[LINP(i,ju,k)];
        }
    }
    chunk = (iu - il + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerU, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerD, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            PT[LINP(i,jl - 1,k)] = haloIn[(i - il) * (ku - kl + 1) + (k - kl)];
        }
    }


    for (int i = il; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            haloOut[(i - il) * (ju - jl + 1) + (j - jl)] = PT[LINP(i,j,kl)];
        }
    }
    chunk = (iu - il + 1) * (ju - jl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerB, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerF, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            PT[LINP(i,j,ku + 1)] = haloIn[(i - il) * (ju - jl + 1) + (j - jl)];
        }
    }


    for (int i = il; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            haloOut[(i - il) * (ju - jl + 1) + (j - jl)] = PT[LINP(i,j,ku)];
        }
    }
    chunk = (iu - il + 1) * (ju - jl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerF, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerB, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            PT[LINP(i,j,kl - 1)] = haloIn[(i - il) * (ju - jl + 1) + (j - jl)];
        }
    }
}

void mpiSwapVelocity(
        Real *U, Real *V, Real *W, int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF,
        Real *haloOut, Real *haloIn, DCI_Status *status) {
    int chunk;

    for (int j = jl; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(j - jl) * (ku - kl + 1) + (k - kl)] = U[LINU(il,j,k)];
        }
    }
    chunk = (ju - jl + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerL, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerR, 1, DCI_COMM_WORLD, status);
    for (int j = jl; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            U[LINU(iu + 1,j,k)] = haloIn[(j - jl) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int j = jl - 1; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(j - jl + 1) * (ku - kl + 1) + (k - kl)] = V[LINV(il,j,k)];
        }
    }
    chunk = (ju - jl + 2) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerL, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerR, 1, DCI_COMM_WORLD, status);
    for (int j = jl - 1; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            V[LINV(iu + 1,j,k)] = haloIn[(j - jl + 1) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int j = jl; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl - 1; k <= ku; k++) {
            haloOut[(j - jl) * (ku - kl + 2) + (k - kl + 1)] = W[LINW(il,j,k)];
        }
    }
    chunk = (ju - jl + 1) * (ku - kl + 2);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerL, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerR, 1, DCI_COMM_WORLD, status);
    for (int j = jl; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl - 1; k <= ku; k++) {
            W[LINW(iu + 1,j,k)] = haloIn[(j - jl) * (ku - kl + 2) + (k - kl + 1)];
        }
    }



    for (int j = jl; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(j - jl) * (ku - kl + 1) + (k - kl)] = U[LINU(iu - 1,j,k)];
        }
    }
    chunk = (ju - jl + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerR, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerL, 1, DCI_COMM_WORLD, status);
    for (int j = jl; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            U[LINU(il - 2,j,k)] = haloIn[(j - jl) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int j = jl - 1; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(j - jl + 1) * (ku - kl + 1) + (k - kl)] = V[LINV(iu,j,k)];
        }
    }
    chunk = (ju - jl + 2) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerR, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerL, 1, DCI_COMM_WORLD, status);
    for (int j = jl - 1; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl; k <= ku; k++) {
            V[LINV(il - 1,j,k)] = haloIn[(j - jl + 1) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int j = jl; j <= ju && peerR != DCI_PROC_NULL; j++) {
        for (int k = kl - 1; k <= ku; k++) {
            haloOut[(j - jl) * (ku - kl + 2) + (k - kl + 1)] = W[LINW(iu,j,k)];
        }
    }
    chunk = (ju - jl + 1) * (ku - kl + 2);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerR, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerL, 1, DCI_COMM_WORLD, status);
    for (int j = jl; j <= ju && peerL != DCI_PROC_NULL; j++) {
        for (int k = kl - 1; k <= ku; k++) {
            W[LINW(il - 1,j,k)] = haloIn[(j - jl) * (ku - kl + 2) + (k - kl + 1)];
        }
    }



    for (int i = il - 1; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(i - il + 1) * (ku - kl + 1) + (k - kl)] = U[LINU(i,jl,k)];
        }
    }
    chunk = (iu - il + 2) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerD, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerU, 1, DCI_COMM_WORLD, status);
    for (int i = il - 1; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            U[LINU(i,ju + 1,k)] = haloIn[(i - il + 1) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int i = il; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(i - il) * (ku - kl + 1) + (k - kl)] = V[LINV(i,jl,k)];
        }
    }
    chunk = (iu - il + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerD, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerU, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            V[LINV(i,ju + 1,k)] = haloIn[(i - il) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int i = il; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl - 1; k <= ku; k++) {
            haloOut[(i - il) * (ku - kl + 2) + (k - kl + 1)] = W[LINW(i,jl,k)];
        }
    }
    chunk = (iu - il + 1) * (ku - kl + 2);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerD, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerU, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl - 1; k <= ku; k++) {
            W[LINW(i,ju + 1,k)] = haloIn[(i - il) * (ku - kl + 2) + (k - kl + 1)];
        }
    }



    for (int i = il - 1; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(i - il + 1) * (ku - kl + 1) + (k - kl)] = U[LINU(i,ju,k)];
        }
    }
    chunk = (iu - il + 2) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerU, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerD, 1, DCI_COMM_WORLD, status);
    for (int i = il - 1; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            U[LINU(i,jl - 1,k)] = haloIn[(i - il + 1) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int i = il; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            haloOut[(i - il) * (ku - kl + 1) + (k - kl)] = V[LINV(i,ju - 1,k)];
        }
    }
    chunk = (iu - il + 1) * (ku - kl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerU, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerD, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl; k <= ku; k++) {
            V[LINV(i,jl - 2,k)] = haloIn[(i - il) * (ku - kl + 1) + (k - kl)];
        }
    }

    for (int i = il; i <= iu && peerU != DCI_PROC_NULL; i++) {
        for (int k = kl - 1; k <= ku; k++) {
            haloOut[(i - il) * (ku - kl + 2) + (k - kl + 1)] = W[LINW(i,ju,k)];
        }
    }
    chunk = (iu - il + 1) * (ku - kl + 2);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerU, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerD, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerD != DCI_PROC_NULL; i++) {
        for (int k = kl - 1; k <= ku; k++) {
            W[LINW(i,jl - 1,k)] = haloIn[(i - il) * (ku - kl + 2) + (k - kl + 1)];
        }
    }



    for (int i = il - 1; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            haloOut[(i - il + 1) * (ju - jl + 1) + (j - jl)] = U[LINU(i,j,kl)];
        }
    }
    chunk = (iu - il + 2) * (ju - jl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerB, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerF, 1, DCI_COMM_WORLD, status);
    for (int i = il - 1; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            U[LINU(i,j,ku + 1)] = haloIn[(i - il + 1) * (ju - jl + 1) + (j - jl)];
        }
    }

    for (int i = il; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl - 1; j <= ju; j++) {
            haloOut[(i - il) * (ju - jl + 2) + (j - jl + 1)] = V[LINV(i,j,kl)];
        }
    }
    chunk = (iu - il + 1) * (ju - jl + 2);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerB, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerF, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl - 1; j <= ju; j++) {
            V[LINV(i,j,ku + 1)] = haloIn[(i - il) * (ju - jl + 2) + (j - jl + 1)];
        }
    }

    for (int i = il; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            haloOut[(i - il) * (ju - jl + 1) + (j - jl)] = W[LINW(i,j,kl)];
        }
    }
    chunk = (iu - il + 1) * (ju - jl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerB, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerF, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            W[LINW(i,j,ku + 1)] = haloIn[(i - il) * (ju - jl + 1) + (j - jl)];
        }
    }



    for (int i = il - 1; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            haloOut[(i - il + 1) * (ju - jl + 1) + (j - jl)] = U[LINU(i,j,ku)];
        }
    }
    chunk = (iu - il + 2) * (ju - jl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerF, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerB, 1, DCI_COMM_WORLD, status);
    for (int i = il - 1; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            U[LINU(i,j,kl - 1)] = haloIn[(i - il + 1) * (ju - jl + 1) + (j - jl)];
        }
    }

    for (int i = il; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl - 1; j <= ju; j++) {
            haloOut[(i - il) * (ju - jl + 2) + (j - jl + 1)] = V[LINV(i,j,ku)];
        }
    }
    chunk = (iu - il + 1) * (ju - jl + 2);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerF, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerB, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl - 1; j <= ju; j++) {
            V[LINV(i,j,kl - 1)] = haloIn[(i - il) * (ju - jl + 2) + (j - jl + 1)];
        }
    }

    for (int i = il; i <= iu && peerF != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            haloOut[(i - il) * (ju - jl + 1) + (j - jl)] = W[LINW(i,j,ku - 1)];
        }
    }
    chunk = (iu - il + 1) * (ju - jl + 1);
    DCI_Sendrecv(haloOut, chunk, MPI_REAL_FLUID3D, peerF, 1, haloIn, chunk, MPI_REAL_FLUID3D, peerB, 1, DCI_COMM_WORLD, status);
    for (int i = il; i <= iu && peerB != DCI_PROC_NULL; i++) {
        for (int j = jl; j <= ju; j++) {
            W[LINW(i,j,kl - 2)] = haloIn[(i - il) * (ju - jl + 1) + (j - jl)];
        }
    }
}

void mpiShutdown() {
    DCI_Barrier(DCI_COMM_WORLD);
    DCI_Finalize();
}
