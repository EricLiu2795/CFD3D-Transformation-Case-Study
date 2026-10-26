
#include <iostream>
#include "CfdSolver/cuda/poisson_kernels.hpp"
#include "CfdSolver/cuda/cuda_runtime_check.hpp"

__global__ void assignYZPlanesPressureBorders(
        int imax, int jmax, int kmax, Real *P) {
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;
    if (j <= jmax && k <= kmax) {
        P[LINP(0, j, k)] = P[LINP(1, j, k)];
        P[LINP(imax + 1, j, k)] = P[LINP(imax, j, k)];
    }
}

__global__ void assignXZPlanesPressureBorders(
        int imax, int jmax, int kmax, Real *P) {
    int i = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;
    if (i <= imax && k <= kmax) {
        P[LINP(i, 0, k)] = P[LINP(i, 1, k)];
        P[LINP(i, jmax + 1, k)] = P[LINP(i, jmax, k)];
    }
}

__global__ void assignXYPlanesPressureBorders(
        int imax, int jmax, int kmax, Real *P) {
    int i = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int j = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && j <= jmax) {
        P[LINP(i, j, 0)] = P[LINP(i, j, 1)];
        P[LINP(i, j, kmax + 1)] = P[LINP(i, j, kmax)];
    }
}

__global__ void assignInteriorPressureBordersCuda(
        int imax, int jmax, int kmax, Real *P, FlagType *Flag) {
    int i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && j <= jmax && k <= kmax && !isWet(Flag[LINFLAG(i, j, k)])) {
        int numDirectFlag = 0;
        Real P_prev = Real(0);

        if (B_R(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i + 1, j, k)];
            numDirectFlag++;
        }

        if (B_L(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i - 1, j, k)];
            numDirectFlag++;
        }

        if (B_U(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j + 1, k)];
            numDirectFlag++;
        }

        if (B_D(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j - 1, k)];
            numDirectFlag++;
        }

        if (B_B(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j, k - 1)];
            numDirectFlag++;
        }

        if (B_F(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j, k + 1)];
            numDirectFlag++;
        }

        if (numDirectFlag == 0) {
            P[LINP(i, j, k)] = 0;
        } else {
            P[LINP(i, j, k)] = P_prev / Real(numDirectFlag);
        }
    }
}

template<class T>
__global__ void arraySumCudaKernel(T *input, T *output, int elemCount) {
    extern __shared__ unsigned int sharedPool[];
    T* sdata = (T*)sharedPool;

    unsigned int threadIndex = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x * 2 + threadIdx.x;

    if (i + blockDim.x < elemCount){
        sdata[threadIndex] = input[i] + input[i + blockDim.x];
    } else if (i < elemCount){
        sdata[threadIndex] = input[i];
    } else{
        sdata[threadIndex] = 0;
    }
    __syncthreads();

    for (unsigned int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (threadIndex < stride) {
            sdata[threadIndex] += sdata[threadIndex + stride];
        }
        __syncthreads();
    }

    if (threadIndex == 0) {
        output[blockIdx.x] = sdata[0];
    }
}

template<class T>
T arraySumCuda(T *input, unsigned int valueCount, T *cudaReduceBufferHelper, int blockExtent1D) {
    T sumResult = T(0);

    T *reduceSrc = input;
    T *reduceDst = cudaReduceBufferHelper;

    int blockCount = int(valueCount);
    int inputLen;
    bool finished = false;

    int iteration = 0;
    while (!finished) {
        inputLen = blockCount;
        blockCount = divCeil(blockCount, blockExtent1D*2);

        if (inputLen != 1) {
            int sharedPoolSize = blockExtent1D * sizeof(T);
            arraySumCudaKernel<<<blockCount, blockExtent1D, sharedPoolSize>>>(
                    reduceSrc, reduceDst, inputLen);
            if (iteration % 2 == 0) {
                reduceSrc = cudaReduceBufferHelper;
                reduceDst = input;
            } else {
                reduceSrc = input;
                reduceDst = cudaReduceBufferHelper;
            }
        }

        if (blockCount == 1) {
            finished = true;
        }
        iteration++;
    }

    ensureCudaOk(cudaMemcpy(&sumResult, reduceSrc, sizeof(T), cudaMemcpyDeviceToHost));

    return sumResult;
}

__global__ void relaxSolverResidualArrayCuda(
        Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *P, Real *RS, FlagType *Flag, Real *cellResiduals, unsigned int *fluidCellFlags) {
    int i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && j <= jmax && k <= kmax){
        int flatIndex = (i-1)*jmax*kmax + (j-1)*kmax + k-1;
        if (isWet(Flag[LINFLAG(i,j,k)])){
            cellResiduals[flatIndex] = SQR(
                    (P[LINP(i+1,j,k)] - Real(2.0)*P[LINP(i,j,k)] + P[LINP(i-1,j,k)])/(dx*dx)
                    + (P[LINP(i,j+1,k)] - Real(2.0)*P[LINP(i,j,k)] + P[LINP(i,j-1,k)])/(dy*dy)
                    + (P[LINP(i,j,k+1)] - Real(2.0)*P[LINP(i,j,k)] + P[LINP(i,j,k-1)])/(dz*dz)
                    - RS[LINRS(i,j,k)]
            );
            fluidCellFlags[flatIndex] = 1;
        } else {
            cellResiduals[flatIndex] = Real(0.0);
            fluidCellFlags[flatIndex] = 0;
        }
    }
}


