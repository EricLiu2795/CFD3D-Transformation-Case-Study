
#include "BoundaryValuesMpi.hpp"
#include "CfdSolver/cell_flags.hpp"
#include "MacrosMpi.hpp"

void setLeftRightBordersMpi(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    if (il == 1) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(0,j,k)])) {
                    U[LINU(0,j,k)] = 0.0;
                } else if (isFrictionless(Flag[LINFLAG(0,j,k)])) {
                    U[LINU(0,j,k)] = 0.0;
                } else if (isOutlet(Flag[LINFLAG(0,j,k)])) {
                    U[LINU(0,j,k)] = U[LINU(1,j,k)];
                }

                if (isHeated(Flag[LINFLAG(0,j,k)])) {
                    T[LINT(0,j,k)] = 2 * T_h - T[LINT(1,j,k)];
                } else if (isChilled(Flag[LINFLAG(0,j,k)])) {
                    T[LINT(0,j,k)] = 2 * T_c - T[LINT(1,j,k)];
                } else {
                    T[LINT(0,j,k)] = T[LINT(1,j,k)];
                }
            }
        }
        for (int j = jl-1; j <= ju; j++) {
            for (int k = kl-1; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(0,j,k)])) {
                    V[LINV(0,j,k)] = -V[LINV(1,j,k)];
                    W[LINW(0,j,k)] = -W[LINW(1,j,k)];
                }
                else if (isFrictionless(Flag[LINFLAG(0,j,k)])) {
                    V[LINV(0,j,k)] = V[LINV(1,j,k)];
                    W[LINW(0,j,k)] = W[LINW(1,j,k)];
                }
                else if (isOutlet(Flag[LINFLAG(0,j,k)])) {
                    V[LINV(0,j,k)] = V[LINV(1,j,k)];
                    W[LINW(0,j,k)] = W[LINW(1,j,k)];
                }
            }
        }
    }

    if (iu == imax) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(imax+1,j,k)])) {
                    U[LINU(imax,j,k)] = 0.0;
                } else if (isFrictionless(Flag[LINFLAG(imax+1,j,k)])) {
                    U[LINU(imax,j,k)] = 0.0;
                } else if (isOutlet(Flag[LINFLAG(imax+1,j,k)])) {
                    U[LINU(imax,j,k)] = U[LINU(imax-1,j,k)];
                }

                if (isHeated(Flag[LINFLAG(imax+1,j,k)])) {
                    T[LINT(imax+1,j,k)] = 2 * T_h - T[LINT(imax,j,k)];
                }  else if (isChilled(Flag[LINFLAG(imax+1,j,k)])) {
                    T[LINT(imax+1,j,k)] = 2 * T_c - T[LINT(imax,j,k)];
                } else {
                    T[LINT(imax+1,j,k)] = T[LINT(imax,j,k)];
                }
            }
        }
        for (int j = jl-1; j <= ju; j++) {
            for (int k = kl-1; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(imax+1,j,k)])) {
                    V[LINV(imax+1,j,k)] = -V[LINV(imax,j,k)];
                    W[LINW(imax+1,j,k)] = -W[LINW(imax,j,k)];
                }
                else if (isFrictionless(Flag[LINFLAG(imax+1,j,k)])) {
                    V[LINV(imax+1,j,k)] = V[LINV(imax,j,k)];
                    W[LINW(imax+1,j,k)] = W[LINW(imax,j,k)];
                }
                else if (isOutlet(Flag[LINFLAG(imax+1,j,k)])) {
                    V[LINV(imax+1,j,k)] = V[LINV(imax,j,k)];
                    W[LINW(imax+1,j,k)] = W[LINW(imax,j,k)];
                }
            }
        }
    }
}

