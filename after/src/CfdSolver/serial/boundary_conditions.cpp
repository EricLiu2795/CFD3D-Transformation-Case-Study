
#include "CfdSolver/serial/boundary_conditions.hpp"
#include "CfdSolver/cell_flags.hpp"

namespace {

Real temperatureBorderValue(FlagType flag, Real T_h, Real T_c, Real T_interior) {
    if (isHeated(flag)) {
        return 2 * T_h - T_interior;
    } else if (isChilled(flag)) {
        return 2 * T_c - T_interior;
    } else {
        return T_interior;
    }
}

}

struct FieldView {
    Real *values;
    int imax, jmax, kmax;
};

using ComponentField = FieldView;

void assignInternalUBordersCpp(
        const ComponentField &field,
        FlagType *Flag) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *U = field.values;
    #pragma omp parallel for
    for (int i = 1; i <= imax-1; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                if (isWet(Flag[LINFLAG(i, j, k)])) {
                    continue;
                }

                int R_count = 0;
                int L_count = 0;
                int R1_count = 0;
                int L1_count = 0;

                if (B_R(Flag[LINFLAG(i, j, k)])) {
                    U[LINU(i, j, k)] = Real(0);
                    R_count = 1;
                }

                if (B_L(Flag[LINFLAG(i, j, k)])) {
                    U[LINU(i - 1, j, k)] = Real(0);
                    L_count = 1;
                }

                if (B_U(Flag[LINFLAG(i, j, k)])) {
                    if (L_count == 0) {
                        U[LINU(i - 1, j, k)] = -U[LINU(i - 1, j + 1, k)];
                        L1_count = 1;
                    }
                    if (R_count == 0) {
                        U[LINU(i, j, k)] = -U[LINU(i, j + 1, k)];
                        R1_count = 1;
                    }
                }

                if (B_D(Flag[LINFLAG(i, j, k)])) {
                    if (L_count == 0) {
                        U[LINU(i - 1, j, k)] = -U[LINU(i - 1, j - 1, k)];
                        L1_count = 1;
                    }
                    if (R_count == 0) {
                        U[LINU(i, j, k)] = -U[LINU(i, j - 1, k)];
                        R1_count = 1;
                    }
                }

                if (B_B(Flag[LINFLAG(i, j, k)])) {
                    if (L_count == 0 && L1_count == 0) {
                        U[LINU(i - 1, j, k)] = -U[LINU(i - 1, j, k - 1)];
                    }
                    if (R_count == 0 && R1_count == 0) {
                        U[LINU(i, j, k)] = -U[LINU(i, j, k - 1)];
                    }
                }

                if (B_F(Flag[LINFLAG(i, j, k)])) {
                    if (L_count == 0 && L1_count == 0) {
                        U[LINU(i - 1, j, k)] = -U[LINU(i - 1, j, k + 1)];
                    }
                    if (R_count == 0 && R1_count == 0) {
                        U[LINU(i, j, k)] = -U[LINU(i, j, k + 1)];
                    }
                }
            }
        }
    }     
}

