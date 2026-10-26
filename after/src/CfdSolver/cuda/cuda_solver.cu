
#include <cstring>
#include <iostream>
#include "CfdSolver/cuda/boundary_kernels.hpp"
#include "CfdSolver/cuda/momentum_kernels.hpp"
#include "CfdSolver/cuda/poisson_kernels.hpp"
#include "CfdSolver/cuda/cuda_solver.hpp"
#include "CfdSolver/cuda/cuda_runtime_check.hpp"
#include "../../Defines.hpp"

FluidSolverCuda::FluidSolverCuda(int gpuId, int tileSizeX, int blockExtentY, int blockExtentZ, int blockExtent1D) {
    this->gpuId = gpuId;
    this->tileSizeX = tileSizeX;
    this->blockExtentY = blockExtentY;
    this->blockExtentZ = blockExtentZ;
    this->blockExtent1D = blockExtent1D;
}

void FluidSolverCuda::initialize(
        const std::string &scenarioName, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
        Real Re, Real Pr, Real omg, Real eps, int itermax, Real alpha, Real beta, Real dt, Real tau,
        Real GX, Real GY, Real GZ, bool useTemperature, Real T_h, Real T_c,
        int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
        Real *U, Real *V, Real *W, Real *P, Real *T, uint32_t *Flag) {
    this->scenarioName = scenarioName;
    this->linearSystemSolverType = linearSystemSolverType;
    this->shallWriteOutput = shallWriteOutput;
    this->Re = Re;
    this->Pr = Pr;
    this->omg = omg;
    this->eps = eps;
    this->itermax = itermax;
    this->alpha = alpha;
    this->beta = beta;
    this->dt = dt;
    this->tau = tau;
    this->GX = GX;
    this->GY = GY;
    this->GZ = GZ;
    this->useTemperature = useTemperature;
    this->T_h = T_h;
    this->T_c = T_c;
    this->imax = imax;
    this->jmax = jmax;
    this->kmax = kmax;
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;

    int numDevices = 0;
    cudaGetDeviceCount(&numDevices);
    if (numDevices == 0) {
        std::cerr << "Fatal error in FluidSolverCuda::initialize: No CUDA devices were found." << std::endl;
        exit(1);
    }
    if (gpuId >= numDevices) {
        std::cerr << "Error in FluidSolverCuda::initialize: Invalid device ID specified. Setting device ID to 0."
                  << std::endl;
        gpuId = 0;
    }
    cudaSetDevice(gpuId);

    ensureCudaOk(cudaMalloc(&this->U, (imax+1)*(jmax+2)*(kmax+2)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->V, (imax+2)*(jmax+1)*(kmax+2)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->W, (imax+2)*(jmax+2)*(kmax+1)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->P, (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->P_prev, (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->T, (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->T_prev, (imax+2)*(jmax+2)*(kmax+2)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->F, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->G, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->H, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->RS, (imax+1)*(jmax+1)*(kmax+1)*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&this->Flag, (imax+2)*(jmax+2)*(kmax+2)*sizeof(unsigned int)));

    int cudaReduceBufferUSize = divCeil((imax+1)*(jmax+2)*(kmax+2), blockExtent1D*2);
    int cudaReduceBufferVSize = divCeil((imax+1)*(jmax+2)*(kmax+2), blockExtent1D*2);
    int cudaReduceBufferWSize = divCeil((imax+1)*(jmax+2)*(kmax+2), blockExtent1D*2);
    int cudaReduceBufferResidualSize1 = divCeil(imax*jmax*kmax, blockExtent1D*2)*blockExtent1D*2;
    int cudaReduceBufferResidualSize2 = divCeil(imax*jmax*kmax, blockExtent1D*2);
    ensureCudaOk(cudaMalloc(&cudaReduceBufferU1, cudaReduceBufferUSize*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&cudaFoldArrayU2, cudaReduceBufferUSize*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&cudaReduceBufferV1, cudaReduceBufferVSize*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&cudaFoldArrayV2, cudaReduceBufferVSize*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&cudaReduceBufferW1, cudaReduceBufferWSize*sizeof(Real)));
    ensureCudaOk(cudaMalloc(&cudaFoldArrayW2, cudaReduceBufferWSize*sizeof(Real)));
    ensureCudaOk(cudaMalloc(
            &cudaReduceBufferResidual1, cudaReduceBufferResidualSize1*sizeof(Real)));
    ensureCudaOk(cudaMalloc(
            &cudaReduceBufferResidual2, cudaReduceBufferResidualSize2*sizeof(Real)));
    ensureCudaOk(cudaMalloc(
            &cudaReduceBufferCellCount1, cudaReduceBufferResidualSize1*sizeof(unsigned int)));
    ensureCudaOk(cudaMalloc(
            &cudaReduceBufferCellCount2, cudaReduceBufferResidualSize2*sizeof(unsigned int)));

    ensureCudaOk(cudaMemcpy(
            this->U, U, sizeof(Real)*(imax+1)*(jmax+2)*(kmax+2), cudaMemcpyHostToDevice));
    ensureCudaOk(cudaMemcpy(
            this->V, V, sizeof(Real)*(imax+2)*(jmax+1)*(kmax+2), cudaMemcpyHostToDevice));
    ensureCudaOk(cudaMemcpy(
            this->W, W, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+1), cudaMemcpyHostToDevice));
    ensureCudaOk(cudaMemcpy(
            this->P, P, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2), cudaMemcpyHostToDevice));
    ensureCudaOk(cudaMemcpy(
            this->T, T, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2), cudaMemcpyHostToDevice));
    ensureCudaOk(cudaMemcpy(
            this->Flag, Flag, sizeof(unsigned int)*(imax+2)*(jmax+2)*(kmax+2), cudaMemcpyHostToDevice));
}

