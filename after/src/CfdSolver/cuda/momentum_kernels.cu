
#include <algorithm>
#include <iostream>
#include "CfdSolver/cuda/momentum_kernels.hpp"
#include "CfdSolver/cuda/cuda_runtime_check.hpp"

__global__ void computeFluxCudaKernel(
        Real Re, Real GX, Real GY, Real GZ, Real alpha, Real beta,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T, Real *F, Real *G, Real *H, FlagType *Flag) {
    int i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    Real u_xx,u_yy,u_zz,
            v_xx,v_yy,v_zz,
            w_xx,w_yy,w_zz;
        
    Real uu_x,uv_y,uw_z,
            uv_x,vv_y,vw_z,
            uw_x,vw_y,ww_z;
        
    Real Dx = 1/dx, Dy = 1/dy, Dz = 1/dz;
    
    if (i <= imax - 1 && j <= jmax && k <= kmax) {
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
            u_xx = (U[LINU(i+1,j,k)] - 2*U[LINU(i,j,k)] + U[LINU(i-1,j,k)])/(dx*dx);
            u_yy = (U[LINU(i,j+1,k)] - 2*U[LINU(i,j,k)] + U[LINU(i,j-1,k)])/(dy*dy);
            u_zz = (U[LINU(i,j,k+1)] - 2*U[LINU(i,j,k)] + U[LINU(i,j,k-1)])/(dz*dz);

            uu_x = Real(0.25)*Dx*(
                    (U[LINU(i,j,k)]+U[LINU(i+1,j,k)])*(U[LINU(i,j,k)]+U[LINU(i+1,j,k)]) -
                    (U[LINU(i-1,j,k)]+U[LINU(i,j,k)])*(U[LINU(i-1,j,k)]+U[LINU(i,j,k)]) +
                    alpha*(
                            (std::abs(U[LINU(i,j,k)]+U[LINU(i+1,j,k)])*(U[LINU(i,j,k)]-U[LINU(i+1,j,k)]))-
                            (std::abs(U[LINU(i-1,j,k)]+U[LINU(i,j,k)])*(U[LINU(i-1,j,k)]-U[LINU(i,j,k)]))
                    )
            );

            uv_y = Real(0.25)*Dy*(
                    (V[LINV(i,j,k)]+V[LINV(i+1,j,k)])*(U[LINU(i,j,k)]+U[LINU(i,j+1,k)]) -
                    (V[LINV(i,j-1,k)]+V[LINV(i+1,j-1,k)])*(U[LINU(i,j-1,k)]+U[LINU(i,j,k)]) +
                    alpha*(
                            (std::abs(V[LINV(i,j,k)]+V[LINV(i+1,j,k)])*(U[LINU(i,j,k)]-U[LINU(i,j+1,k)]))-
                            (std::abs(V[LINV(i,j-1,k)]+V[LINV(i+1,j-1,k)])*(U[LINU(i,j-1,k)]-U[LINU(i,j,k)]))
                    )
            );

            uw_z = Real(0.25)*Dz*(
                    (W[LINW(i,j,k)]+W[LINW(i+1,j,k)])*(U[LINU(i,j,k)]+U[LINU(i,j,k+1)]) -
                    (W[LINW(i,j,k-1)]+W[LINW(i+1,j,k-1)])*(U[LINU(i,j,k-1)]+U[LINU(i,j,k)]) +
                    alpha*(
                            (std::abs(W[LINW(i,j,k)]+W[LINW(i+1,j,k)])*(U[LINU(i,j,k)]-U[LINU(i,j,k+1)]))-
                            (std::abs(W[LINW(i,j,k-1)]+W[LINW(i+1,j,k-1)])*(U[LINU(i,j,k-1)]-U[LINU(i,j,k)]))
                    )
            );

            F[LINF(i,j,k)] = U[LINU(i,j,k)] + dt * (
                    (1/Re)*(u_xx+u_yy+u_zz)-
                    uu_x-uv_y-uw_z+
                    GX-(beta/2)*(T[LINT(i,j,k)]+T[LINT(i+1,j,k)])*GX
            );
        } else if (B_L(Flag[LINFLAG(i,j,k)])) {
            F[LINF(i-1,j,k)] = U[LINU(i-1,j,k)];
        } else if (B_R(Flag[LINFLAG(i,j,k)])) {
            F[LINF(i,j,k)] = U[LINU(i,j,k)];
        }
    }

    if (i <= imax && j <= jmax - 1 && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
            v_xx = (V[LINV(i+1,j,k)] - 2*V[LINV(i,j,k)] + V[LINV(i-1,j,k)])/(dx*dx);
            v_yy = (V[LINV(i,j+1,k)] - 2*V[LINV(i,j,k)] + V[LINV(i,j-1,k)])/(dy*dy);
            v_zz = (V[LINV(i,j,k+1)] - 2*V[LINV(i,j,k)] + V[LINV(i,j,k-1)])/(dz*dz);

            uv_x = Real(0.25)*Dx*(
                    (U[LINU(i,j,k)]+U[LINU(i,j+1,k)])*(V[LINV(i,j,k)]+V[LINV(i+1,j,k)]) -
                    (U[LINU(i-1,j,k)]+U[LINU(i-1,j+1,k)])*(V[LINV(i-1,j,k)]+V[LINV(i,j,k)]) +
                    alpha*(
                            (std::abs(U[LINU(i,j,k)]+U[LINU(i,j+1,k)])*(V[LINV(i,j,k)]-V[LINV(i+1,j,k)]))-
                            (std::abs(U[LINU(i-1,j,k)]+U[LINU(i-1,j+1,k)])*(V[LINV(i-1,j,k)]-V[LINV(i,j,k)]))
                    )
            );

            vv_y = Real(0.25)*Dy*(
                    (V[LINV(i,j,k)]+V[LINV(i,j+1,k)])*(V[LINV(i,j,k)]+V[LINV(i,j+1,k)]) -
                    (V[LINV(i,j-1,k)]+V[LINV(i,j,k)])*(V[LINV(i,j-1,k)]+V[LINV(i,j,k)]) +
                    alpha*(
                            (std::abs(V[LINV(i,j,k)]+V[LINV(i,j+1,k)])*(V[LINV(i,j,k)]-V[LINV(i,j+1,k)]))-
                            (std::abs(V[LINV(i,j-1,k)]+V[LINV(i,j,k)])*(V[LINV(i,j-1,k)]-V[LINV(i,j,k)]))
                    )
            );

            vw_z = Real(0.25)*Dz*(
                    (W[LINW(i,j,k)]+W[LINW(i,j+1,k)])*(V[LINV(i,j,k)]+V[LINV(i,j,k+1)]) -
                    (W[LINW(i,j,k-1)]+W[LINW(i,j+1,k-1)])*(V[LINV(i,j,k-1)]+V[LINV(i,j,k)]) +
                    alpha*(
                            (std::abs(W[LINW(i,j,k)]+W[LINW(i,j+1,k)])*(V[LINV(i,j,k)]-V[LINV(i,j,k+1)]))-
                            (std::abs(W[LINW(i,j,k-1)]+W[LINW(i,j+1,k-1)])*(V[LINV(i,j,k-1)]-V[LINV(i,j,k)]))
                    )
            );

            G[LING(i,j,k)] = V[LINV(i,j,k)] + dt * (
                    (1/Re)*(v_xx+v_yy+v_zz)-
                    uv_x-vv_y-vw_z+
                    GY-(beta/2)*(T[LINT(i,j,k)]+T[LINT(i,j+1,k)])*GY
            );
        } else if (B_D(Flag[LINFLAG(i,j,k)])) {
            G[LING(i,j-1,k)] = V[LINV(i,j-1,k)];
        } else if (B_U(Flag[LINFLAG(i,j,k)])) {
            G[LING(i,j,k)] = V[LINV(i,j,k)];
        }        
    }

    if (i <= imax && j <= jmax && k <= kmax - 1) {
        if (isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
            w_xx = (W[LINW(i+1,j,k)] - 2*W[LINW(i,j,k)] + W[LINW(i-1,j,k)])/(dx*dx);
            w_yy = (W[LINW(i,j+1,k)] - 2*W[LINW(i,j,k)] + W[LINW(i,j-1,k)])/(dy*dy);
            w_zz = (W[LINW(i,j,k+1)] - 2*W[LINW(i,j,k)] + W[LINW(i,j,k-1)])/(dz*dz);

            uw_x = Real(0.25)*Dx*(
                    (U[LINU(i,j,k)]+U[LINU(i,j,k+1)])*(W[LINW(i,j,k)]+W[LINW(i+1,j,k)]) -
                    (U[LINU(i-1,j,k)]+U[LINU(i-1,j,k+1)])*(W[LINW(i-1,j,k)]+W[LINW(i,j,k)]) +
                    alpha*(
                            (std::abs(U[LINU(i,j,k)]+U[LINU(i,j,k+1)])*(W[LINW(i,j,k)]-W[LINW(i+1,j,k)]))-
                            (std::abs(U[LINU(i-1,j,k)]+U[LINU(i-1,j,k+1)])*(W[LINW(i-1,j,k)]-W[LINW(i,j,k)]))
                    )
            );

            vw_y = Real(0.25)*Dy*(
                    (V[LINV(i,j,k)]+V[LINV(i,j,k+1)])*(W[LINW(i,j,k)]+W[LINW(i,j+1,k)]) -
                    (V[LINV(i,j-1,k)]+V[LINV(i,j-1,k+1)])*(W[LINW(i,j-1,k)]+W[LINW(i,j,k)]) +
                    alpha*(
                            (std::abs(V[LINV(i,j,k)]+V[LINV(i,j,k+1)])*(W[LINW(i,j,k)]-W[LINW(i,j+1,k)]))-
                            (std::abs(V[LINV(i,j-1,k)]+V[LINV(i,j-1,k+1)])*(W[LINW(i,j-1,k)]-W[LINW(i,j,k)]))
                    )
            );

            ww_z = Real(0.25)*Dz*(
                    (W[LINW(i,j,k)]+W[LINW(i,j,k+1)])*(W[LINW(i,j,k)]+W[LINW(i,j,k+1)]) -
                    (W[LINW(i,j,k-1)]+W[LINW(i,j,k)])*(W[LINW(i,j,k-1)]+W[LINW(i,j,k)]) +
                    alpha*(
                            (std::abs(W[LINW(i,j,k)]+W[LINW(i,j,k+1)])*(W[LINW(i,j,k)]-W[LINW(i,j,k+1)]))-
                            (std::abs(W[LINW(i,j,k-1)]+W[LINW(i,j,k)])*(W[LINW(i,j,k-1)]-W[LINW(i,j,k)]))
                    )
            );

            H[LINH(i,j,k)] = W[LINW(i,j,k)] +  dt * (
                    (1/Re)*(w_xx+w_yy+w_zz)-
                    uw_x-vw_y-ww_z+
                    GZ-(beta/2)*(T[LINT(i,j,k)]+T[LINT(i,j,k+1)])*GZ
            );
        } else if (B_B(Flag[LINFLAG(i,j,k)])) {
            H[LINH(i,j,k-1)] = W[LINW(i,j,k-1)];
        } else if (B_F(Flag[LINFLAG(i,j,k)])) {
            H[LINH(i,j,k)] = W[LINW(i,j,k)];
        }    
    }
}