void setDownUpBordersMpi(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    if (jl == 1) {
        for (int i = il; i <= iu; i++) {
            for (int k = kl; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(i,0,k)])) {
                    V[LINV(i,0,k)] = 0.0;
                } else if (isFrictionless(Flag[LINFLAG(i,0,k)])) {
                    V[LINV(i,0,k)] = 0.0;
                } else if (isOutlet(Flag[LINFLAG(i,0,k)])) {
                    V[LINV(i,0,k)] = V[LINV(i,1,k)];
                }

                if (isHeated(Flag[LINFLAG(i,0,k)])) {
                    T[LINT(i,0,k)] = 2 * T_h - T[LINT(i,1,k)];
                } else if (isChilled(Flag[LINFLAG(i,0,k)])) {
                    T[LINT(i,0,k)] = 2 * T_c - T[LINT(i,1,k)];
                } else {
                    T[LINT(i,0,k)] = T[LINT(i,1,k)];
                }
            }
        }
        for (int i = il-1; i <= iu; i++) {
            for (int k = kl-1; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(i,0,k)])) {
                    U[LINU(i,0,k)] = -U[LINU(i,1,k)];
                    W[LINW(i,0,k)] = -W[LINW(i,1,k)];
                } else if (isFrictionless(Flag[LINFLAG(i,0,k)])) {
                    U[LINU(i,0,k)] = U[LINU(i,1,k)];
                    W[LINW(i,0,k)] = W[LINW(i,1,k)];
                } else if (isOutlet(Flag[LINFLAG(i,0,k)])) {
                    U[LINU(i,0,k)] = U[LINU(i,1,k)];
                    W[LINW(i,0,k)] = W[LINW(i,1,k)];
                }
            }
        }
    }

    if (ju == jmax) {
        for (int i = il; i <= iu; i++) {
            for (int k = kl; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(i,jmax+1,k)])) {
                    V[LINV(i,jmax,k)] = 0.0;
                } else if (isFrictionless(Flag[LINFLAG(i,jmax+1,k)])) {
                    V[LINV(i,jmax,k)] = 0.0;
                } else if (isOutlet(Flag[LINFLAG(i,jmax+1,k)])) {
                    V[LINV(i,jmax,k)] = V[LINV(i,jmax-1,k)];
                }

                if (isHeated(Flag[LINFLAG(i,jmax+1,k)])) {
                    T[LINT(i,jmax+1,k)] = 2 * T_h - T[LINT(i,jmax,k)];
                } else if (isChilled(Flag[LINFLAG(i,jmax+1,k)])) {
                    T[LINT(i,jmax+1,k)] = 2 * T_c - T[LINT(i,jmax,k)];
                } else {
                    T[LINT(i,jmax+1,k)] = T[LINT(i,jmax,k)];
                }
            }
        }
        for (int i = il-1; i <= iu; i++) {
            for (int k = kl-1; k <= ku; k++) {
                if (isSticky(Flag[LINFLAG(i,jmax+1,k)])) {
                    U[LINU(i,jmax+1,k)] = -U[LINU(i,jmax,k)];
                    W[LINW(i,jmax+1,k)] = -W[LINW(i,jmax,k)];
                } else if (isFrictionless(Flag[LINFLAG(i,jmax+1,k)])) {
                    U[LINU(i,jmax+1,k)] = U[LINU(i,jmax,k)];
                    W[LINW(i,jmax+1,k)] = W[LINW(i,jmax,k)];
                } else if (isOutlet(Flag[LINFLAG(i,jmax+1,k)])) {
                    U[LINU(i,jmax+1,k)] = U[LINU(i,jmax,k)];
                    V[LINV(i,jmax,k)] = V[LINV(i,jmax-1,k)];
                    W[LINW(i,jmax+1,k)] = W[LINW(i,jmax,k)];
                }
            }
        }
    }
}

