
#include <cstring>
#include <iostream>
#include "CfdSolver/serial/boundary_conditions.hpp"
#include "CfdSolver/serial/momentum_eqs.hpp"
#include "CfdSolver/serial/poisson_sor.hpp"
#include "CfdSolver/serial/serial_solver.hpp"

void FluidSolverCpp::initialize(
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
    this->spacing.dx = dx;
    this->spacing.dy = dy;
    this->spacing.dz = dz;

    this->U = new Real[(imax+1)*(jmax+2)*(kmax+2)];
    this->V = new Real[(imax+2)*(jmax+1)*(kmax+2)];
    this->W = new Real[(imax+2)*(jmax+2)*(kmax+1)];
    this->P = new Real[(imax+2)*(jmax+2)*(kmax+2)];
    this->P_prev = new Real[(imax+2)*(jmax+2)*(kmax+2)];
    this->T = new Real[(imax+2)*(jmax+2)*(kmax+2)];
    this->T_prev = new Real[(imax+2)*(jmax+2)*(kmax+2)];
    this->F = new Real[(imax+1)*(jmax+1)*(kmax+1)];
    this->G = new Real[(imax+1)*(jmax+1)*(kmax+1)];
    this->H = new Real[(imax+1)*(jmax+1)*(kmax+1)];
    this->RS = new Real[(imax+1)*(jmax+1)*(kmax+1)];
    this->Flag = new FlagType[(imax+2)*(jmax+2)*(kmax+2)];

    memset(this->U, 0, sizeof(Real)*(imax+1)*(jmax+2)*(kmax+2));
    memset(this->V, 0, sizeof(Real)*(imax+2)*(jmax+1)*(kmax+2));
    memset(this->W, 0, sizeof(Real)*(imax+1)*(jmax+2)*(kmax+1));
    memset(this->P, 0, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
    memset(this->P_prev, 0, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
    memset(this->T, 0, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
    memset(this->T_prev, 0, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
    memset(this->F, 0, sizeof(Real)*(imax+1)*(jmax+1)*(kmax+1));
    memset(this->G, 0, sizeof(Real)*(imax+1)*(jmax+1)*(kmax+1));
    memset(this->H, 0, sizeof(Real)*(imax+1)*(jmax+1)*(kmax+1));
    memset(this->RS, 0, sizeof(Real)*(imax+1)*(jmax+1)*(kmax+1));
    memset(this->Flag, 0, sizeof(FlagType)*(imax+2)*(jmax+2)*(kmax+2));

    memcpy(this->U, U, sizeof(Real)*(imax+1)*(jmax+2)*(kmax+2));
    memcpy(this->V, V, sizeof(Real)*(imax+2)*(jmax+1)*(kmax+2));
    memcpy(this->W, W, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+1));
    memcpy(this->P, P, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
    memcpy(this->T, T, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
    memcpy(this->Flag, Flag, sizeof(unsigned int)*(imax+2)*(jmax+2)*(kmax+2));
}

FluidSolverCpp::~FluidSolverCpp() {
    delete[] U;
    delete[] V;
    delete[] W;
    delete[] P;
    delete[] P_prev;
    delete[] T;
    delete[] T_prev;
    delete[] F;
    delete[] G;
    delete[] H;
    delete[] RS;
    delete[] Flag;
}

void FluidSolverCpp::calculateFgh() {
    computeFghCpp({Re, GX, GY, GZ, alpha, beta}, dt, spacing.dx, spacing.dy, spacing.dz, imax, jmax, kmax, U, V, W, T, F, G, H, Flag);
}

void FluidSolverCpp::calculateRs() {
    computeRsCpp(dt, spacing.dx, spacing.dy, spacing.dz, imax, jmax, kmax, F, G, H, RS);
}

Real FluidSolverCpp::calculateDt() {
    computeDtCpp(Re, Pr, tau, dt, spacing.dx, spacing.dy, spacing.dz, imax, jmax, kmax, U, V, W,
            useTemperature ? TEMPERATURE_ENABLED : TEMPERATURE_DISABLED);
    return dt;
}


void FluidSolverCpp::calculateTemperature() {
    Real *temp = T;
    T = T_prev;
    T_prev = temp;
    computeTemperatureCpp(Re, Pr, alpha, dt, spacing.dx, spacing.dy, spacing.dz, imax, jmax, kmax, U, V, W, T, T_prev, Flag);
}

void FluidSolverCpp::calculateUvw() {
    computeUvwCpp(dt, spacing.dx, spacing.dy, spacing.dz, imax, jmax, kmax, U, V, W, F, G, H, P, Flag);
}


void FluidSolverCpp::executeSorSolver() {
    relaxSolverCpp(
            omg, eps, itermax, linearSystemSolverType,
            shallWriteOutput ? SOLVER_LOG_WARNINGS : SOLVER_LOG_SILENT,
            spacing.dx, spacing.dy, spacing.dz, imax, jmax, kmax, P, P_prev, RS, Flag);
}

void FluidSolverCpp::setBoundaryValues() {
    assignBorderValuesCpp(T_h, T_c, imax, jmax, kmax, U, V, W, T, Flag);
}

void FluidSolverCpp::setBoundaryValuesScenarioSpecific() {
    assignScenarioBorderValuesCpp(scenarioName, {imax, jmax, kmax}, U, V, W, Flag);
}

void FluidSolverCpp::getDataForOutput(Real *U, Real *V, Real *W, Real *P, Real *T) {
    memcpy(U, this->U, sizeof(Real)*(imax+1)*(jmax+2)*(kmax+2));
    memcpy(V, this->V, sizeof(Real)*(imax+2)*(jmax+1)*(kmax+2));
    memcpy(W, this->W, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+1));
    memcpy(P, this->P, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
    memcpy(T, this->T, sizeof(Real)*(imax+2)*(jmax+2)*(kmax+2));
}
