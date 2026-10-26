
#include <cmath>
#include <algorithm>
#include "Defines.hpp"
#include "CfdSolver/serial/momentum_eqs.hpp"
#include "CfdSolver/cell_flags.hpp"

namespace {

struct Advection {
    static Real donorCellFlux(Real D, Real conv_hi, Real val_hi, Real diff_hi,
            Real conv_lo, Real val_lo, Real diff_lo, Real alpha) {
        return Real(0.25)*D*(
                conv_hi*val_hi - conv_lo*val_lo +
                alpha*(
                        std::abs(conv_hi)*diff_hi - std::abs(conv_lo)*diff_lo
                )
        );
    }

    static Real advectTemperature(Real d, Real alpha, Real vel_c, Real vel_m, Real T_m, Real T_c, Real T_p) {
        return 1 / d * (
                vel_c * ((T_c + T_p) / 2) -
                vel_m * ((T_m + T_c) / 2) +
                alpha * (
                        std::abs(vel_c)*((T_c - T_p) / 2) -
                        std::abs(vel_m)*((T_m - T_c) / 2)
                )
        );
    }
};

}

void computeRsCpp(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *F, Real *G, Real *H, Real *RS) {
    #pragma omp parallel for shared(imax, jmax, kmax, dx, dy, dz, dt, F, G, H, RS) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                RS[LINRS(i, j, k)] = ((F[LINF(i, j, k)] - F[LINF(i - 1, j, k)]) / dx +
                        (G[LING(i, j, k)] - G[LING(i, j - 1, k)]) / dy +
                        (H[LINH(i, j, k)] - H[LINH(i, j, k - 1)]) / dz) / dt;
            }
        }
    }
}

