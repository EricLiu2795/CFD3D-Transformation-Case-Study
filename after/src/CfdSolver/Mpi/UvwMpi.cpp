
#include <cmath>
#include <algorithm>
#include "Defines.hpp"
#include "UvwMpi.hpp"
#include "MpiHelpers.hpp"
#include "CfdSolver/cell_flags.hpp"
#include "MacrosMpi.hpp"

void computeFluxMpi(
        Real Re, Real GX, Real GY, Real GZ, Real alpha, Real beta,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W, const Real *T, Real *F, Real *G, Real *H, FlagType *Flag) {
    Real u_xx,u_yy,u_zz,
         v_xx,v_yy,v_zz,
         w_xx,w_yy,w_zz;
    
    Real uu_x,uv_y,uw_z,
         uv_x,vv_y,vw_z,
         uw_x,vw_y,ww_z;
    
    Real Dx = 1/dx, Dy = 1/dy, Dz = 1/dz;
    Real dx2 = dx*dx, dy2 = dy*dy, dz2 = dz*dz;

    auto donorCellFlux = [](Real D, Real conv_hi, Real val_hi, Real conv_lo, Real val_lo,
            Real diff_hi, Real diff_lo, Real alpha) {
        return Real(0.25)*D*(
                conv_hi*val_hi - conv_lo*val_lo +
                alpha*(
                        std::abs(conv_hi)*diff_hi - std::abs(conv_lo)*diff_lo
                )
        );
    };

    for (int i = il-1; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
                    u_xx = (U[LINU(i+1,j,k)] - 2*U[LINU(i,j,k)] + U[LINU(i-1,j,k)])/dx2;
                    u_yy = (U[LINU(i,j+1,k)] - 2*U[LINU(i,j,k)] + U[LINU(i,j-1,k)])/dy2;
                    u_zz = (U[LINU(i,j,k+1)] - 2*U[LINU(i,j,k)] + U[LINU(i,j,k-1)])/dz2;

                    uu_x = donorCellFlux(Dx,
                            U[LINU(i,j,k)]+U[LINU(i+1,j,k)], U[LINU(i,j,k)]+U[LINU(i+1,j,k)],
                            U[LINU(i-1,j,k)]+U[LINU(i,j,k)], U[LINU(i-1,j,k)]+U[LINU(i,j,k)],
                            U[LINU(i,j,k)]-U[LINU(i+1,j,k)], U[LINU(i-1,j,k)]-U[LINU(i,j,k)], alpha);

                    uv_y = donorCellFlux(Dy,
                            V[LINV(i,j,k)]+V[LINV(i+1,j,k)], U[LINU(i,j,k)]+U[LINU(i,j+1,k)],
                            V[LINV(i,j-1,k)]+V[LINV(i+1,j-1,k)], U[LINU(i,j-1,k)]+U[LINU(i,j,k)],
                            U[LINU(i,j,k)]-U[LINU(i,j+1,k)], U[LINU(i,j-1,k)]-U[LINU(i,j,k)], alpha);

                    uw_z = donorCellFlux(Dz,
                            W[LINW(i,j,k)]+W[LINW(i+1,j,k)], U[LINU(i,j,k)]+U[LINU(i,j,k+1)],
                            W[LINW(i,j,k-1)]+W[LINW(i+1,j,k-1)], U[LINU(i,j,k-1)]+U[LINU(i,j,k)],
                            U[LINU(i,j,k)]-U[LINU(i,j,k+1)], U[LINU(i,j,k-1)]-U[LINU(i,j,k)], alpha);

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

    for (int i = il; i <= iu; i++) {
        for (int j = jl-1; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
                    v_xx = (V[LINV(i+1,j,k)] - 2*V[LINV(i,j,k)] + V[LINV(i-1,j,k)])/dx2;
                    v_yy = (V[LINV(i,j+1,k)] - 2*V[LINV(i,j,k)] + V[LINV(i,j-1,k)])/dy2;
                    v_zz = (V[LINV(i,j,k+1)] - 2*V[LINV(i,j,k)] + V[LINV(i,j,k-1)])/dz2;

                    uv_x = donorCellFlux(Dx,
                            U[LINU(i,j,k)]+U[LINU(i,j+1,k)], V[LINV(i,j,k)]+V[LINV(i+1,j,k)],
                            U[LINU(i-1,j,k)]+U[LINU(i-1,j+1,k)], V[LINV(i-1,j,k)]+V[LINV(i,j,k)],
                            V[LINV(i,j,k)]-V[LINV(i+1,j,k)], V[LINV(i-1,j,k)]-V[LINV(i,j,k)], alpha);

                    vv_y = donorCellFlux(Dy,
                            V[LINV(i,j,k)]+V[LINV(i,j+1,k)], V[LINV(i,j,k)]+V[LINV(i,j+1,k)],
                            V[LINV(i,j-1,k)]+V[LINV(i,j,k)], V[LINV(i,j-1,k)]+V[LINV(i,j,k)],
                            V[LINV(i,j,k)]-V[LINV(i,j+1,k)], V[LINV(i,j-1,k)]-V[LINV(i,j,k)], alpha);

                    vw_z = donorCellFlux(Dz,
                            W[LINW(i,j,k)]+W[LINW(i,j+1,k)], V[LINV(i,j,k)]+V[LINV(i,j,k+1)],
                            W[LINW(i,j,k-1)]+W[LINW(i,j+1,k-1)], V[LINV(i,j,k-1)]+V[LINV(i,j,k)],
                            V[LINV(i,j,k)]-V[LINV(i,j,k+1)], V[LINV(i,j-1,k)]-V[LINV(i,j,k)], alpha);

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

    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl-1; k <= ku; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
                    w_xx = (W[LINW(i+1,j,k)] - 2*W[LINW(i,j,k)] + W[LINW(i-1,j,k)])/dx2;
                    w_yy = (W[LINW(i,j+1,k)] - 2*W[LINW(i,j,k)] + W[LINW(i,j-1,k)])/dy2;
                    w_zz = (W[LINW(i,j,k+1)] - 2*W[LINW(i,j,k)] + W[LINW(i,j,k-1)])/dz2;

                    uw_x = donorCellFlux(Dx,
                            U[LINU(i,j,k)]+U[LINU(i,j,k+1)], W[LINW(i,j,k)]+W[LINW(i+1,j,k)],
                            U[LINU(i-1,j,k)]+U[LINU(i-1,j,k+1)], W[LINW(i-1,j,k)]+W[LINW(i,j,k)],
                            W[LINW(i,j,k)]-W[LINW(i+1,j,k)], W[LINW(i-1,j,k)]-W[LINW(i,j,k)], alpha);

                    vw_y = donorCellFlux(Dy,
                            V[LINV(i,j,k)]+V[LINV(i,j,k+1)], W[LINW(i,j,k)]+W[LINW(i,j+1,k)],
                            V[LINV(i,j-1,k)]+V[LINV(i,j-1,k+1)], W[LINW(i,j-1,k)]+W[LINW(i,j,k)],
                            W[LINW(i,j,k)]-W[LINW(i,j+1,k)], W[LINW(i,j-1,k)]-W[LINW(i,j,k)], alpha);

                    ww_z = donorCellFlux(Dz,
                            W[LINW(i,j,k)]+W[LINW(i,j,k+1)], W[LINW(i,j,k)]+W[LINW(i,j,k+1)],
                            W[LINW(i,j,k-1)]+W[LINW(i,j,k)], W[LINW(i,j,k-1)]+W[LINW(i,j,k)],
                            W[LINW(i,j,k)]-W[LINW(i,j,k+1)], W[LINW(i,j,k-1)]-W[LINW(i,j,k)], alpha);

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

    if (il == 1) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                F[LINF(0,j,k)] = U[LINU(0,j,k)];
            }
        }
    }
    if (iu == imax) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                F[LINF(imax,j,k)] = U[LINU(imax,j,k)];
            }
        }
    }

    if (jl == 1) {
        for (int i = il; i <= iu; i++) {
            for (int k = kl; k <= ku; k++) {
                G[LING(i,0,k)] = V[LINV(i,0,k)];
            }
        }
    }
    if (ju == jmax) {
        for (int i = il; i <= iu; i++) {
            for (int k = kl; k <= ku; k++) {
                G[LING(i,jmax,k)] = V[LINV(i,jmax,k)];
            }
        }
    }

    if (kl == 1) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                H[LINH(i,j,0)] = W[LINW(i,j,0)];
            }
        }
    }
    if (ku == kmax) {
        for (int i = il; i <= iu; i++) {
            for (int j = jl; j <= ju; j++) {
                H[LINH(i,j,kmax)] = W[LINW(i,j,kmax)];
            }
        }
    }

}