void assignInternalVBordersCpp(
        const ComponentField &field,
        FlagType *Flag) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *V = field.values;
    #pragma omp parallel for
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax-1; j++) {
            for (int k = 1; k <= kmax; k++) {
                int U_count = 0;
                int D_count = 0;
                int U1_count = 0;
                int D1_count = 0;

                if (!isWet(Flag[LINFLAG(i, j, k)])) {
                    if (B_U(Flag[LINFLAG(i, j, k)])) {
                        V[LINV(i, j, k)] = Real(0);
                        U_count = 1;
                    }

                    if (B_D(Flag[LINFLAG(i, j, k)])) {
                        V[LINV(i, j - 1, k)] = Real(0);
                        D_count = 1;
                    }

                    if (B_R(Flag[LINFLAG(i, j, k)])) {
                        if (D_count == 0) {
                            V[LINV(i, j - 1, k)] = -V[LINV(i + 1, j - 1, k)];
                            D1_count = 0;
                        }
                        if (U_count == 0) {
                            V[LINV(i, j, k)] = -V[LINV(i + 1, j, k)];
                            U1_count = 0;
                        }
                    }

                    if (B_L(Flag[LINFLAG(i, j, k)])) {
                        if (D_count == 0) {
                            V[LINV(i, j - 1, k)] = -V[LINV(i - 1, j - 1, k)];
                            D1_count = 0;
                        }
                        if (U_count == 0) {
                            V[LINV(i, j, k)] = -V[LINV(i - 1, j, k)];
                            U1_count = 0;
                        }
                    }

                    if (B_B(Flag[LINFLAG(i, j, k)])) {
                        if (D_count == 0 && D1_count == 0) {
                            V[LINV(i, j - 1, k)] = -V[LINV(i, j - 1, k - 1)];
                        }
                        if (U_count == 0 && U1_count == 0) {
                            V[LINV(i, j, k)] = -V[LINV(i, j, k - 1)];
                        }
                    }

                    if (B_F(Flag[LINFLAG(i, j, k)])) {
                        if (D_count == 0 && D1_count == 0) {
                            V[LINV(i, j - 1, k)] = -V[LINV(i, j - 1, k + 1)];
                        }
                        if (U_count == 0 && U1_count == 0) {
                            V[LINV(i, j, k)] = -V[LINV(i, j, k + 1)];
                        }
                    }
                }
            }
        }
    }   
}

void assignInternalWBordersCpp(
        const ComponentField &field,
        FlagType *Flag) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *W = field.values;
    #pragma omp parallel for
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax-1; k++) {
                int F_count = 0;
                int B_count = 0;
                int F1_count = 0;
                int B1_count = 0;

                if (!isWet(Flag[LINFLAG(i, j, k)])) {
                    if (B_B(Flag[LINFLAG(i, j, k)])) {
                        W[LINW(i, j, k - 1)] = Real(0);
                        B_count = 1;
                    }

                    if (B_F(Flag[LINFLAG(i, j, k)])) {
                        W[LINW(i, j, k)] = Real(0);
                        F_count = 1;
                    }

                    if (B_R(Flag[LINFLAG(i, j, k)])) {
                        if (B_count == 0) {
                            W[LINW(i, j, k - 1)] = -W[LINW(i + 1, j, k - 1)];
                            B1_count = 1;
                        }
                        if (F_count == 0) {
                            W[LINW(i, j, k)] = -W[LINW(i + 1, j, k)];
                            F1_count = 1;
                        }
                    }

                    if (B_L(Flag[LINFLAG(i, j, k)])) {
                        if (B_count == 0) {
                            W[LINW(i, j, k - 1)] = -W[LINW(i - 1, j, k - 1)];
                            B1_count = 1;
                        }
                        if (F_count == 0) {
                            W[LINW(i, j, k)] = -W[LINW(i - 1, j, k)];
                            F1_count = 1;
                        }
                    }

                    if (B_U(Flag[LINFLAG(i, j, k)])) {
                        if (B_count == 0 && B1_count == 0) {
                            W[LINW(i, j, k - 1)] = -W[LINW(i, j + 1, k - 1)];
                        }
                        if (F_count == 0 && F1_count == 0) {
                            W[LINW(i, j, k)] = -W[LINW(i, j + 1, k)];
                        }
                    }

                    if (B_D(Flag[LINFLAG(i, j, k)])) {
                        if (B_count == 0 && B1_count == 0) {
                            W[LINW(i, j, k - 1)] = -W[LINW(i, j - 1, k - 1)];
                        }
                        if (F_count == 0 && F1_count == 0) {
                            W[LINW(i, j, k)] = -W[LINW(i, j - 1, k)];
                        }
                    }
                }
            }     
        }
    }      
}

