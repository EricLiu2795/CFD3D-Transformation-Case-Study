
#include "CfdSolver/cuda/boundary_kernels.hpp"
#include "../../Defines.hpp"
#include "CfdSolver/cuda/cuda_runtime_check.hpp"

static __device__ inline void computeInteriorCellIndex(int &i, int &j, int &k) {
    i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    k = blockIdx.x * blockDim.x + threadIdx.x + 1;
}

__global__ void assignInternalUBordersCuda(
        int imax, int jmax, int kmax,
        Real *U, FlagType *Flag) {
    int i, j, k;
    computeInteriorCellIndex(i, j, k);

    if (i <= imax - 1 && j <= jmax && k <= kmax){
        int R_count = 0;
        int L_count = 0;
        int R1_count = 0;
        int L1_count = 0;

        if (!isWet(Flag[LINFLAG(i, j, k)])) {
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

__global__ void assignInternalVBordersCuda(
    int imax, int jmax, int kmax,
    Real *V,
    FlagType *Flag) {
    int i, j, k;
    computeInteriorCellIndex(i, j, k);

    if (i <= imax && j <= jmax - 1 && k <= kmax){
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

__global__ void assignInternalWBordersCuda(
    int imax, int jmax, int kmax,
    Real *W,
    FlagType *Flag) {
    int i, j, k;
    computeInteriorCellIndex(i, j, k);

    if (i > imax || j > jmax || k > kmax - 1) {
        return;
    }

    int F_count = 0;
    int B_count = 0;
    int F1_count = 0;
    int B1_count = 0;

    if (isWet(Flag[LINFLAG(i, j, k)])) {
        return;
    }

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

__global__ void assignInternalTBordersCuda(
    int imax, int jmax, int kmax,
    Real *T,
    FlagType *Flag) {
    int i, j, k;
    computeInteriorCellIndex(i, j, k);
    
    if (i > imax || j > jmax || k > kmax) {
        return;
    }

    int numDirectFlag = 0;
    Real T_prev = Real(0);

    if (isWet(Flag[LINFLAG(i, j, k)])) {
        return;
    }

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

__global__ void assignLeftRightBordersCuda(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (j <= jmax && k <= kmax){
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

        if (isHeated(Flag[LINFLAG(0,j,k)])) {
            T[LINT(0,j,k)] = 2 * T_h - T[LINT(1,j,k)];
        } else if (isChilled(Flag[LINFLAG(0,j,k)])) {
            T[LINT(0,j,k)] = 2 * T_c - T[LINT(1,j,k)];
        } else {
            T[LINT(0,j,k)] = T[LINT(1,j,k)];
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

__global__ void assignDownUpBordersCuda(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    int i = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && k <= kmax){
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

        if (isHeated(Flag[LINFLAG(i,0,k)])) {
            T[LINT(i,0,k)] = 2 * T_h - T[LINT(i,1,k)];
        } else if (isChilled(Flag[LINFLAG(i,0,k)])) {
            T[LINT(i,0,k)] = 2 * T_c - T[LINT(i,1,k)];
        } else {
            T[LINT(i,0,k)] = T[LINT(i,1,k)];
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

__global__ void assignFrontBackBordersCuda(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    int i = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int j = blockIdx.x * blockDim.x + threadIdx.x + 1;
    
    if (i <= imax && j <= jmax){
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

        if (isHeated(Flag[LINFLAG(i,j,0)])) {
            T[LINT(i,j,0)] = 2 * T_h - T[LINT(i,j,1)];
        } 
        else if (isChilled(Flag[LINFLAG(i,j,0)])) {
            T[LINT(i,j,0)] = 2 * T_c - T[LINT(i,j,1)];
        }
        else {
            T[LINT(i,j,0)] = T[LINT(i,j,1)];
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



void setBoundaryValuesCuda(
        Real T_h, Real T_c,
        int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ,
        Real *U, Real *V, Real *W, Real *T,
        FlagType *Flag) {
    dim3 blockShape2D(tileSizeX, blockExtentY);

    dim3 gridShape_y_z(divCeil(kmax,blockShape2D.x),divCeil(jmax,blockShape2D.y));
    assignLeftRightBordersCuda<<<gridShape_y_z,blockShape2D>>>(T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);

    dim3 gridShape_x_z(divCeil(kmax,blockShape2D.x),divCeil(imax,blockShape2D.y));
    assignDownUpBordersCuda<<<gridShape_x_z,blockShape2D>>>(T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);

    dim3 gridShape_x_y(divCeil(jmax,blockShape2D.x),divCeil(imax,blockShape2D.y));
    assignFrontBackBordersCuda<<<gridShape_x_y,blockShape2D>>>(T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);

    dim3 blockShape3D(tileSizeX, blockExtentY, blockExtentZ);
    dim3 gridShape_internal(divCeil(kmax,blockShape3D.x),divCeil(jmax,blockShape3D.y),divCeil(imax,blockShape3D.z));
    assignInternalUBordersCuda<<<gridShape_internal, blockShape3D>>>(imax, jmax, kmax, U, Flag);
    assignInternalVBordersCuda<<<gridShape_internal, blockShape3D>>>(imax, jmax, kmax, V, Flag);
    assignInternalWBordersCuda<<<gridShape_internal, blockShape3D>>>(imax, jmax, kmax, W, Flag);
    assignInternalTBordersCuda<<<gridShape_internal, blockShape3D>>>(imax, jmax, kmax, T, Flag);
}

__global__ void assignMountainBordersCuda(int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, FlagType *Flag){
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if(j <= jmax && k <= kmax){
        if (isInlet(Flag[LINFLAG(0, j, k)])) {
            U[LINU(0, j, k)] = 1.0;
            V[LINV(0, j, k)] = 0.0;
            W[LINW(0, j, k)] = 0.0;
        }
    }
}

__global__ void assignSingleTowerBordersCuda(int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W){
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if(j <= jmax && k <= kmax) {
        U[LINU(0,j,k)] = 1.0;
        V[LINV(0,j,k)] = 0.0;
        W[LINW(0,j,k)] = 0.0;
    }
}

__global__ void assignFlowOverStepBordersCuda(int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W){
    int j = blockIdx.y * blockDim.y + threadIdx.y + jmax/2+1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if(j <= jmax && k <= kmax) {
        U[LINU(0,j,k)] = 1.0;
        V[LINV(0,j,k)] = 0.0;
        W[LINW(0,j,k)] = 0.0;
    }
}

__global__ void assignDrivenCavityBordersCuda(int imax, int jmax, int kmax,
        Real *U){
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && k <= kmax) {
        U[LINU(i,jmax+1,k)] = 2.0 - U[LINU(i,jmax,k)];
    }
}


void setBoundaryValuesScenarioSpecificCuda(
        const std::string &scenarioName,
        int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ,
        Real *U, Real *V, Real *W,
        FlagType *Flag) {
    dim3 blockShape2D(tileSizeX, blockExtentY);
    if (scenarioName == "driven_cavity") {
        dim3 gridShape_x_z(divCeil(kmax,blockShape2D.x), divCeil(imax + 1,blockShape2D.y));
        assignDrivenCavityBordersCuda<<<blockShape2D, gridShape_x_z>>>(imax, jmax, kmax, U);
    } else if (scenarioName == "flow_over_step") {
        dim3 gridShape_y_z(divCeil(kmax,blockShape2D.x), divCeil(jmax - (jmax / 2 + 1) + 1,blockShape2D.y));
        assignFlowOverStepBordersCuda<<<blockShape2D, gridShape_y_z>>>(imax, jmax, kmax, U, V, W);
    } else if (scenarioName == "single_tower") {
        dim3 gridShape_y_z(divCeil(kmax,blockShape2D.x), divCeil(jmax,blockShape2D.y));
        assignSingleTowerBordersCuda<<<blockShape2D, gridShape_y_z>>>(imax, jmax, kmax, U, V, W);
    } else if (scenarioName == "terrain_1" || scenarioName == "fuji_san" || scenarioName == "zugspitze") {
        dim3 gridShape_y_z(divCeil(kmax,blockShape2D.x), divCeil(jmax,blockShape2D.y));
        assignMountainBordersCuda<<<blockShape2D, gridShape_y_z>>>(imax, jmax, kmax, U, V, W, Flag);
    }
}