__global__ void setHBordersCuda(int imax, int jmax, int kmax, Real *W, Real *H) {

    int i = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int j = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && j <= jmax){
        H[LINH(i,j,0)] = W[LINW(i,j,0)];         
        H[LINH(i,j,kmax)] = W[LINW(i,j,kmax)];
    }
}

__global__ void setGBordersCuda(int imax, int jmax, int kmax, Real *V, Real *G) {

    int i = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && k <= kmax){
        G[LING(i,0,k)] = V[LINV(i,0,k)];         
        G[LING(i,jmax,k)] = V[LINV(i,jmax,k)];
    }
}

__global__ void setFBordersCuda(int imax, int jmax, int kmax, Real *U, Real *F) {

    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (j <= jmax && k <= kmax){
        F[LINF(0,j,k)] = U[LINU(0,j,k)];         
        F[LINF(imax,j,k)] = U[LINU(imax,j,k)];
    }
}


void calculateFghCuda(
        Real Re, Real GX, Real GY, Real GZ, Real alpha, Real beta,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ,
        Real *U, Real *V, Real *W, Real *T, Real *F, Real *G, Real *H, FlagType *Flag) {

    dim3 blockShape(tileSizeX, blockExtentY, blockExtentZ);
    dim3 gridShape(divCeil(kmax,blockShape.x),divCeil(jmax,blockShape.y),divCeil(imax,blockShape.z));
    computeFluxCudaKernel<<<gridShape,blockShape>>>(
            Re, GX, GY, GZ, alpha, beta, dt, dx, dy, dz, imax, jmax, kmax, U, V, W, T, F, G, H, Flag);

    dim3 blockShape2D(tileSizeX, blockExtentY);
    dim3 gridShape_y_z(divCeil(kmax,blockShape2D.x),divCeil(jmax,blockShape2D.y));
    setFBordersCuda<<<gridShape_y_z,blockShape2D>>>(imax, jmax, kmax, U, F);

    dim3 gridShape_x_z(divCeil(kmax,blockShape2D.x),divCeil(imax,blockShape2D.y));
    setGBordersCuda<<<gridShape_x_z,blockShape2D>>>(imax, jmax, kmax, V, G);

    dim3 gridShape_x_y(divCeil(jmax,blockShape2D.x),divCeil(imax,blockShape2D.y));
    setHBordersCuda<<<gridShape_x_y,blockShape2D>>>(imax, jmax, kmax, W, H);
        
}