void computeRsMpi(
        Real dx, Real dy, Real dz, Real dt, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        const Real *F, const Real *G, const Real *H, Real *RS) {
    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                RS[LINRS(i, j, k)] = ((F[LINF(i, j, k)] - F[LINF(i - 1, j, k)]) / dx +
                        (G[LING(i, j, k)] - G[LING(i, j - 1, k)]) / dy +
                        (H[LINH(i, j, k)] - H[LINH(i, j, k - 1)]) / dz) / dt;
            }
        }
    }
}

void computeDtMpi(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        Real *U, Real *V, Real *W,
        bool useTemperature) {
    Real uPeak = Real(0.0), vPeak = Real(0.0), wPeak = Real(0.0);

    for (int i = il-1; i <= iu; i++) {
        for (int j = jl-1; j <= ju+1; j++) {
            for (int k = kl-1; k <= ku+1; k++) {
                uPeak = std::max(uPeak, std::abs(U[LINU(i,j,k)]));
            }
        }
    }
    for (int i = il-1; i <= iu+1; i++) {
        for (int j = jl-1; j <= ju; j++) {
            for (int k = kl-1; k <= ku+1; k++) {
                vPeak = std::max(vPeak, std::abs(V[LINV(i,j,k)]));
            }
        }
    }

    for (int i = il-1; i <= iu+1; i++) {
        for (int j = jl-1; j <= ju+1; j++) {
            for (int k = kl-1; k <= ku; k++) {
                wPeak = std::max(wPeak, std::abs(W[LINW(i,j,k)]));
            }
        }
    }

    DCI_Allreduce(DCI_IN_PLACE, &uPeak, 1, MPI_REAL_FLUID3D, DCI_MAX, DCI_COMM_WORLD);
    DCI_Allreduce(DCI_IN_PLACE, &vPeak, 1, MPI_REAL_FLUID3D, DCI_MAX, DCI_COMM_WORLD);
    DCI_Allreduce(DCI_IN_PLACE, &wPeak, 1, MPI_REAL_FLUID3D, DCI_MAX, DCI_COMM_WORLD);

    if (tau < Real(0.0)) {
        assert(2 / Re * dt < dx * dx * dy * dy * dz * dz / (dx * dx + dy * dy + dz * dz));
        assert(uPeak * dt < dx);
        assert(vPeak * dt < dy);
        assert(wPeak * dt < dz);
        if (useTemperature){
            assert(dt < (Re*Pr/2)*(1/((1/(dx*dx))+1/(dy*dy)+1/(dz*dz))));
        }
        return;
    }

    dt = std::min(dx / uPeak, dy / vPeak);
    dt = std::min(dt, dz / wPeak);
    dt = std::min(dt, (Re / Real(2.0)) * (Real(1.0) / (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy)
            + Real(1.0) / (dz*dz))));
    if (useTemperature){
        dt = std::min(dt, (Re * Pr / Real(2.0)) * (Real(1.0) / (Real(1.0) / (dx*dx) + Real(1.0) / (dy*dy)
                + Real(1.0) / (dz*dz))));
    }
    dt = tau * dt;
}