void assignInternalTBordersCpp(
        const ComponentField &field,
        FlagType *Flag) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *T = field.values;
    struct NeighborTap {
        bool (*isConnected)(unsigned int);
        int di, dj, dk;
    };
    static const NeighborTap temperatureNeighbors[] = {
        {B_R,  1,  0,  0},
        {B_L, -1,  0,  0},
        {B_U,  0,  1,  0},
        {B_D,  0, -1,  0},
        {B_B,  0,  0, -1},
        {B_F,  0,  0,  1},
    };

    #pragma omp parallel for
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                if (isWet(Flag[LINFLAG(i, j, k)])) {
                    continue;
                }

                int numDirectFlag = 0;
                Real T_prev = Real(0);

                for (const NeighborTap &tap : temperatureNeighbors) {
                    if (tap.isConnected(Flag[LINFLAG(i, j, k)])) {
                        T_prev = T[LINT(i + tap.di, j + tap.dj, k + tap.dk)];
                        numDirectFlag++;
                    }
                }

                if (numDirectFlag == 0) {
                    T[LINT(i,j,k)] = 0;
                } else {
                    T[LINT(i,j,k)] = T_prev / Real(numDirectFlag);
                }
            }     
        }
    }      
}

void assignLeftRightBordersCpp(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    #pragma omp parallel for
    for (int j = 1; j <= jmax; j++) {
        for (int k = 1; k <= kmax; k++) {
            if (isSticky(Flag[LINFLAG(0,j,k)])) {
                U[LINU(0,j,k)] = 0.0;
                V[LINV(0,j,k)] = -V[LINV(1,j,k)];
                W[LINW(0,j,k)] = -W[LINW(1,j,k)];
            } else if (isFrictionless(Flag[LINFLAG(0,j,k)])) {
                U[LINU(0,j,k)] = 0.0;
                V[LINV(0,j,k)] = V[LINV(1,j,k)];
                W[LINW(0,j,k)] = W[LINW(1,j,k)];
            } else if (isOutlet(Flag[LINFLAG(0,j,k)])) {
                U[LINU(0,j,k)] = U[LINU(1,j,k)];
                V[LINV(0,j,k)] = V[LINV(1,j,k)];
                W[LINW(0,j,k)] = W[LINW(1,j,k)];
            }

            if (isSticky(Flag[LINFLAG(imax+1,j,k)])) {
                U[LINU(imax,j,k)] = 0.0;
                V[LINV(imax+1,j,k)] = -V[LINV(imax,j,k)];
                W[LINW(imax+1,j,k)] = -W[LINW(imax,j,k)];
            } else if (isFrictionless(Flag[LINFLAG(imax+1,j,k)])) {
                U[LINU(imax,j,k)] = 0.0;
                V[LINV(imax+1,j,k)] = V[LINV(imax,j,k)];
                W[LINW(imax+1,j,k)] = W[LINW(imax,j,k)];
            } else if (isOutlet(Flag[LINFLAG(imax+1,j,k)])) {
                U[LINU(imax,j,k)] = U[LINU(imax-1,j,k)];
                V[LINV(imax+1,j,k)] = V[LINV(imax,j,k)];
                W[LINW(imax+1,j,k)] = W[LINW(imax,j,k)];
            }

            T[LINT(0,j,k)] = temperatureBorderValue(Flag[LINFLAG(0,j,k)], T_h, T_c, T[LINT(1,j,k)]);

            T[LINT(imax+1,j,k)] = temperatureBorderValue(Flag[LINFLAG(imax+1,j,k)], T_h, T_c, T[LINT(imax,j,k)]);
        }
    }
}

