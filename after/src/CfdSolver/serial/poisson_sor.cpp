
#include <iostream>
#include <cmath>
#include <cstring>
#include "CfdSolver/cell_flags.hpp"
#include "CfdSolver/serial/poisson_sor.hpp"

void relaxSolverSweepCpp(
        Real omg, Real dx, Real dy, Real dz, Real coeff, int imax, int jmax, int kmax,
        LinearSystemSolverType linearSystemSolverType,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag, Real &residual) {
    Real dx2 = dx*dx, dy2 = dy*dy, dz2 = dz*dz;
    #pragma omp parallel for shared(imax, jmax, kmax, P) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            P[LINP(i,j,0)] = P[LINP(i,j,1)];
            P[LINP(i,j,kmax+1)] = P[LINP(i,j,kmax)];
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, P) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int k = 1; k <= kmax; k++) {
            P[LINP(i,0,k)] = P[LINP(i,1,k)];
            P[LINP(i,jmax+1,k)] = P[LINP(i,jmax,k)];
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, P) default(none)
    for (int j = 1; j <= jmax; j++) {
        for (int k = 1; k <= kmax; k++) {
            P[LINP(0,j,k)] = P[LINP(1,j,k)];
            P[LINP(imax+1,j,k)] = P[LINP(imax,j,k)];
        }
    }

    struct NeighborTap {
        bool (*isConnected)(unsigned int);
        int di, dj, dk;
    };
    static const NeighborTap pressureNeighbors[] = {
        {B_R,  1,  0,  0},
        {B_L, -1,  0,  0},
        {B_U,  0,  1,  0},
        {B_D,  0, -1,  0},
        {B_B,  0,  0, -1},
        {B_F,  0,  0,  1},
    };

    #pragma omp parallel for shared(imax, jmax, kmax, P, Flag, pressureNeighbors) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                int numDirectFlag = 0;
                Real P_prev = Real(0);

                if (!isWet(Flag[LINFLAG(i, j, k)])) {
                    for (const NeighborTap &tap : pressureNeighbors) {
                        if (tap.isConnected(Flag[LINFLAG(i, j, k)])) {
                            P_prev += P[LINP(i + tap.di, j + tap.dj, k + tap.dk)];
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
        #pragma omp parallel for shared(imax, jmax, kmax, P, P_prev) default(none)
        for (int i = 0; i <= imax+1; i++) {
            for (int j = 0; j <= jmax+1; j++) {
                for (int k = 0; k <= kmax+1; k++) {
                    P_prev[LINP(i, j, k)] = P[LINP(i, j, k)];
                }
            }
        }
    }


    if (linearSystemSolverType == POISSON_RELAX || linearSystemSolverType == POISSON_GAUSS_SEIDEL) {
        for (int i = 1; i <= imax; i++) {
            for (int j = 1; j <= jmax; j++) {
                for (int k = 1; k <= kmax; k++) {
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
    } else if (linearSystemSolverType == POISSON_RELAX_PARALLEL
            || linearSystemSolverType == LINEAR_SOLVER_GAUSS_SEIDEL_PARALLEL) {
        #pragma omp parallel for shared(imax, jmax, kmax, dx2, dy2, dz2, omg, coeff, P, P_prev, RS, Flag) default(none)
        for (int i = 1; i <= imax; i++) {
            for (int j = 1; j <= jmax; j++) {
                for (int k = 1; k <= kmax; k++) {
                    if (isWet(Flag[LINFLAG(i,j,k)])){
                        P_prev[LINP(i,j,k)] = (Real(1.0) - omg)*P[LINP(i,j,k)] + coeff *
                                ((P[LINP(i+1,j,k)])/dx2
                                + (P[LINP(i,j+1,k)])/dy2
                                + (P[LINP(i,j,k+1)])/dz2
                                - RS[LINRS(i,j,k)]);
                    }
                }
            }
        }

        for (int i = 1; i <= imax; i++) {
            for (int j = 1; j <= jmax; j++) {
                for (int k = 1; k <= kmax; k++) {
                    if (isWet(Flag[LINFLAG(i,j,k)])){
                        P[LINP(i,j,k)] = P_prev[LINP(i,j,k)] + coeff *
                                ((P[LINP(i-1,j,k)])/dx2
                                 + (P[LINP(i,j-1,k)])/dy2
                                 + (P[LINP(i,j,k-1)])/dz2);
                    }
                }
            }
        }
    } else if (linearSystemSolverType == LINEAR_SOLVER_JACOBI) {
        #pragma omp parallel for shared(imax, jmax, kmax, dx2, dy2, dz2, omg, coeff, P, P_prev, RS, Flag) default(none)
        for (int i = 1; i <= imax; i++) {
            for (int j = 1; j <= jmax; j++) {
                for (int k = 1; k <= kmax; k++) {
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


    residual = Real(0.0);
    int fluidCellCount = 0;
#if _OPENMP >= 201107
    #pragma omp parallel for shared(imax, jmax, kmax, dx2, dy2, dz2, P, RS, Flag) \
    reduction(+: residual) reduction(+: fluidCellCount) default(none)
#endif
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
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

    residual = std::sqrt(residual / Real(fluidCellCount));
}

void relaxSolverCpp(
        Real omg, Real eps, int itermax, LinearSystemSolverType linearSystemSolverType, SolverLogging solverLogging,
        Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag) {
    if (linearSystemSolverType == POISSON_RELAX || linearSystemSolverType == POISSON_RELAX_PARALLEL) {
        omg = Real(1.2);
    } else {
        omg = Real(1.0);
    }

    const Real coeff = omg / (Real(2.0) * (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy) + Real(1.0) / (dz*dz)));

    Real residual = Real(1e9);
    int it = 0;

    while (it < itermax && residual > eps) {
        relaxSolverSweepCpp(
                omg, dx, dy, dz, coeff, imax, jmax, kmax, linearSystemSolverType,
                P, P_prev, RS, Flag, residual);
        it++;
    }

    if (((residual > eps && it == itermax) || std::isnan(residual)) && solverLogging == SOLVER_LOG_WARNINGS) {
        std::cerr << "\nSOR solver reached maximum number of iterations without converging (res: "
                << residual << ")." << std::endl;
    }
    if (std::isnan(residual)) {
        std::cerr << "\nResidual in SOR solver is not a number." << std::endl;
        exit(1);
    }
}
