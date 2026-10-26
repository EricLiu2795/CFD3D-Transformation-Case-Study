
#ifndef FLUID3D_SERIAL_SOLVER_HPP
#define FLUID3D_SERIAL_SOLVER_HPP

#include "CfdSolver/solver_interface.hpp"

struct GridSpacing {
    Real dx, dy, dz;
};

class FluidSolverCpp : public CfdSolver {
public:
    virtual void initialize(
            const std::string &scenarioName, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
            Real Re, Real Pr, Real omg, Real eps, int itermax, Real alpha, Real beta, Real dt, Real tau,
            Real GX, Real GY, Real GZ, bool useTemperature, Real T_h, Real T_c,
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
            Real *U, Real *V, Real *W, Real *P, Real *T, uint32_t *Flag);

    ~FluidSolverCpp();


    virtual void setBoundaryValues();

    virtual void setBoundaryValuesScenarioSpecific();

    virtual Real calculateDt();


    virtual void calculateTemperature();

    virtual void calculateFgh();

    virtual void calculateRs();

    virtual void executeSorSolver();

    virtual void calculateUvw();


    virtual void getDataForOutput(Real *U, Real *V, Real *W, Real *P, Real *T);

private:
    std::string scenarioName;
    LinearSystemSolverType linearSystemSolverType;
    bool shallWriteOutput;
    Real Re, Pr, omg, eps, alpha, beta, dt, tau, GX, GY, GZ, T_h, T_c;
    bool useTemperature;
    int itermax;
    int imax, jmax, kmax;
    GridSpacing spacing;
    Real *U, *V, *W , *P, *P_prev, *T, *T_prev, *F, *G, *H, *RS;
    FlagType *Flag;
};


#endif