FluidSolverCuda::~FluidSolverCuda() {
    ensureCudaOk(cudaFree(U));
    ensureCudaOk(cudaFree(V));
    ensureCudaOk(cudaFree(W));
    ensureCudaOk(cudaFree(P));
    ensureCudaOk(cudaFree(P_prev));
    ensureCudaOk(cudaFree(T));
    ensureCudaOk(cudaFree(T_prev));
    ensureCudaOk(cudaFree(F));
    ensureCudaOk(cudaFree(G));
    ensureCudaOk(cudaFree(H));
    ensureCudaOk(cudaFree(RS));
    ensureCudaOk(cudaFree(Flag));

    ensureCudaOk(cudaFree(cudaReduceBufferU1));
    ensureCudaOk(cudaFree(cudaFoldArrayU2));
    ensureCudaOk(cudaFree(cudaReduceBufferV1));
    ensureCudaOk(cudaFree(cudaFoldArrayV2));
    ensureCudaOk(cudaFree(cudaReduceBufferW1));
    ensureCudaOk(cudaFree(cudaFoldArrayW2));
    ensureCudaOk(cudaFree(cudaReduceBufferResidual1));
    ensureCudaOk(cudaFree(cudaReduceBufferResidual2));
    ensureCudaOk(cudaFree(cudaReduceBufferCellCount1));
    ensureCudaOk(cudaFree(cudaReduceBufferCellCount2));
}

void FluidSolverCuda::calculateFgh() {
    calculateFghCuda(
            Re, GX, GY, GZ, alpha, beta, dt, dx, dy, dz, imax, jmax, kmax,
            tileSizeX, blockExtentY, blockExtentZ, U, V, W, T, F, G, H, Flag);
}

void FluidSolverCuda::calculateRs() {
    dim3 blockShape(tileSizeX, blockExtentY, blockExtentZ);
    dim3 gridShape(divCeil(kmax,blockShape.x),divCeil(jmax,blockShape.y),divCeil(imax,blockShape.z));
    calculateRsCuda<<<gridShape,blockShape>>>(dt, dx, dy, dz, imax, jmax, kmax, F, G, H, RS);
}

Real FluidSolverCuda::calculateDt() {
    calculateDtCuda(
            Re, Pr, tau, dt, dx, dy, dz, imax, jmax, kmax, blockExtent1D, U, V, W,
            cudaReduceBufferU1, cudaFoldArrayU2,
            cudaReduceBufferV1, cudaFoldArrayV2,
            cudaReduceBufferW1, cudaFoldArrayW2,
            useTemperature);
    return dt;
}


void FluidSolverCuda::calculateTemperature() {
    Real *temp = T;
    T = T_prev;
    T_prev = temp;
    dim3 blockShape(tileSizeX, blockExtentY, blockExtentZ);
    dim3 gridShape(divCeil(kmax,blockShape.x),divCeil(jmax,blockShape.y),divCeil(imax,blockShape.z));
    calculateTemperatureCuda<<<gridShape,blockShape>>>(
            Re, Pr, alpha, dt, dx, dy, dz, imax, jmax, kmax, U, V, W, T, T_prev, Flag);
}

void FluidSolverCuda::calculateUvw() {
    dim3 blockShape(tileSizeX, blockExtentY, blockExtentZ);
    dim3 gridShape(divCeil(kmax,blockShape.x),divCeil(jmax,blockShape.y),divCeil(imax,blockShape.z));
    calculateUvwCuda<<<gridShape,blockShape>>>(dt, dx, dy, dz, imax, jmax, kmax, U, V, W, F, G, H, P, Flag);
}


void FluidSolverCuda::executeSorSolver() {
    sorSolverCuda(
            omg, eps, itermax, linearSystemSolverType, shallWriteOutput,
            dx, dy, dz, imax, jmax, kmax,
            tileSizeX, blockExtentY, blockExtentZ, blockExtent1D, P, P_prev, RS, Flag,
            cudaReduceBufferResidual1, cudaReduceBufferResidual2,
            cudaReduceBufferCellCount1, cudaReduceBufferCellCount2);
}

void FluidSolverCuda::setBoundaryValues() {
    setBoundaryValuesCuda(
            T_h, T_c, imax, jmax, kmax, tileSizeX, blockExtentY, blockExtentZ, U, V, W, T, Flag);
}

void FluidSolverCuda::setBoundaryValuesScenarioSpecific() {
    setBoundaryValuesScenarioSpecificCuda(
            scenarioName, imax, jmax, kmax, tileSizeX, blockExtentY, blockExtentZ, U, V, W, Flag);
}

void FluidSolverCuda::getDataForOutput(Real *U, Real *V, Real *W, Real *P, Real *T) {
    ensureCudaOk(cudaMemcpy(
            U, this->U, sizeof(Real)*(imax+1)*(jmax+2)*(kmax+2), cudaMemcpyDeviceToHost));
    ensureCudaOk(cudaMemcpy(
            V, this->V, sizeof(Real)*(imax+2)*(jmax+1)*(kmax+2), cudaMemcpyDeviceToHost));
    ensureCudaOk(cudaMemcpy(
            W, this->W, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+1), cudaMemcpyDeviceToHost));
    ensureCudaOk(cudaMemcpy(
            P, this->P, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2), cudaMemcpyDeviceToHost));
    ensureCudaOk(cudaMemcpy(
            T, this->T, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2), cudaMemcpyDeviceToHost));
}