void computeFghCpp(
        const MomentumConstants &constants,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T, Real *F, Real *G, Real *H, FlagType *Flag) {
    const Real Re = constants.Re;
    const Real GX = constants.GX, GY = constants.GY, GZ = constants.GZ;
    const Real alpha = constants.alpha, beta = constants.beta;

    Real u_xx,u_yy,u_zz,
         v_xx,v_yy,v_zz,
         w_xx,w_yy,w_zz;
    
    Real uu_x,uv_y,uw_z,
         uv_x,vv_y,vw_z,
         uw_x,vw_y,ww_z;
    
    Real Dx = 1/dx, Dy = 1/dy, Dz = 1/dz;
    Real dx2 = dx*dx, dy2 = dy*dy, dz2 = dz*dz;

    #pragma omp parallel for private(u_xx, u_yy, u_zz, uu_x, uv_y, uw_z) default(none) \
    shared(imax, jmax, kmax, dx2, dy2, dz2, Dx, Dy, Dz, dt, Re, alpha, beta, GX, U, V, W, T, F, Flag)
    for (int i = 1; i <= imax-1; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
                    u_xx = (U[LINU(i+1,j,k)] - 2*U[LINU(i,j,k)] + U[LINU(i-1,j,k)])/dx2;
                    u_yy = (U[LINU(i,j+1,k)] - 2*U[LINU(i,j,k)] + U[LINU(i,j-1,k)])/dy2;
                    u_zz = (U[LINU(i,j,k+1)] - 2*U[LINU(i,j,k)] + U[LINU(i,j,k-1)])/dz2;

                    uu_x = Advection::donorCellFlux(Dx,
                            U[LINU(i,j,k)]+U[LINU(i+1,j,k)], U[LINU(i,j,k)]+U[LINU(i+1,j,k)], U[LINU(i,j,k)]-U[LINU(i+1,j,k)],
                            U[LINU(i-1,j,k)]+U[LINU(i,j,k)], U[LINU(i-1,j,k)]+U[LINU(i,j,k)], U[LINU(i-1,j,k)]-U[LINU(i,j,k)], alpha);

                    uv_y = Advection::donorCellFlux(Dy,
                            V[LINV(i,j,k)]+V[LINV(i+1,j,k)], U[LINU(i,j,k)]+U[LINU(i,j+1,k)], U[LINU(i,j,k)]-U[LINU(i,j+1,k)],
                            V[LINV(i,j-1,k)]+V[LINV(i+1,j-1,k)], U[LINU(i,j-1,k)]+U[LINU(i,j,k)], U[LINU(i,j-1,k)]-U[LINU(i,j,k)], alpha);

                    uw_z = Advection::donorCellFlux(Dz,
                            W[LINW(i,j,k)]+W[LINW(i+1,j,k)], U[LINU(i,j,k)]+U[LINU(i,j,k+1)], U[LINU(i,j,k)]-U[LINU(i,j,k+1)],
                            W[LINW(i,j,k-1)]+W[LINW(i+1,j,k-1)], U[LINU(i,j,k-1)]+U[LINU(i,j,k)], U[LINU(i,j,k-1)]-U[LINU(i,j,k)], alpha);

                    F[LINF(i,j,k)] = U[LINU(i,j,k)] + dt * (
                            (1/Re)*(u_xx+u_yy+u_zz)-
                            uu_x-uv_y-uw_z+
                            GX-(beta/2)*(T[LINT(i,j,k)]+T[LINT(i+1,j,k)])*GX
                    );
                } else if (B_L(Flag[LINFLAG(i,j,k)])) {
                    F[LINF(i-1,j,k)] = U[LINU(i-1,j,k)];
                } else if (B_R(Flag[LINFLAG(i,j,k)])) {
                    F[LINF(i,j,k)] = U[LINU(i,j,k)];
                }
            }
        }
    }

    #pragma omp parallel for private(v_xx, v_yy, v_zz, uv_x, vv_y, vw_z) default(none) \
    shared(imax, jmax, kmax, dx2, dy2, dz2, Dx, Dy, Dz, dt, Re, alpha, beta, GY, U, V, W, T, G, Flag)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax-1; j++) {
            for (int k = 1; k <= kmax; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
                    v_xx = (V[LINV(i+1,j,k)] - 2*V[LINV(i,j,k)] + V[LINV(i-1,j,k)])/dx2;
                    v_yy = (V[LINV(i,j+1,k)] - 2*V[LINV(i,j,k)] + V[LINV(i,j-1,k)])/dy2;
                    v_zz = (V[LINV(i,j,k+1)] - 2*V[LINV(i,j,k)] + V[LINV(i,j,k-1)])/dz2;

                    uv_x = Advection::donorCellFlux(Dx,
                            U[LINU(i,j,k)]+U[LINU(i,j+1,k)], V[LINV(i,j,k)]+V[LINV(i+1,j,k)], V[LINV(i,j,k)]-V[LINV(i+1,j,k)],
                            U[LINU(i-1,j,k)]+U[LINU(i-1,j+1,k)], V[LINV(i-1,j,k)]+V[LINV(i,j,k)], V[LINV(i-1,j,k)]-V[LINV(i,j,k)], alpha);

                    vv_y = Advection::donorCellFlux(Dy,
                            V[LINV(i,j,k)]+V[LINV(i,j+1,k)], V[LINV(i,j,k)]+V[LINV(i,j+1,k)], V[LINV(i,j,k)]-V[LINV(i,j+1,k)],
                            V[LINV(i,j-1,k)]+V[LINV(i,j,k)], V[LINV(i,j-1,k)]+V[LINV(i,j,k)], V[LINV(i,j-1,k)]-V[LINV(i,j,k)], alpha);

                    vw_z = Advection::donorCellFlux(Dz,
                            W[LINW(i,j,k)]+W[LINW(i,j+1,k)], V[LINV(i,j,k)]+V[LINV(i,j,k+1)], V[LINV(i,j,k)]-V[LINV(i,j,k+1)],
                            W[LINW(i,j,k-1)]+W[LINW(i,j+1,k-1)], V[LINV(i,j,k-1)]+V[LINV(i,j,k)], V[LINV(i,j,k-1)]-V[LINV(i,j,k)], alpha);

                    G[LING(i,j,k)] = V[LINV(i,j,k)] + dt * (
                            (1/Re)*(v_xx+v_yy+v_zz)-
                            uv_x-vv_y-vw_z+
                            GY-(beta/2)*(T[LINT(i,j,k)]+T[LINT(i,j+1,k)])*GY
                    );
                } else if (B_D(Flag[LINFLAG(i,j,k)])) {
                    G[LING(i,j-1,k)] = V[LINV(i,j-1,k)];
                } else if (B_U(Flag[LINFLAG(i,j,k)])) {
                    G[LING(i,j,k)] = V[LINV(i,j,k)];
                }                
            }
        }
    }

    #pragma omp parallel for private(w_xx, w_yy, w_zz, uw_x, vw_y, ww_z) default(none) \
    shared(imax, jmax, kmax, dx2, dy2, dz2, Dx, Dy, Dz, dt, Re, alpha, beta, GZ, U, V, W, T, H, Flag)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax-1; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
                    w_xx = (W[LINW(i+1,j,k)] - 2*W[LINW(i,j,k)] + W[LINW(i-1,j,k)])/dx2;
                    w_yy = (W[LINW(i,j+1,k)] - 2*W[LINW(i,j,k)] + W[LINW(i,j-1,k)])/dy2;
                    w_zz = (W[LINW(i,j,k+1)] - 2*W[LINW(i,j,k)] + W[LINW(i,j,k-1)])/dz2;

                    uw_x = Advection::donorCellFlux(Dx,
                            U[LINU(i,j,k)]+U[LINU(i,j,k+1)], W[LINW(i,j,k)]+W[LINW(i+1,j,k)], W[LINW(i,j,k)]-W[LINW(i+1,j,k)],
                            U[LINU(i-1,j,k)]+U[LINU(i-1,j,k+1)], W[LINW(i-1,j,k)]+W[LINW(i,j,k)], W[LINW(i-1,j,k)]-W[LINW(i,j,k)], alpha);

                    vw_y = Advection::donorCellFlux(Dy,
                            V[LINV(i,j,k)]+V[LINV(i,j,k+1)], W[LINW(i,j,k)]+W[LINW(i,j+1,k)], W[LINW(i,j,k)]-W[LINW(i,j+1,k)],
                            V[LINV(i,j-1,k)]+V[LINV(i,j-1,k+1)], W[LINW(i,j-1,k)]+W[LINW(i,j,k)], W[LINW(i,j-1,k)]-W[LINW(i,j,k)], alpha);

                    ww_z = Advection::donorCellFlux(Dz,
                            W[LINW(i,j,k)]+W[LINW(i,j,k+1)], W[LINW(i,j,k)]+W[LINW(i,j,k+1)], W[LINW(i,j,k)]-W[LINW(i,j,k+1)],
                            W[LINW(i,j,k-1)]+W[LINW(i,j,k)], W[LINW(i,j,k-1)]+W[LINW(i,j,k)], W[LINW(i,j,k-1)]-W[LINW(i,j,k)], alpha);

                    H[LINH(i,j,k)] = W[LINW(i,j,k)] +  dt * (
                            (1/Re)*(w_xx+w_yy+w_zz)-
                            uw_x-vw_y-ww_z+
                            GZ-(beta/2)*(T[LINT(i,j,k)]+T[LINT(i,j,k+1)])*GZ
                    );
                } else if (B_B(Flag[LINFLAG(i,j,k)])) {
                    H[LINH(i,j,k-1)] = W[LINW(i,j,k-1)];
                } else if (B_F(Flag[LINFLAG(i,j,k)])) {
                    H[LINH(i,j,k)] = W[LINW(i,j,k)];
                }    
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, U, F) default(none)
    for (int j = 1; j <= jmax; j++) {
        for (int k = 1; k <= kmax; k++) {
            F[LINF(0,j,k)] = U[LINU(0,j,k)];         
            F[LINF(imax,j,k)] = U[LINU(imax,j,k)];   
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, V, G) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int k = 1; k <= kmax; k++) {
            G[LING(i,0,k)] = V[LINV(i,0,k)];         
            G[LING(i,jmax,k)] = V[LINV(i,jmax,k)];               
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, W, H) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            H[LINH(i,j,0)] = W[LINW(i,j,0)];         
            H[LINH(i,j,kmax)] = W[LINW(i,j,kmax)];               
        }
    }            

}

