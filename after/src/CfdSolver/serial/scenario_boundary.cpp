#include "CfdSolver/serial/boundary_conditions.hpp"
#include "CfdSolver/cell_flags.hpp"

void assignScenarioBorderValuesCpp(
        const std::string &scenarioName,
        const GridDimensions &grid,
        Real *U, Real *V, Real *W,
        FlagType *Flag) {
    const int imax = grid.imax, jmax = grid.jmax, kmax = grid.kmax;
    if (scenarioName == "driven_cavity") {
        #pragma omp parallel for
        for (int i = 0; i <= imax; i++) {
            for (int k = 1; k <= kmax; k++) {
                U[LINU(i,jmax+1,k)] = Real(2.0) - U[LINU(i,jmax,k)];
            }
        }
    } else if (scenarioName == "flow_over_step") {
        #pragma omp parallel for
        for (int j = jmax/2+1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                U[LINU(0,j,k)] = 1.0;
                V[LINV(0,j,k)] = 0.0;
                W[LINW(0,j,k)] = 0.0;
            }
        }
    } else if (scenarioName == "single_tower") {
        #pragma omp parallel for
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                U[LINU(0,j,k)] = 1.0;
                V[LINV(0,j,k)] = 0.0;
                W[LINW(0,j,k)] = 0.0;
            }
        }
    } else if (scenarioName == "terrain_1" || scenarioName == "fuji_san" || scenarioName == "zugspitze" || scenarioName == "HK_Habor") {
        #pragma omp parallel for
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                if (isInlet(Flag[LINFLAG(0, j, k)])) {
                    U[LINU(0, j, k)] = 1.0;
                    V[LINV(0, j, k)] = 0.0;
                    W[LINW(0, j, k)] = 0.0;
                }
            }
        }
    }
}