void setFrontBackBordersMpi(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    if (kl == 1) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                if (isSticky(Flag[LINFLAG(i,j,0)])) {
                    W[LINW(i,j,0)] = 0.0;
                }
                else if (isFrictionless(Flag[LINFLAG(i,j,0)])) {
                    W[LINW(i,j,0)] = 0.0;
                }
                else if (isOutlet(Flag[LINFLAG(i,j,0)])) {
                    W[LINW(i,j,0)] = W[LINW(i,j,1)];
                }

                if (isHeated(Flag[LINFLAG(i,j,0)])) {
                    T[LINT(i,j,0)] = 2 * T_h - T[LINT(i,j,1)];
                }
                else if (isChilled(Flag[LINFLAG(i,j,0)])) {
                    T[LINT(i,j,0)] = 2 * T_c - T[LINT(i,j,1)];
                }
                else {
                    T[LINT(i,j,0)] = T[LINT(i,j,1)];
                }
            }
        }
        for (int i = il-1; i <= iu; i++) {
            for (int j = jl-1; j <= ju; j++) {
                if (isSticky(Flag[LINFLAG(i,j,0)])) {
                    U[LINU(i,j,0)] = -U[LINU(i,j,1)];
                    V[LINV(i,j,0)] = -V[LINV(i,j,1)];
                }
                else if (isFrictionless(Flag[LINFLAG(i,j,0)])) {
                    U[LINU(i,j,0)] = U[LINU(i,j,1)];
                    V[LINV(i,j,0)] = V[LINV(i,j,1)];
                }
                else if (isOutlet(Flag[LINFLAG(i,j,0)])) {
                    U[LINU(i,j,0)] = U[LINU(i,j,1)];
                    V[LINV(i,j,0)] = V[LINV(i,j,1)];
                }
            }
        }
    }

    if (ku == kmax) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                if (isSticky(Flag[LINFLAG(i,j,kmax+1)])) {
                    W[LINW(i,j,kmax)] = 0.0;
                }
                else if (isFrictionless(Flag[LINFLAG(i,j,kmax+1)])) {
                    W[LINW(i,j,kmax)] = 0.0;
                }
                else if (isOutlet(Flag[LINFLAG(i,j,kmax+1)])) {
                    W[LINW(i,j,kmax)] = W[LINW(i,j,kmax-1)];
                }

                if (isHeated(Flag[LINFLAG(i,j,kmax+1)])) {
                    T[LINT(i,j,kmax+1)] = 2 * T_h - T[LINT(i,j,kmax)];
                }
                else if (isChilled(Flag[LINFLAG(i,j,kmax+1)])) {
                    T[LINT(i,j,kmax+1)] = 2 * T_c - T[LINT(i,j,kmax)];
                }
                else {
                    T[LINT(i,j,kmax+1)] = T[LINT(i,j,kmax)];
                }
            }
        }
        for (int i = il-1; i <= iu; i++) {
            for (int j = jl-1; j <= ju; j++) {
                if (isSticky(Flag[LINFLAG(i,j,kmax+1)])) {
                    U[LINU(i,j,kmax+1)] = -U[LINU(i,j,kmax)];
                    V[LINV(i,j,kmax+1)] = -V[LINV(i,j,kmax)];
                }
                else if (isFrictionless(Flag[LINFLAG(i,j,kmax+1)])) {
                    U[LINU(i,j,kmax+1)] = U[LINU(i,j,kmax)];
                    V[LINV(i,j,kmax+1)] = V[LINV(i,j,kmax)];
                }
                else if (isOutlet(Flag[LINFLAG(i,j,kmax+1)])) {
                    U[LINU(i,j,kmax+1)] = U[LINU(i,j,kmax)];
                    V[LINV(i,j,kmax+1)] = V[LINV(i,j,kmax)];
                }
            }
        }
    }
}