void computeUvwCpp(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *F, Real *G, Real *H, Real *P, FlagType *Flag) {
    #pragma omp parallel for shared(imax, jmax, kmax, dx, dt, U, P, F, Flag) default(none)
    for (int i = 1; i <= imax - 1; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
                    U[LINU(i, j, k)] = F[LINF(i, j, k)] - dt / dx * (P[LINP(i + 1, j, k)] - P[LINP(i, j, k)]);
                }
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, dy, dt, V, P, G, Flag) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax - 1; j++) {
            for (int k = 1; k <= kmax; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
                    V[LINV(i, j, k)] = G[LING(i, j, k)] - dt / dy * (P[LINP(i, j + 1, k)] - P[LINP(i, j, k)]);
                }
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, dz, dt, W, P, H, Flag) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax - 1; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
                    W[LINW(i, j, k)] = H[LINH(i, j, k)] - dt / dz * (P[LINP(i, j, k + 1)] - P[LINP(i, j, k)]);
                }
            }
        }
    }
}

void computeTemperatureCpp(
        Real Re, Real Pr, Real alpha,
        Real dt, Real dx, Real dy, Real dz,
        int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W, Real *T, Real *T_prev, FlagType *Flag) {
    Real uT_x, vT_y, wT_z, T_xx, T_yy, T_zz;
    Real dx2 = dx*dx, dy2 = dy*dy, dz2 = dz*dz;

    #pragma omp parallel for private(uT_x, vT_y, wT_z, T_xx, T_yy, T_zz) default(none) \
    shared(imax, jmax, kmax, dx, dy, dz, dx2, dy2, dz2, dt, Re, Pr, alpha, U, V, W, T, T_prev, Flag)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)])){
                    uT_x = Advection::advectTemperature(dx, alpha, U[LINU(i, j, k)], U[LINU(i - 1, j, k)],
                            T_prev[LINT(i - 1, j, k)], T_prev[LINT(i, j, k)], T_prev[LINT(i + 1, j, k)]);

                    vT_y = Advection::advectTemperature(dy, alpha, V[LINV(i, j, k)], V[LINV(i, j - 1, k)],
                            T_prev[LINT(i, j - 1, k)], T_prev[LINT(i, j, k)], T_prev[LINT(i, j + 1, k)]);

                    wT_z = Advection::advectTemperature(dz, alpha, W[LINW(i, j, k)], W[LINW(i, j, k - 1)],
                            T_prev[LINT(i, j, k - 1)], T_prev[LINT(i, j, k)], T_prev[LINT(i, j, k + 1)]);

                    T_xx =
                            (T_prev[LINT(i + 1, j, k)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i - 1, j, k)]) / dx2;

                    T_yy =
                            (T_prev[LINT(i, j + 1, k)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i, j - 1, k)]) / dy2;

                    T_zz =
                            (T_prev[LINT(i, j, k + 1)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i, j, k - 1)]) / dz2;

                    T[LINT(i, j, k)] = T_prev[LINT(i, j, k)] + dt * (
                            (1 / (Re*Pr))*(T_xx + T_yy + T_zz) -
                            uT_x -
                            vT_y -
                            wT_z
                    );
                }
            }
        }
    }
}

