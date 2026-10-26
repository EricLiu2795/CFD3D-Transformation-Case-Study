
#ifndef FLUID3D_FLUIDSOLVERMPI_HPP
#define FLUID3D_FLUIDSOLVERMPI_HPP

#include "CfdSolver/solver_interface.hpp"

struct NeighborRanks {
    int left, right, down, up, back, front;
};

class CfdSolverMpi : public CfdSolver {
public:
    CfdSolverMpi(
            int il, int iu, int jl, int ju, int kl, int ku,
            int localRank, int peerL, int peerR, int peerD, int peerU, int peerB, int peerF);

    void initialize(
            const std::string &scenarioName, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
            Real Re, Real Pr, Real omg, Real eps, int itermax, Real alpha, Real beta, Real dt, Real tau,
            Real GX, Real GY, Real GZ, bool useTemperature, Real T_h, Real T_c,
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
            Real *U, Real *V, Real *W, Real *P, Real *T, uint32_t *Flag) override;

    ~CfdSolverMpi() override;


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
    int il, iu, jl, ju, kl, ku;
    int localRank;
    NeighborRanks neighbors;
    Real dx, dy, dz;
    Real *U, *V, *W, *P, *P_prev, *T, *T_prev, *F, *G, *H, *RS;
    FlagType *Flag;
    Real *haloOut, *haloIn;
};

#endif
