
#ifndef FLUID3D_CUDA_SOLVER_HPP
#define FLUID3D_CUDA_SOLVER_HPP

#include "CfdSolver/solver_interface.hpp"
#include <cmath>

class FluidSolverCuda : public CfdSolver {
public:
    FluidSolverCuda(int gpuId, int tileSizeX, int blockExtentY, int blockExtentZ, int blockExtent1D);

    void initialize(
            const std::string &scenarioName, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
            Real Re, Real Pr, Real omg, Real eps, int itermax, Real alpha, Real beta, Real dt, Real tau,
            Real GX, Real GY, Real GZ, bool useTemperature, Real T_h, Real T_c,
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
            Real *U, Real *V, Real *W, Real *P, Real *T, uint32_t *Flag) override;

    ~FluidSolverCuda() override;


    void setBoundaryValues() override;

    void setBoundaryValuesScenarioSpecific() override;

    Real calculateDt() override;


    void calculateTemperature() override;

    void calculateFgh() override;

    void calculateRs() override;

    void executeSorSolver() override;

    void calculateUvw() override;


    void getDataForOutput(Real *U, Real *V, Real *W, Real *P, Real *T) override;

private:
    std::string scenarioName;
    LinearSystemSolverType linearSystemSolverType;
    bool shallWriteOutput;
    Real Re, Pr, omg, eps, alpha, beta, dt, tau, GX, GY, GZ, T_h, T_c;
    bool useTemperature;
    int itermax;
    int imax, jmax, kmax;
    Real dx, dy, dz;
    Real *U, *V, *W , *P, *P_prev, *T, *T_prev, *F, *G, *H, *RS;
    FlagType *Flag;
    int gpuId;
    int tileSizeX, blockExtentY, blockExtentZ, blockExtent1D;

    Real *cudaReduceBufferU1, *cudaFoldArrayU2;
    Real *cudaReduceBufferV1, *cudaFoldArrayV2;
    Real *cudaReduceBufferW1, *cudaFoldArrayW2;
    Real *cudaReduceBufferResidual1, *cudaReduceBufferResidual2;
    unsigned int *cudaReduceBufferCellCount1, *cudaReduceBufferCellCount2;
};


#endif