void computeDtCpp(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        Real *U, Real *V, Real *W,
        TemperatureHandling temperatureHandling) {
    Real uPeak = Real(0.0), vPeak = Real(0.0), wPeak = Real(0.0);

#if _OPENMP >= 201107
    #pragma omp parallel for shared(imax, jmax, kmax, U) reduction(max: uPeak) default(none)
#endif
    for (int i = 0; i <= imax; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                uPeak = std::max(uPeak, std::abs(U[LINU(i,j,k)]));
            }
        }
    }
#if _OPENMP >= 201107
    #pragma omp parallel for shared(imax, jmax, kmax, V) reduction(max: vPeak) default(none)
#endif
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                vPeak = std::max(vPeak, std::abs(V[LINV(i,j,k)]));
            }
        }
    }

#if _OPENMP >= 201107
    #pragma omp parallel for shared(imax, jmax, kmax, W) reduction(max: wPeak) default(none)
#endif
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax; k++) {
                wPeak = std::max(wPeak, std::abs(W[LINW(i,j,k)]));
            }
        }
    }

    if (tau < Real(0.0)) {
        assert(2 / Re * dt < dx * dx * dy * dy * dz * dz / (dx * dx + dy * dy + dz * dz));
        assert(uPeak * dt < dx);
        assert(vPeak * dt < dy);
        assert(wPeak * dt < dz);
        if (temperatureHandling == TEMPERATURE_ENABLED){
            assert(dt < (Re*Pr/2)*(1/((1/(dx*dx))+1/(dy*dy)+1/(dz*dz))));
        }
        return;
    }

    dt = std::min(dx / uPeak, dy / vPeak);
    dt = std::min(dt, dz / wPeak);
    dt = std::min(dt, (Re / Real(2.0)) * (Real(1.0) / (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy)
            + Real(1.0) / (dz*dz))));
    if (temperatureHandling == TEMPERATURE_ENABLED){
        dt = std::min(dt, (Re * Pr / Real(2.0)) * (Real(1.0) / (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy)
                + Real(1.0) / (dz*dz))));
    }
    dt = tau * dt;
}