__global__ void calculateRsCuda(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *F, Real *G, Real *H, Real *RS) {
    int i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && j <= jmax && k <= kmax){
        RS[LINRS(i, j, k)] = ((F[LINF(i, j, k)] - F[LINF(i - 1, j, k)]) / dx +
                (G[LING(i, j, k)] - G[LING(i, j - 1, k)]) / dy +
                (H[LINH(i, j, k)] - H[LINH(i, j, k - 1)]) / dz) / dt;
    }
}

__global__ void computePeak(Real *input, Real *output, int elemCount, int blockExtent1D) {
    extern __shared__ Real sdata[];

    unsigned int threadIndex = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x * 2 + threadIdx.x;

    if (i + blockDim.x < elemCount){
#ifdef FP_DOUBLE
    sdata[threadIndex] = fmax(fabs(input[i]), fabs(input[i + blockDim.x]));
#else
    sdata[threadIndex] = fmaxf(fabsf(input[i]), fabsf(input[i + blockDim.x]));
#endif
    } else if (i < elemCount){
        sdata[threadIndex] = input[i];
    } else{
        sdata[threadIndex] = 0;
    }
    __syncthreads();

    for (unsigned int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (threadIndex < stride) {
#ifdef FP_DOUBLE
            sdata[threadIndex] = fmax(fabs(sdata[threadIndex]), fabs(sdata[threadIndex + stride]));
#else
            sdata[threadIndex] = fmaxf(fabsf(sdata[threadIndex]), fabsf(sdata[threadIndex + stride]));
#endif
        }
        __syncthreads();
    }

    if (threadIndex == 0) {
        output[blockIdx.x] = sdata[0];
    }
}

