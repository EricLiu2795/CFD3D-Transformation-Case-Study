
#include <cstring>
#include <iostream>
#include "BoundaryValuesMpi.hpp"
#include "UvwMpi.hpp"
#include "SorSolverMpi.hpp"
#include "CfdSolverMpi.hpp"
#include "MpiHelpers.hpp"
#include "MacrosMpi.hpp"

CfdSolverMpi::CfdSolverMpi(
        int il, int iu, int jl, int ju, int kl, int ku,
        int localRank, int peerL, int peerR, int peerD, int peerU, int peerB, int peerF) {
    this->il = il;
    this->iu = iu;
    this->jl = jl;
    this->ju = ju;
    this->kl = kl;
    this->ku = ku;
    this->localRank = localRank;
    this->neighbors.left = peerL;
    this->neighbors.right = peerR;
    this->neighbors.down = peerD;
    this->neighbors.up = peerU;
    this->neighbors.back = peerB;
    this->neighbors.front = peerF;
}

void CfdSolverMpi::initialize(
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

    this->U = new Real[(iu - il + 4)*(ju - jl + 3)*(ku - kl + 3)];
    this->V = new Real[(iu - il + 3)*(ju - jl + 4)*(ku - kl + 3)];
    this->W = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 4)];
    this->P = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];
    this->P_prev = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];
    this->T = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];
    this->T_prev = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];
    this->F = new Real[(iu - il + 4)*(ju - jl + 3)*(ku - kl + 3)];
    this->G = new Real[(iu - il + 3)*(ju - jl + 4)*(ku - kl + 3)];
    this->H = new Real[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 4)];
    this->RS = new Real[(iu - il + 1)*(ju - jl + 1)*(ku - kl + 1)];
    this->Flag = new FlagType[(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3)];

    memset(this->U, 0, sizeof(Real)*(iu - il + 4)*(ju - jl + 3)*(ku - kl + 3));
    memset(this->V, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 4)*(ku - kl + 3));
    memset(this->W, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 4));
    memset(this->P, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
    memset(this->P_prev, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
    memset(this->T, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
    memset(this->T_prev, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
    memset(this->F, 0, sizeof(Real)*(iu - il + 4)*(ju - jl + 3)*(ku - kl + 3));
    memset(this->G, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 4)*(ku - kl + 3));
    memset(this->H, 0, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 4));
    memset(this->RS, 0, sizeof(Real)*(iu - il + 1)*(ju - jl + 1)*(ku - kl + 1));
    memset(this->Flag, 0, sizeof(FlagType)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));

    memcpy(this->U, U, sizeof(Real)*(iu - il + 4)*(ju - jl + 3)*(ku - kl + 3));
    memcpy(this->V, V, sizeof(Real)*(iu - il + 3)*(ju - jl + 4)*(ku - kl + 3));
    memcpy(this->W, W, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 4));
    memcpy(this->P, P, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
    memcpy(this->T, T, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
    memcpy(this->Flag, Flag, sizeof(unsigned int)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));

    int maxHaloSize = std::max(
            std::max((iu - il + 2) * (ju - jl + 2),
                    (iu - il + 2) * (ku - kl + 2)),
            (ju - jl + 2) * (ku - kl + 2));
    haloOut = new Real[maxHaloSize];
    haloIn = new Real[maxHaloSize];
}

CfdSolverMpi::~CfdSolverMpi() {
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

    delete[] haloOut;
    delete[] haloIn;
}

void CfdSolverMpi::setBoundaryValues() {
    setBorderValuesMpi(T_h, T_c, imax, jmax, kmax, {il, iu, jl, ju, kl, ku}, U, V, W, T, Flag);
}

void CfdSolverMpi::setBoundaryValuesScenarioSpecific() {
    setBorderValuesCaseSpecificMpi(scenarioName, imax, jmax, kmax, il, iu, jl, ju, kl, ku, U, V, W, Flag);
}

Real CfdSolverMpi::calculateDt() {
    computeDtMpi(Re, Pr, tau, dt, dx, dy, dz, imax, jmax, kmax, il, iu, jl, ju, kl, ku, U, V, W, useTemperature);
    return dt;
}


void CfdSolverMpi::calculateTemperature() {
    Real *temp = T;
    T = T_prev;
    T_prev = temp;
    computeTemperatureMpi(
            Re, Pr, alpha, dt, dx, dy, dz, imax, jmax, kmax, il, iu, jl, ju, kl, ku,
            neighbors.left, neighbors.right, neighbors.down, neighbors.up, neighbors.back, neighbors.front,
            haloOut, haloIn, U, V, W, T, T_prev, Flag);
}

void CfdSolverMpi::calculateFgh() {
    computeFluxMpi(Re, GX, GY, GZ, alpha, beta, dt, dx, dy, dz, imax, jmax, kmax, il, iu, jl, ju, kl, ku,
            U, V, W, T, F, G, H, Flag);
}

void CfdSolverMpi::calculateRs() {
    computeRsMpi(dx, dy, dz, dt, imax, jmax, kmax, il, iu, jl, ju, kl, ku, F, G, H, RS);
}


void CfdSolverMpi::executeSorSolver() {
    relaxSolverMpi(
            localRank, omg, eps, itermax, linearSystemSolverType, shallWriteOutput,
            dx, dy, dz, imax, jmax, kmax, il, iu, jl, ju, kl, ku,
            neighbors.left, neighbors.right, neighbors.down, neighbors.up, neighbors.back, neighbors.front,
            haloOut, haloIn, P, P_prev, RS, Flag);
}

void CfdSolverMpi::calculateUvw() {
    computeVelocityMpi(
            dt, dx, dy, dz, imax, jmax, kmax, il, iu, jl, ju, kl, ku,
            neighbors.left, neighbors.right, neighbors.down, neighbors.up, neighbors.back, neighbors.front,
            haloOut, haloIn, U, V, W, F, G, H, P, Flag);
}

void CfdSolverMpi::getDataForOutput(Real *U, Real *V, Real *W, Real *P, Real *T) {
    memcpy(U, this->U, sizeof(Real)*(iu - il + 4)*(ju - jl + 3)*(ku - kl + 3));
    memcpy(V, this->V, sizeof(Real)*(iu - il + 3)*(ju - jl + 4)*(ku - kl + 3));
    memcpy(W, this->W, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 4));
    memcpy(P, this->P, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
    memcpy(T, this->T, sizeof(Real)*(iu - il + 3)*(ju - jl + 3)*(ku - kl + 3));
}
