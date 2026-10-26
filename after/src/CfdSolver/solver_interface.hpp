
#ifndef FLUID3D_SOLVER_INTERFACE_HPP
#define FLUID3D_SOLVER_INTERFACE_HPP

#include <string>
#include <cstdint>
#include "Defines.hpp"

class CfdSolver {
public:
    virtual void initialize(
            const std::string &scenarioName, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
            Real Re, Real Pr, Real omg, Real eps, int itermax, Real alpha, Real beta, Real dt, Real tau,
            Real GX, Real GY, Real GZ, bool useTemperature, Real T_h, Real T_c,
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
            Real *U, Real *V, Real *W, Real *P, Real *T, uint32_t *Flag)=0;

    virtual ~CfdSolver();


    virtual void setBoundaryValues()=0;

    virtual void setBoundaryValuesScenarioSpecific()=0;

    virtual Real calculateDt()=0;


    virtual void calculateTemperature()=0;

    virtual void calculateFgh()=0;

    virtual void calculateRs()=0;

    virtual void executeSorSolver()=0;

    virtual void calculateUvw()=0;


    virtual void getDataForOutput(Real *U, Real *V, Real *W, Real *P, Real *T)=0;
};

#endif
