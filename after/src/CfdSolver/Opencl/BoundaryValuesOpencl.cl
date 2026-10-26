/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2019, Christoph Neuhauser, Stefan Haas, Paul Ng
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

kernel void assignLeftRightBordersOpencl(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W, global Real *T,
        global FlagType *Flag) {
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    // Set the boundary values for the pressure on the y-z-planes.
    if (j <= jmax && k <= kmax){
        // Left wall
        if (isSticky(Flag[LINFLAG(0,j,k)])) {
            U[LINU(0,j,k)] = 0.0;
            V[LINV(0,j,k)] = -V[LINV(1,j,k)];
            W[LINW(0,j,k)] = -W[LINW(1,j,k)];
        }
        else if (isFrictionless(Flag[LINFLAG(0,j,k)])) {
            U[LINU(0,j,k)] = 0.0;
            V[LINV(0,j,k)] = V[LINV(1,j,k)];
            W[LINW(0,j,k)] = W[LINW(1,j,k)];
        }
        else if (isOutlet(Flag[LINFLAG(0,j,k)])) {
            U[LINU(0,j,k)] = U[LINU(1,j,k)];
            V[LINV(0,j,k)] = V[LINV(1,j,k)];
            W[LINW(0,j,k)] = W[LINW(1,j,k)];
        }
        // Right wall
        if (isSticky(Flag[LINFLAG(imax+1,j,k)])) {
            U[LINU(imax,j,k)] = 0.0;
            V[LINV(imax+1,j,k)] = -V[LINV(imax,j,k)];
            W[LINW(imax+1,j,k)] = -W[LINW(imax,j,k)];
        }
        else if (isFrictionless(Flag[LINFLAG(imax+1,j,k)])) {
            U[LINU(imax,j,k)] = 0.0;
            V[LINV(imax+1,j,k)] = V[LINV(imax,j,k)];
            W[LINW(imax+1,j,k)] = W[LINW(imax,j,k)];
        }
        else if (isOutlet(Flag[LINFLAG(imax+1,j,k)])) {
            U[LINU(imax,j,k)] = U[LINU(imax-1,j,k)];
            V[LINV(imax+1,j,k)] = V[LINV(imax,j,k)];
            W[LINW(imax+1,j,k)] = W[LINW(imax,j,k)];
        }

        // Left boundary T
        if (isHeated(Flag[LINFLAG(0,j,k)])) {
            T[LINT(0,j,k)] = 2 * T_h - T[LINT(1,j,k)];
        } else if (isChilled(Flag[LINFLAG(0,j,k)])) {
            T[LINT(0,j,k)] = 2 * T_c - T[LINT(1,j,k)];
        } else {
            T[LINT(0,j,k)] = T[LINT(1,j,k)];
        }

        // Right boundary T
        if (isHeated(Flag[LINFLAG(imax+1,j,k)])) {
            T[LINT(imax+1,j,k)] = 2 * T_h - T[LINT(imax,j,k)];
        }  else if (isChilled(Flag[LINFLAG(imax+1,j,k)])) {
            T[LINT(imax+1,j,k)] = 2 * T_c - T[LINT(imax,j,k)];
        } else {
            T[LINT(imax+1,j,k)] = T[LINT(imax,j,k)];
        }
    }
}

kernel void assignDownUpBordersOpencl(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W, global Real *T,
        global FlagType *Flag) {
    int i = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    // Set the boundary values for the pressure on the x-z-planes.
    if (i <= imax && k <= kmax){
        // Down wall
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
        // Up wall
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

        // Down boundary T
        if (isHeated(Flag[LINFLAG(i,0,k)])) {
            T[LINT(i,0,k)] = 2 * T_h - T[LINT(i,1,k)];
        } else if (isChilled(Flag[LINFLAG(i,0,k)])) {
            T[LINT(i,0,k)] = 2 * T_c - T[LINT(i,1,k)];
        } else {
            T[LINT(i,0,k)] = T[LINT(i,1,k)];
        }

        // Up boundary T
        if (isHeated(Flag[LINFLAG(i,jmax+1,k)])) {
            T[LINT(i,jmax+1,k)] = 2 * T_h - T[LINT(i,jmax,k)];
        } else if (isChilled(Flag[LINFLAG(i,jmax+1,k)])) {
            T[LINT(i,jmax+1,k)] = 2 * T_c - T[LINT(i,jmax,k)];
        } else {
            T[LINT(i,jmax+1,k)] = T[LINT(i,jmax,k)];
        }
    }
}