void setInternalUBordersMpi(
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U,
        FlagType *Flag) {
    for (int i = il; i <= iu-1; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
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

void setInternalVBordersMpi(
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *V,
        FlagType *Flag) {
    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju-1; j++) {
            for (int k = kl; k <= ku; k++) {
                if (isWet(Flag[LINFLAG(i, j, k)])) {
                    continue;
                }

                int U_count = 0;
                int D_count = 0;
                int U1_count = 0;
                int D1_count = 0;

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

void setInternalWBordersMpi(
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *W,
        FlagType *Flag) {
    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku-1; k++) {
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

void setInternalTBordersMpi(
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *T,
        FlagType *Flag) {
    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                int numDirectFlag = 0;
                Real T_prev = Real(0);

                if (!isWet(Flag[LINFLAG(i, j, k)])) {
                    if (B_R(Flag[LINFLAG(i, j, k)])) {
                        T_prev = T[LINT(i + 1, j, k)];
                        numDirectFlag++;
                    }

                    if (B_L(Flag[LINFLAG(i, j, k)])) {
                        T_prev = T[LINT(i - 1, j, k)];
                        numDirectFlag++;
                    }

                    if (B_U(Flag[LINFLAG(i, j, k)])) {
                        T_prev = T[LINT(i, j + 1, k)];
                        numDirectFlag++;
                    }

                    if (B_D(Flag[LINFLAG(i, j, k)])) {
                        T_prev = T[LINT(i, j - 1, k)];
                        numDirectFlag++;
                    }

                    if (B_B(Flag[LINFLAG(i, j, k)])) {
                        T_prev = T[LINT(i, j, k - 1)];
                        numDirectFlag++;
                    }

                    if (B_F(Flag[LINFLAG(i, j, k)])) {
                        T_prev = T[LINT(i, j, k + 1)];
                        numDirectFlag++;
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
}

void setBorderValuesMpi(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        const MpiSubdomainBounds &bounds,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    const int il = bounds.il, iu = bounds.iu;
    const int jl = bounds.jl, ju = bounds.ju;
    const int kl = bounds.kl, ku = bounds.ku;
    setLeftRightBordersMpi(T_h, T_c, imax, jmax, kmax, il, iu, jl, ju, kl, ku, U, V, W, T, Flag);
    setDownUpBordersMpi(T_h, T_c, imax, jmax, kmax, il, iu, jl, ju, kl, ku, U, V, W, T, Flag);
    setFrontBackBordersMpi(T_h, T_c, imax, jmax, kmax, il, iu, jl, ju, kl, ku, U, V, W, T, Flag);
    setInternalUBordersMpi(imax, jmax, kmax, il, iu, jl, ju, kl, ku, U,Flag);
    setInternalVBordersMpi(imax, jmax, kmax, il, iu, jl, ju, kl, ku, V,Flag);
    setInternalWBordersMpi(imax, jmax, kmax, il, iu, jl, ju, kl, ku, W,Flag);
    setInternalTBordersMpi(imax, jmax, kmax, il, iu, jl, ju, kl, ku, T,Flag);
}


void setBorderValuesCaseSpecificMpi(
        const std::string &scenarioName,
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W,
        FlagType *Flag) {
    if (scenarioName == "driven_cavity") {
        if (ju == jmax) {
            for (int i = il-2; i <= iu+1; i++) {
                for (int k = kl-1; k <= ku+1; k++) {
                    U[LINU(i,jmax+1,k)] = 2.0 - U[LINU(i,jmax,k)];
                }
            }
        }
    } else if (scenarioName == "flow_over_step") {
        if (il == 1) {
            for (int j = jmax / 2 + 1; j <= jmax; j++) {
                for (int k = kl; k <= ku; k++) {
                    U[LINU(0, j, k)] = 1.0;
                    V[LINV(0, j, k)] = 0.0;
                    W[LINW(0, j, k)] = 0.0;
                }
            }
        }
    } else if (scenarioName == "single_tower") {
        if (il == 1) {
            for (int j = jl; j <= ju; j++) {
                for (int k = kl; k <= ku; k++) {
                    U[LINU(0, j, k)] = 1.0;
                    V[LINV(0, j, k)] = 0.0;
                    W[LINW(0, j, k)] = 0.0;
                }
            }
        }
    } else if (scenarioName == "terrain_1" || scenarioName == "fuji_san" || scenarioName == "zugspitze") {
        if (il == 1) {
            for (int j = jl; j <= ju; j++) {
                for (int k = kl; k <= ku; k++) {
                    if (isInlet(Flag[LINFLAG(0, j, k)])) {
                        U[LINU(0, j, k)] = 1.0;
                        V[LINV(0, j, k)] = 0.0;
                        W[LINW(0, j, k)] = 0.0;
                    }
                }
            }
        }
    }
}
