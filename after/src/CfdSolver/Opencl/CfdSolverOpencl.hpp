
#ifndef FLUID3D_FLUIDSOLVEROPENCL_HPP
#define FLUID3D_FLUIDSOLVEROPENCL_HPP

#include <cmath>
#include "CfdSolver/solver_interface.hpp"
#include "ClInterface.hpp"

class CfdSolverOpencl : public CfdSolver {
public:
    CfdSolverOpencl(int gpuId, int platformIndex, int tileSizeX, int blockExtentY, int blockExtentZ, int blockExtent1D);

    void initialize(
            const std::string &scenarioName, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
            Real Re, Real Pr, Real omg, Real eps, int itermax, Real alpha, Real beta, Real dt, Real tau,
            Real GX, Real GY, Real GZ, bool useTemperature, Real T_h, Real T_c,
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
            Real *U, Real *V, Real *W, Real *P, Real *T, uint32_t *Flag) override;

    ~CfdSolverOpencl() override;


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
    cl::Buffer U, V, W, P, P_prev, T, T_prev, F, G, H, RS;
    cl::Buffer Flag;
    int gpuId, platformIndex;
    int tileSizeX, blockExtentY, blockExtentZ, blockExtent1D;

    cl::Context context;
    std::vector<cl::Device> devices;
    cl::Program gpuProgramBorderValues, gpuProgramRelax, gpuProgramUvw;
    cl::DispatchQueue queue;
    cl::GridRange teamSize1D, teamSize2D, teamSize3D;

    cl::Kernel assignLeftRightBordersOpenclKernel, assignDownUpBordersOpenclKernel, assignFrontBackBordersOpenclKernel,
            assignInternalUBordersOpenclKernel, assignInternalVBordersOpenclKernel, assignInternalWBordersOpenclKernel,
            assignInternalTBordersOpenclKernel, assignDrivenCavityBordersOpenclKernel,
            assignFlowOverStepBordersOpenclKernel, assignSingleTowerBordersOpenclKernel,
            assignMountainBordersOpenclKernel;

    cl::Kernel assignXYPlanesPressureBordersOpenclKernel, assignXZPlanesPressureBordersOpenclKernel,
            assignYZPlanesPressureBordersOpenclKernel, assignBorderConditionsPressureInDomainOpenclKernel,
            clonePressureOpenclKernel, collapseSumOpenclKernelReal, collapseSumOpenclKernelUint,
            relaxSolverIterationOpenclKernel, relaxSolverComputeResidualArrayOpenclKernel;

    cl::Kernel computeFghOpenclKernel, assignFBordersOpenclKernel, assignGBordersOpenclKernel,
            assignHBordersOpenclKernel, computeRsOpenclKernel, computeMaximumKernel, computeUvwOpenclKernel,
            computeTemperatureOpenclKernel;

    cl::Buffer openclCollapseArrayU1, openclCollapseArrayU2;
    cl::Buffer openclCollapseArrayV1, openclCollapseArrayV2;
    cl::Buffer openclCollapseArrayW1, openclCollapseArrayW2;
    cl::Buffer openclCollapseArrayResidual1, openclCollapseArrayResidual2;
    cl::Buffer openclCollapseArrayNumCells1, openclCollapseArrayNumCells2;
};


#endif
