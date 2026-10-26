
#include <iostream>
#include <cmath>
#include <cstring>
#include "CfdSolver/cell_flags.hpp"
#include "MpiHelpers.hpp"
#include "SorSolverMpi.hpp"
#include "MacrosMpi.hpp"

void relaxSolverIterationMpi(
        Real omg, Real dx, Real dy, Real dz, Real coeff, LinearSystemSolverType linearSystemSolverType,
        int imax, int jmax, int kmax, int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF, Real *haloOut, Real *haloIn,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag, Real &residual) {
    Real dx2 = dx*dx, dy2 = dy*dy, dz2 = dz*dz;
    if (kl == 1) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                P[LINP(i, j, 0)] = P[LINP(i, j, 1)];
            }
        }
    }
    if (ku == kmax) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                P[LINP(i, j, kmax + 1)] = P[LINP(i, j, kmax)];
            }
        }
    }

    if (jl == 1) {
        for (int i = il; i <= iu; i++) {
            for (int k = kl; k <= ku; k++) {
                P[LINP(i,0,k)] = P[LINP(i,1,k)];
            }
        }
    }
    if (ju == jmax) {
        for (int i = il; i <= iu; i++) {
            for (int k = kl; k <= ku; k++) {
                P[LINP(i,jmax+1,k)] = P[LINP(i,jmax,k)];
            }
        }
    }

    if (il == 1) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                P[LINP(0,j,k)] = P[LINP(1,j,k)];
            }
        }
    }
    if (iu == imax) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                P[LINP(imax+1,j,k)] = P[LINP(imax,j,k)];
            }
        }
    }

    static bool (*const faceConnected[])(unsigned int) = {B_R, B_L, B_U, B_D, B_B, B_F};
    static const int faceDi[] = { 1, -1,  0,  0,  0,  0};
    static const int faceDj[] = { 0,  0,  1, -1,  0,  0};
    static const int faceDk[] = { 0,  0,  0,  0, -1,  1};

    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                int numDirectFlag = 0;
                Real P_prev = Real(0);

                if (!isWet(Flag[LINFLAG(i, j, k)])) {
                    for (int n = 0; n < 6; n++) {
                        if (faceConnected[n](Flag[LINFLAG(i, j, k)])) {
                            P_prev += P[LINP(i + faceDi[n], j + faceDj[n], k + faceDk[n])];
                            numDirectFlag++;
                        }
                    }

                    if (numDirectFlag == 0) {
                        P[LINP(i, j, k)] = 0;
                    } else {
                        P[LINP(i, j, k)] = P_prev / Real(numDirectFlag);
                    }
                }
            }     
        }
    }


    if (linearSystemSolverType == LINEAR_SOLVER_JACOBI) {
        for (int i = il-1; i <= iu+1; i++) {
            for (int j = jl-1; j <= ju+1; j++) {
                for (int k = kl-1; k <= ku+1; k++) {
                    P_prev[LINP(i, j, k)] = P[LINP(i, j, k)];
                }
            }
        }
    }


    if (linearSystemSolverType == POISSON_RELAX || linearSystemSolverType == POISSON_GAUSS_SEIDEL) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                for (int k = kl; k <= ku; k++) {
                    if (isWet(Flag[LINFLAG(i,j,k)])){
                        P[LINP(i,j,k)] = (Real(1.0) - omg)*P[LINP(i,j,k)] + coeff *
                                ((P[LINP(i+1,j,k)]+P[LINP(i-1,j,k)])/dx2
                                + (P[LINP(i,j+1,k)]+P[LINP(i,j-1,k)])/dy2
                                + (P[LINP(i,j,k+1)]+P[LINP(i,j,k-1)])/dz2
                                - RS[LINRS(i,j,k)]);
                    }
                }
            }
        }
    } else if (linearSystemSolverType == LINEAR_SOLVER_JACOBI) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                for (int k = kl; k <= ku; k++) {
                    if (isWet(Flag[LINFLAG(i,j,k)])) {
                        P[LINP(i,j,k)] = (Real(1.0) - omg)*P_prev[LINP(i,j,k)] + coeff *
                                ((P_prev[LINP(i+1,j,k)]+P_prev[LINP(i-1,j,k)])/dx2
                                 + (P_prev[LINP(i,j+1,k)]+P_prev[LINP(i,j-1,k)])/dy2
                                 + (P_prev[LINP(i,j,k+1)]+P_prev[LINP(i,j,k-1)])/dz2
                                 - RS[LINRS(i,j,k)]);
                    }
                }
            }
        }
    }


    DCI_Status status;
    mpiSwapCellData(P, il, iu, jl, ju, kl, ku, peerL, peerR, peerD, peerU, peerB, peerF, haloOut, haloIn, &status);

    residual = 0;
    int fluidCellCount = 0;
    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if (isWet(Flag[LINFLAG(i,j,k)])){
                    residual += SQR(
                               (P[LINP(i+1,j,k)] - Real(2.0)*P[LINP(i,j,k)] + P[LINP(i-1,j,k)])/dx2
                             + (P[LINP(i,j+1,k)] - Real(2.0)*P[LINP(i,j,k)] + P[LINP(i,j-1,k)])/dy2
                             + (P[LINP(i,j,k+1)] - Real(2.0)*P[LINP(i,j,k)] + P[LINP(i,j,k-1)])/dz2
                             - RS[LINRS(i,j,k)]
                    );
                    fluidCellCount++;
                }
            }
        }
    }

    DCI_Allreduce(DCI_IN_PLACE, &fluidCellCount, 1, DCI_INT, DCI_SUM, DCI_COMM_WORLD);
    DCI_Allreduce(DCI_IN_PLACE, &residual, 1, MPI_REAL_FLUID3D, DCI_SUM, DCI_COMM_WORLD);
    residual = std::sqrt(residual/fluidCellCount);
}

void relaxSolverMpi(
        int localRank, Real omg, Real eps, int itermax, LinearSystemSolverType linearSystemSolverType,
        bool shallWriteOutput, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF, Real *haloOut, Real *haloIn,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag) {
    if (linearSystemSolverType == POISSON_RELAX || linearSystemSolverType == POISSON_RELAX_PARALLEL) {
        omg = 1.2;
    } else {
        omg = 1.0;
    }

    const Real coeff = omg / (Real(2.0) * (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy) + Real(1.0) / (dz*dz)));
    Real residual = Real(1e9);
    int it = 0;

    while (it < itermax && residual > eps) {
        relaxSolverIterationMpi(
                omg, dx, dy, dz, coeff, linearSystemSolverType,
                imax, jmax, kmax, il, iu, jl, ju, kl, ku,
                peerL, peerR, peerD, peerU, peerB, peerF, haloOut, haloIn,
                P, P_prev, RS, Flag, residual);
        it++;
    }

    if (localRank == 0) {
        if (((residual > eps && it == itermax) || std::isnan(residual)) && shallWriteOutput) {
            std::cerr << "\nSOR solver reached maximum number of iterations without converging (res: "
                      << residual << ")." << std::endl;
        }
        if (std::isnan(residual)) {
            std::cerr << "\nResidual in SOR solver is not a number." << std::endl;
        }
    }
    if (std::isnan(residual)) {
        mpiShutdown();
        exit(1);
    }
}
