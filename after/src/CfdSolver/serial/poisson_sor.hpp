
#ifndef FLUID3D_POISSON_SOR_HPP
#define FLUID3D_POISSON_SOR_HPP

#include "Defines.hpp"

enum SolverLogging {
    SOLVER_LOG_SILENT, SOLVER_LOG_WARNINGS
};

void relaxSolverCpp(
        Real omg, Real eps, int itermax, LinearSystemSolverType linearSystemSolverType, SolverLogging solverLogging,
        Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *P, Real *P_prev, Real *RS, FlagType *Flag);

#endif