__global__ void relaxSolverSweepCuda(
        Real omg, Real dx, Real dy, Real dz, Real coeff, int imax, int jmax, int kmax,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag) {
    int i = blockIdx.z * blockDim.z + threadIdx.z + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.x * blockDim.x + threadIdx.x + 1;

    if (i <= imax && j <= jmax && k <= kmax) {
        if (isWet(Flag[LINFLAG(i,j,k)])) {
            P[LINP(i, j, k)] = (Real(1.0) - omg) * P_prev[LINP(i, j, k)] + coeff *
                    ((P_prev[LINP(i + 1, j, k)] + P_prev[LINP(i - 1, j, k)]) / (dx * dx)
                     + (P_prev[LINP(i, j + 1, k)] + P_prev[LINP(i, j - 1, k)]) / (dy * dy)
                     + (P_prev[LINP(i, j, k + 1)] + P_prev[LINP(i, j, k - 1)]) / (dz * dz)
                     - RS[LINRS(i, j, k)]);
        }
    }
}

void sorSolverCuda(
        Real omg, Real eps, int itermax, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
        Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ, int blockExtent1D,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag,
        Real *cudaReduceBufferResidual1, Real *cudaReduceBufferResidual2,
        unsigned int *cudaReduceBufferCellCount1, unsigned int *cudaReduceBufferCellCount2) {
    if (linearSystemSolverType == POISSON_RELAX || linearSystemSolverType == POISSON_RELAX_PARALLEL) {
        omg = 1.5;
    } else {
        omg = 1.0;
    }

    const Real coeff = omg / (2.0 * (1.0 / (dx*dx) + 1.0 / (dy*dy) + 1.0 / (dz*dz)));
    Real residual = 1e9;
    int it = 0;
    while (it < itermax && residual > eps) {
        dim3 blockShape2D(tileSizeX, blockExtentY);
        dim3 gridShape_x_y(divCeil(jmax, blockShape2D.x), divCeil(imax, blockShape2D.y));
        assignXYPlanesPressureBorders<<<gridShape_x_y, blockShape2D>>>(imax, jmax, kmax, P);

        dim3 gridShape_x_z(divCeil(kmax, blockShape2D.x), divCeil(imax, blockShape2D.y));
        assignXZPlanesPressureBorders<<<gridShape_x_z, blockShape2D>>>(imax, jmax, kmax, P);

        dim3 gridShape_y_z(divCeil(kmax, blockShape2D.x), divCeil(jmax, blockShape2D.y));
        assignYZPlanesPressureBorders<<<gridShape_y_z, blockShape2D>>>(imax, jmax, kmax, P);

        dim3 blockShape(tileSizeX, blockExtentY, blockExtentZ);
        dim3 gridShape(divCeil(kmax, blockShape.x), divCeil(jmax, blockShape.y), divCeil(imax, blockShape.z));
        assignInteriorPressureBordersCuda<<<gridShape, blockShape>>>(imax, jmax, kmax, P, Flag);

        ensureCudaOk(cudaMemcpy(
                P_prev, P, sizeof(Real) * (imax + 2) * (jmax + 2) * (kmax + 2),
                cudaMemcpyDeviceToDevice));

        relaxSolverSweepCuda<<<gridShape, blockShape>>>(
                omg, dx, dy, dz, coeff, imax, jmax, kmax, P, P_prev, RS, Flag);

        relaxSolverResidualArrayCuda<<<gridShape, blockShape>>>(
                dx, dy, dz, imax, jmax, kmax, P, RS, Flag, cudaReduceBufferResidual1,
                cudaReduceBufferCellCount1);

        residual = arraySumCuda(
                cudaReduceBufferResidual1, imax*jmax*kmax,
                cudaReduceBufferResidual2, blockExtent1D);
        unsigned int fluidCellCount = arraySumCuda(
                cudaReduceBufferCellCount1, imax*jmax*kmax,
                cudaReduceBufferCellCount2, blockExtent1D);
        residual = std::sqrt(residual / Real(fluidCellCount));

        it++;
    }

    if (((residual > eps && it == itermax) || std::isnan(residual)) && shallWriteOutput) {
        std::cerr << "\nSOR solver reached maximum number of iterations without converging (res: "
                  << residual << ")." << std::endl;
    }
    if (std::isnan(residual)) {
        std::cerr << "\nResidual in SOR solver is not a number." << std::endl;
        exit(1);
    }
}