void computeVelocityMpi(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF, Real *haloOut, Real *haloIn,
        Real *U, Real *V, Real *W, const Real *F, const Real *G, const Real *H, const Real *P, FlagType *Flag) {
    for (int i = il-1; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
                    U[LINU(i, j, k)] = F[LINF(i, j, k)] - dt / dx * (P[LINP(i + 1, j, k)] - P[LINP(i, j, k)]);
                }
            }
        }
    }

    for (int i = il; i <= iu; i++) {
        for (int j = jl-1; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
                    V[LINV(i, j, k)] = G[LING(i, j, k)] - dt / dy * (P[LINP(i, j + 1, k)] - P[LINP(i, j, k)]);
                }
            }
        }
    }

    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl-1; k <= ku; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
                    W[LINW(i, j, k)] = H[LINH(i, j, k)] - dt / dz * (P[LINP(i, j, k + 1)] - P[LINP(i, j, k)]);
                }
            }
        }
    }

    DCI_Status status;
    mpiSwapVelocity(U, V, W, il, iu, jl, ju, kl, ku, peerL, peerR, peerD, peerU, peerB, peerF, haloOut, haloIn, &status);
}

void computeTemperatureMpi(
        Real Re, Real Pr, Real alpha,
        Real dt, Real dx, Real dy, Real dz,
        int imax, int jmax, int kmax,
        int il, int iu, int jl, int ju, int kl, int ku,
        int peerL, int peerR, int peerD, int peerU, int peerB, int peerF, Real *haloOut, Real *haloIn,
        Real *U, Real *V, Real *W, Real *T, const Real *T_prev, FlagType *Flag) {
    Real uT_x, vT_y, wT_z, T_xx, T_yy, T_zz;
    Real dx2 = dx*dx, dy2 = dy*dy, dz2 = dz*dz;

    auto advectTemperature = [](Real d, Real vel_c, Real vel_m, Real T_m, Real T_c, Real T_p, Real alpha) {
        return 1 / d * (
                vel_c * ((T_c + T_p) / 2) -
                vel_m * ((T_m + T_c) / 2) +
                alpha * (
                        std::abs(vel_c)*((T_c - T_p) / 2) -
                        std::abs(vel_m)*((T_m - T_c) / 2)
                )
        );
    };

    for (int i = il; i <= iu; i++) {
        for (int j = jl; j <= ju; j++) {
            for (int k = kl; k <= ku; k++) {
                if(isWet(Flag[LINFLAG(i,j,k)])){
                    uT_x = advectTemperature(dx, U[LINU(i, j, k)], U[LINU(i - 1, j, k)],
                            T_prev[LINT(i - 1, j, k)], T_prev[LINT(i, j, k)], T_prev[LINT(i + 1, j, k)], alpha);

                    vT_y = advectTemperature(dy, V[LINV(i, j, k)], V[LINV(i, j - 1, k)],
                            T_prev[LINT(i, j - 1, k)], T_prev[LINT(i, j, k)], T_prev[LINT(i, j + 1, k)], alpha);

                    wT_z = advectTemperature(dz, W[LINW(i, j, k)], W[LINW(i, j, k - 1)],
                            T_prev[LINT(i, j, k - 1)], T_prev[LINT(i, j, k)], T_prev[LINT(i, j, k + 1)], alpha);

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

    DCI_Status status;
    mpiSwapCellData(T, il, iu, jl, ju, kl, ku, peerL, peerR, peerD, peerU, peerB, peerF, haloOut, haloIn, &status);
}
