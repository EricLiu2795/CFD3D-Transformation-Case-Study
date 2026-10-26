
#include "CfdSolver/field_init.hpp"

void initFields(
        Real UI, Real VI, Real WI, Real PI, Real TI, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *P, Real *T, FlagType *Flag) {
    #pragma omp parallel for shared(imax, jmax, kmax, U, UI) default(none)
    for (int i = 0; i <= imax; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                U[LINU(i,j,k)] = UI;
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, V, VI) default(none)
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                V[LINV(i,j,k)] = VI;
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, W, WI) default(none)
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax; k++) {
                W[LINW(i,j,k)] = WI;
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, P, T, PI, TI) default(none)
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                P[LINP(i,j,k)] = PI;
                T[LINT(i,j,k)] = TI;
            }
        }
    }
}

#include "CfdSolver/Mpi/MacrosMpi.hpp"

void initFieldsMpi(
        Real UI, Real VI, Real WI, Real PI, Real TI, int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W, Real *P, Real *T, FlagType *Flag) {
    for (int i = il - 2; i <= iu + 1; i++) {
        for (int j = jl - 1; j <= ju + 1; j++) {
            for (int k = kl - 1; k <= ku + 1; k++) {
                U[LINU(i,j,k)] = UI;
            }
        }
    }

    for (int i = il - 1; i <= iu + 1; i++) {
        for (int j = jl - 2; j <= ju + 1; j++) {
            for (int k = kl - 1; k <= ku + 1; k++) {
                V[LINV(i,j,k)] = VI;
            }
        }
    }

    for (int i = il - 1; i <= iu + 1; i++) {
        for (int j = jl - 1; j <= ju + 1; j++) {
            for (int k = kl - 2; k <= ku + 1; k++) {
                W[LINW(i,j,k)] = WI;
            }
        }
    }

    for (int i = il - 1; i <= iu + 1; i++) {
        for (int j = jl - 1; j <= ju + 1; j++) {
            for (int k = kl - 1; k <= ku + 1; k++) {
                P[LINP(i,j,k)] = PI;
                T[LINT(i,j,k)] = TI;
            }
        }
    }
}