void assignDownUpBordersCpp(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    #pragma omp parallel for
    for (int i = 1; i <= imax; i++) {
        for (int k = 1; k <= kmax; k++) {
            if (isSticky(Flag[LINFLAG(i,0,k)])) {
                U[LINU(i,0,k)] = -U[LINU(i,1,k)];
                V[LINV(i,0,k)] = 0.0;
                W[LINW(i,0,k)] = -W[LINW(i,1,k)];
            } else if (isFrictionless(Flag[LINFLAG(i,0,k)])) {
                U[LINU(i,0,k)] = U[LINU(i,1,k)];
                V[LINV(i,0,k)] = 0.0;
                W[LINW(i,0,k)] = W[LINW(i,1,k)];
            } else if (isOutlet(Flag[LINFLAG(i,0,k)])) {
                U[LINU(i,0,k)] = U[LINU(i,1,k)];
                V[LINV(i,0,k)] = V[LINV(i,1,k)];
                W[LINW(i,0,k)] = W[LINW(i,1,k)];
            }

            if (isSticky(Flag[LINFLAG(i,jmax+1,k)])) {
                U[LINU(i,jmax+1,k)] = -U[LINU(i,jmax,k)];
                V[LINV(i,jmax,k)] = 0.0;
                W[LINW(i,jmax+1,k)] = -W[LINW(i,jmax,k)];
            } else if (isFrictionless(Flag[LINFLAG(i,jmax+1,k)])) {
                U[LINU(i,jmax+1,k)] = U[LINU(i,jmax,k)];
                V[LINV(i,jmax,k)] = 0.0;
                W[LINW(i,jmax+1,k)] = W[LINW(i,jmax,k)];
            } else if (isOutlet(Flag[LINFLAG(i,jmax+1,k)])) {
                U[LINU(i,jmax+1,k)] = U[LINU(i,jmax,k)];
                V[LINV(i,jmax,k)] = V[LINV(i,jmax-1,k)];
                W[LINW(i,jmax+1,k)] = W[LINW(i,jmax,k)];
            }

            T[LINT(i,0,k)] = temperatureBorderValue(Flag[LINFLAG(i,0,k)], T_h, T_c, T[LINT(i,1,k)]);

            T[LINT(i,jmax+1,k)] = temperatureBorderValue(Flag[LINFLAG(i,jmax+1,k)], T_h, T_c, T[LINT(i,jmax,k)]);
        }
    }
}

void assignFrontBackBordersCpp(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    #pragma omp parallel for
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            if (isSticky(Flag[LINFLAG(i,j,0)])) {
                U[LINU(i,j,0)] = -U[LINU(i,j,1)];
                V[LINV(i,j,0)] = -V[LINV(i,j,1)];
                W[LINW(i,j,0)] = 0.0;
            }
            else if (isFrictionless(Flag[LINFLAG(i,j,0)])) {
                U[LINU(i,j,0)] = U[LINU(i,j,1)];
                V[LINV(i,j,0)] = V[LINV(i,j,1)];
                W[LINW(i,j,0)] = 0.0;
            }
            else if (isOutlet(Flag[LINFLAG(i,j,0)])) {
                U[LINU(i,j,0)] = U[LINU(i,j,1)];
                V[LINV(i,j,0)] = V[LINV(i,j,1)];
                W[LINW(i,j,0)] = W[LINW(i,j,1)];
            }

            if (isSticky(Flag[LINFLAG(i,j,kmax+1)])) {
                U[LINU(i,j,kmax+1)] = -U[LINU(i,j,kmax)];
                V[LINV(i,j,kmax+1)] = -V[LINV(i,j,kmax)];
                W[LINW(i,j,kmax)] = 0.0;
            }
            else if (isFrictionless(Flag[LINFLAG(i,j,kmax+1)])) {
                U[LINU(i,j,kmax+1)] = U[LINU(i,j,kmax)];
                V[LINV(i,j,kmax+1)] = V[LINV(i,j,kmax)];
                W[LINW(i,j,kmax)] = 0.0;
            }
            else if (isOutlet(Flag[LINFLAG(i,j,kmax+1)])) {
                U[LINU(i,j,kmax+1)] = U[LINU(i,j,kmax)];
                V[LINV(i,j,kmax+1)] = V[LINV(i,j,kmax)];
                W[LINW(i,j,kmax)] = W[LINW(i,j,kmax-1)];
            }

            T[LINT(i,j,0)] = temperatureBorderValue(Flag[LINFLAG(i,j,0)], T_h, T_c, T[LINT(i,j,1)]);

            T[LINT(i,j,kmax+1)] = temperatureBorderValue(Flag[LINFLAG(i,j,kmax+1)], T_h, T_c, T[LINT(i,j,kmax)]);
        }
    }
}

void assignBorderValuesCpp(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    assignLeftRightBordersCpp(T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);
    assignDownUpBordersCpp(T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);
    assignFrontBackBordersCpp(T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);
    assignInternalUBordersCpp({U, imax, jmax, kmax}, Flag);
    assignInternalVBordersCpp({V, imax, jmax, kmax}, Flag);
    assignInternalWBordersCpp({W, imax, jmax, kmax}, Flag);
    assignInternalTBordersCpp({T, imax, jmax, kmax}, Flag);
}