void calculateDtCuda(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax, int blockExtent1D,
        Real *U, Real *V, Real *W,
        Real *cudaReduceBufferU1, Real *cudaFoldArrayU2,
        Real *cudaReduceBufferV1, Real *cudaFoldArrayV2,
        Real *cudaReduceBufferW1, Real *cudaFoldArrayW2,
        bool useTemperature) {
    Real uPeak = Real(0.0), vPeak = Real(0.0), wPeak = Real(0.0);
    
    Real *U_foldInput = U;
    Real *V_foldInput = V;
    Real *W_foldInput = W;
    Real *U_foldOutput = cudaReduceBufferU1;
    Real *V_foldOutput = cudaReduceBufferV1;
    Real *W_foldOutput = cudaReduceBufferW1;

    int blockCountI = (imax+1)*(jmax+2)*(kmax+2);
    int blockCountJ = (imax+2)*(jmax+1)*(kmax+2);
    int blockCountK = (imax+2)*(jmax+2)*(kmax+1);
    int lengthI;
    int lengthJ;
    int lengthK;
    bool finished = false;


    int iteration = 0;
    while (!finished) {
        lengthI = blockCountI;
        lengthJ = blockCountJ;
        lengthK = blockCountK;
        blockCountI = divCeil(blockCountI, blockExtent1D*2);
        blockCountJ = divCeil(blockCountJ, blockExtent1D*2);
        blockCountK = divCeil(blockCountK, blockExtent1D*2);

        if (lengthI != 1) {
            int sharedPoolSize = blockExtent1D * sizeof(Real);
            computePeak<<<blockCountI, blockExtent1D, sharedPoolSize>>>(
                    U_foldInput, U_foldOutput, lengthI, blockExtent1D);
            if (iteration % 2 == 0) {
                U_foldInput = cudaReduceBufferU1;
                U_foldOutput = cudaFoldArrayU2;
            } else {
                U_foldInput = cudaFoldArrayU2;
                U_foldOutput = cudaReduceBufferU1;
            }
        }
        if (lengthJ != 1) {
            int sharedPoolSize = blockExtent1D * sizeof(Real);
            computePeak <<<blockCountJ, blockExtent1D, sharedPoolSize>>> (
                    V_foldInput, V_foldOutput, lengthJ, blockExtent1D);
            if (iteration % 2 == 0) {
                V_foldInput = cudaReduceBufferV1;
                V_foldOutput = cudaFoldArrayV2;
            } else {
                V_foldInput = cudaFoldArrayV2;
                V_foldOutput = cudaReduceBufferV1;
            }
        }
        if (lengthK != 1) {
            int sharedPoolSize = blockExtent1D * sizeof(Real);
            computePeak <<<blockCountK, blockExtent1D, sharedPoolSize>>> (
                    W_foldInput, W_foldOutput, lengthK, blockExtent1D);
            if (iteration % 2 == 0) {
                W_foldInput = cudaReduceBufferW1;
                W_foldOutput = cudaFoldArrayW2;
            } else {
                W_foldInput = cudaFoldArrayW2;
                W_foldOutput = cudaReduceBufferW1;
            }
        }


        if (blockCountI == 1 && blockCountJ == 1 && blockCountK == 1) {
            finished = true;
        }
        iteration++;
    }

    ensureCudaOk(cudaMemcpy(&uPeak, U_foldInput, sizeof(Real), cudaMemcpyDeviceToHost));
    ensureCudaOk(cudaMemcpy(&vPeak, V_foldInput, sizeof(Real), cudaMemcpyDeviceToHost));
    ensureCudaOk(cudaMemcpy(&wPeak, W_foldInput, sizeof(Real), cudaMemcpyDeviceToHost));

    if (tau < Real(0.0)) {
        assert(2 / Re * dt < dx * dx * dy * dy * dz * dz / (dx * dx + dy * dy + dz * dz));
        assert(uPeak * dt < dx);
        assert(vPeak * dt < dy);
        assert(wPeak * dt < dz);
        if (useTemperature){
            assert(dt < (Re*Pr/2)*(1/((1/(dx*dx))+1/(dy*dy)+1/(dz*dz))));
        }
        return;
    }

    dt = std::min(dx / uPeak, dy / vPeak);
    dt = std::min(dt, dz / wPeak);
    dt = std::min(dt, (Re / Real(2.0)) * (Real(1.0) / (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy)
            + Real(1.0) / (dz*dz))));
    if (useTemperature){
        dt = std::min(dt, (Re * Pr / Real(2.0)) * (Real(1.0) / (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy)
                + Real(1.0) / (dz*dz))));
    }
    dt = tau * dt;
}