kernel void assignFrontBackBordersOpencl(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W, global Real *T,
        global FlagType *Flag) {
    int i = get_global_id(1) + 1;
    int j = get_global_id(0) + 1;

    // Set the boundary values for the pressure on the x-y-planes.
    if (i <= imax && j <= jmax){
        // Front wall
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
        // Back wall
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

        // Front boundary T
        if (isHeated(Flag[LINFLAG(i,j,0)])) {
            T[LINT(i,j,0)] = 2 * T_h - T[LINT(i,j,1)];
        }
        else if (isChilled(Flag[LINFLAG(i,j,0)])) {
            T[LINT(i,j,0)] = 2 * T_c - T[LINT(i,j,1)];
        }
        else {
            T[LINT(i,j,0)] = T[LINT(i,j,1)];
        }

        // Back boundary T
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

kernel void assignInternalUBordersOpencl(
        int imax, int jmax, int kmax,
        global Real *U, global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax - 1 && j <= jmax && k <= kmax){
        int R_count = 0;
        int L_count = 0;
        int R1_count = 0;
        int L1_count = 0;

        if (!isWet(Flag[LINFLAG(i, j, k)])) {
            if (B_R(Flag[LINFLAG(i, j, k)])) {
                U[LINU(i, j, k)] = (Real)(0);
                R_count = 1;
            }

            if (B_L(Flag[LINFLAG(i, j, k)])) {
                U[LINU(i - 1, j, k)] = (Real)(0);
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

kernel void assignInternalVBordersOpencl(
        int imax, int jmax, int kmax,
        global Real *V, global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax && j <= jmax - 1 && k <= kmax){
        int U_count = 0;
        int D_count = 0;
        int U1_count = 0;
        int D1_count = 0;

        if (!isWet(Flag[LINFLAG(i, j, k)])) {
            if (B_U(Flag[LINFLAG(i, j, k)])) {
                V[LINV(i, j, k)] = (Real)(0);
                U_count = 1;
            }

            if (B_D(Flag[LINFLAG(i, j, k)])) {
                V[LINV(i, j - 1, k)] = (Real)(0);
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

kernel void assignInternalWBordersOpencl(
        int imax, int jmax, int kmax,
        global Real *W, global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax && j <= jmax && k <= kmax - 1){
        int F_count = 0;
        int B_count = 0;
        int F1_count = 0;
        int B1_count = 0;

        if (!isWet(Flag[LINFLAG(i, j, k)])) {
            if (B_B(Flag[LINFLAG(i, j, k)])) {
                W[LINW(i, j, k - 1)] = (Real)(0);
                B_count = 1;
            }

            if (B_F(Flag[LINFLAG(i, j, k)])) {
                W[LINW(i, j, k)] = (Real)(0);
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

kernel void assignInternalTBordersOpencl(
        int imax, int jmax, int kmax,
        global Real *T, global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax && j <= jmax && k <= kmax){
        int numDirectFlag = 0;
        Real T_prev = (Real)(0);

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
                T[LINT(i,j,k)] = T_prev / (Real)(numDirectFlag);
            }
        }
    }
}



kernel void assignDrivenCavityBordersOpencl(int imax, int jmax, int kmax,
        global Real *U){
    int i = get_global_id(1);
    int k = get_global_id(0) + 1;

    if (i <= imax && k <= kmax) {
        // Upper wall
        U[LINU(i,jmax+1,k)] = 2.0 - U[LINU(i,jmax,k)];
    }
}

kernel void assignFlowOverStepBordersOpencl(int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W){
    int j = get_global_id(1) + jmax/2+1;
    int k = get_global_id(0) + 1;

    if(j <= jmax && k <= kmax) {
        // Left wall
        U[LINU(0,j,k)] = 1.0;
        V[LINV(0,j,k)] = 0.0;
        W[LINW(0,j,k)] = 0.0;
    }
}

kernel void assignSingleTowerBordersOpencl(int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W){
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if(j <= jmax && k <= kmax) {
        // Left wall
        U[LINU(0,j,k)] = 1.0;
        V[LINV(0,j,k)] = 0.0;
        W[LINW(0,j,k)] = 0.0;
    }
}

kernel void assignMountainBordersOpencl(int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W, global FlagType *Flag){
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if(j <= jmax && k <= kmax){
        // Left wall
        if (isInlet(Flag[LINFLAG(0, j, k)])) {
            U[LINU(0, j, k)] = 1.0;
            V[LINV(0, j, k)] = 0.0;
            W[LINW(0, j, k)] = 0.0;
        }
    }
}