__global__ void calculateTemperatureCuda(
    Real Re, Real Pr, Real alpha,
    Real dt, Real dx, Real dy, Real dz,
    int imax, int jmax, int kmax,
    Real *U, Real *V, Real *W, Real *T, const Real *T_prev, FlagType *Flag) {

    int i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    Real uT_x, vT_y, wT_z, T_xx, T_yy, T_zz;

    if (i <= imax && j <= jmax && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)])){
            uT_x = 1 / dx * (
                    U[LINU(i, j, k)] * ((T_prev[LINT(i, j, k)] + T_prev[LINT(i + 1, j, k)]) / 2) -
                    U[LINU(i - 1, j, k)] * ((T_prev[LINT(i - 1, j, k)] + T_prev[LINT(i, j, k)]) / 2) +
                    alpha * (
                            std::abs(U[LINU(i, j, k)])*((T_prev[LINT(i, j, k)] - T_prev[LINT(i + 1, j, k)]) / 2) -
                            std::abs(U[LINU(i - 1, j, k)])*((T_prev[LINT(i - 1, j, k)] - T_prev[LINT(i, j, k)]) / 2)
                    )
            );

            vT_y = 1 / dy * (
                    V[LINV(i, j, k)] * ((T_prev[LINT(i, j, k)] + T_prev[LINT(i, j + 1, k)]) / 2) -
                    V[LINV(i, j - 1, k)] * ((T_prev[LINT(i, j - 1, k)] + T_prev[LINT(i, j, k)]) / 2) +
                    alpha * (
                            std::abs(V[LINV(i, j, k)])*((T_prev[LINT(i, j, k)] - T_prev[LINT(i, j + 1, k)]) / 2) -
                            std::abs(V[LINV(i, j - 1, k)])*((T_prev[LINT(i, j - 1, k)] - T_prev[LINT(i, j, k)]) / 2)
                    )
            );

            wT_z = 1 / dz * (
                    W[LINW(i, j, k)] * ((T_prev[LINT(i, j, k)] + T_prev[LINT(i, j, k + 1)]) / 2) -
                    W[LINW(i, j, k - 1)] * ((T_prev[LINT(i, j, k - 1)] + T_prev[LINT(i, j, k)]) / 2) +
                    alpha * (
                            std::abs(W[LINW(i, j, k)])*((T_prev[LINT(i, j, k)] - T_prev[LINT(i, j, k + 1)]) / 2) -
                            std::abs(W[LINW(i, j, k - 1)])*((T_prev[LINT(i, j, k - 1)] - T_prev[LINT(i, j, k)]) / 2)
                    )
            );

            T_xx =
                    (T_prev[LINT(i + 1, j, k)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i - 1, j, k)]) / (dx*dx);

            T_yy =
                    (T_prev[LINT(i, j + 1, k)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i, j - 1, k)]) / (dy*dy);

            T_zz =
                    (T_prev[LINT(i, j, k + 1)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i, j, k - 1)]) / (dz*dz);

            T[LINT(i, j, k)] = T_prev[LINT(i, j, k)] + dt * (
                    (1 / (Re*Pr))*(T_xx + T_yy + T_zz) -
                    uT_x -
                    vT_y -
                    wT_z
            );
        }
    }
}

__global__ void calculateUvwCuda(
    Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
    Real *U, Real *V, Real *W, Real *F, Real *G, Real *H, Real *P, FlagType *Flag) {

    int i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax - 1 && j <= jmax && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
            U[LINU(i, j, k)] = F[LINF(i, j, k)] - dt / dx * (P[LINP(i + 1, j, k)] - P[LINP(i, j, k)]);
        }
    }

    if (i <= imax && j <= jmax - 1 && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
            V[LINV(i, j, k)] = G[LING(i, j, k)] - dt / dy * (P[LINP(i, j + 1, k)] - P[LINP(i, j, k)]);
        }
    }

    if (i <= imax && j <= jmax && k <= kmax - 1){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
            W[LINW(i, j, k)] = H[LINH(i, j, k)] - dt / dz * (P[LINP(i, j, k + 1)] - P[LINP(i, j, k)]);
        }
    }
}
