/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2019, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
kernel void computeFghOpencl(
        Real Re, Real GX, Real GY, Real GZ, Real alpha, Real beta,
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W, global Real *T, global Real *F, global Real *G, global Real *H,
        global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    Real u_xx,u_yy,u_zz,
            v_xx,v_yy,v_zz,
            w_xx,w_yy,w_zz;

    Real uu_x,uv_y,uw_z,
            uv_x,vv_y,vw_z,
            uw_x,vw_y,ww_z;

    Real Dx = 1/dx, Dy = 1/dy, Dz = 1/dz;

    if (i <= imax - 1 && j <= jmax && k <= kmax) {
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
            u_xx = (U[LINU(i+1,j,k)] - 2*U[LINU(i,j,k)] + U[LINU(i-1,j,k)])/(dx*dx);
            u_yy = (U[LINU(i,j+1,k)] - 2*U[LINU(i,j,k)] + U[LINU(i,j-1,k)])/(dy*dy);
            u_zz = (U[LINU(i,j,k+1)] - 2*U[LINU(i,j,k)] + U[LINU(i,j,k-1)])/(dz*dz);

            uu_x = (Real)(0.25)*Dx*(
                    (U[LINU(i,j,k)]+U[LINU(i+1,j,k)])*(U[LINU(i,j,k)]+U[LINU(i+1,j,k)]) -
                    (U[LINU(i-1,j,k)]+U[LINU(i,j,k)])*(U[LINU(i-1,j,k)]+U[LINU(i,j,k)]) +
                    alpha*(
                            (fabs(U[LINU(i,j,k)]+U[LINU(i+1,j,k)])*(U[LINU(i,j,k)]-U[LINU(i+1,j,k)]))-
                            (fabs(U[LINU(i-1,j,k)]+U[LINU(i,j,k)])*(U[LINU(i-1,j,k)]-U[LINU(i,j,k)]))
                    )
            );

            uv_y = (Real)(0.25)*Dy*(
                    (V[LINV(i,j,k)]+V[LINV(i+1,j,k)])*(U[LINU(i,j,k)]+U[LINU(i,j+1,k)]) -
                    (V[LINV(i,j-1,k)]+V[LINV(i+1,j-1,k)])*(U[LINU(i,j-1,k)]+U[LINU(i,j,k)]) +
                    alpha*(
                            (fabs(V[LINV(i,j,k)]+V[LINV(i+1,j,k)])*(U[LINU(i,j,k)]-U[LINU(i,j+1,k)]))-
                            (fabs(V[LINV(i,j-1,k)]+V[LINV(i+1,j-1,k)])*(U[LINU(i,j-1,k)]-U[LINU(i,j,k)]))
                    )
            );

            uw_z = (Real)(0.25)*Dz*(
                    (W[LINW(i,j,k)]+W[LINW(i+1,j,k)])*(U[LINU(i,j,k)]+U[LINU(i,j,k+1)]) -
                    (W[LINW(i,j,k-1)]+W[LINW(i+1,j,k-1)])*(U[LINU(i,j,k-1)]+U[LINU(i,j,k)]) +
                    alpha*(
                            (fabs(W[LINW(i,j,k)]+W[LINW(i+1,j,k)])*(U[LINU(i,j,k)]-U[LINU(i,j,k+1)]))-
                            (fabs(W[LINW(i,j,k-1)]+W[LINW(i+1,j,k-1)])*(U[LINU(i,j,k-1)]-U[LINU(i,j,k)]))
                    )
            );

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

    if (i <= imax && j <= jmax - 1 && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
            v_xx = (V[LINV(i+1,j,k)] - 2*V[LINV(i,j,k)] + V[LINV(i-1,j,k)])/(dx*dx);
            v_yy = (V[LINV(i,j+1,k)] - 2*V[LINV(i,j,k)] + V[LINV(i,j-1,k)])/(dy*dy);
            v_zz = (V[LINV(i,j,k+1)] - 2*V[LINV(i,j,k)] + V[LINV(i,j,k-1)])/(dz*dz);

            uv_x = (Real)(0.25)*Dx*(
                    (U[LINU(i,j,k)]+U[LINU(i,j+1,k)])*(V[LINV(i,j,k)]+V[LINV(i+1,j,k)]) -
                    (U[LINU(i-1,j,k)]+U[LINU(i-1,j+1,k)])*(V[LINV(i-1,j,k)]+V[LINV(i,j,k)]) +
                    alpha*(
                            (fabs(U[LINU(i,j,k)]+U[LINU(i,j+1,k)])*(V[LINV(i,j,k)]-V[LINV(i+1,j,k)]))-
                            (fabs(U[LINU(i-1,j,k)]+U[LINU(i-1,j+1,k)])*(V[LINV(i-1,j,k)]-V[LINV(i,j,k)]))
                    )
            );

            vv_y = (Real)(0.25)*Dy*(
                    (V[LINV(i,j,k)]+V[LINV(i,j+1,k)])*(V[LINV(i,j,k)]+V[LINV(i,j+1,k)]) -
                    (V[LINV(i,j-1,k)]+V[LINV(i,j,k)])*(V[LINV(i,j-1,k)]+V[LINV(i,j,k)]) +
                    alpha*(
                            (fabs(V[LINV(i,j,k)]+V[LINV(i,j+1,k)])*(V[LINV(i,j,k)]-V[LINV(i,j+1,k)]))-
                            (fabs(V[LINV(i,j-1,k)]+V[LINV(i,j,k)])*(V[LINV(i,j-1,k)]-V[LINV(i,j,k)]))
                    )
            );

            vw_z = (Real)(0.25)*Dz*(
                    (W[LINW(i,j,k)]+W[LINW(i,j+1,k)])*(V[LINV(i,j,k)]+V[LINV(i,j,k+1)]) -
                    (W[LINW(i,j,k-1)]+W[LINW(i,j+1,k-1)])*(V[LINV(i,j,k-1)]+V[LINV(i,j,k)]) +
                    alpha*(
                            (fabs(W[LINW(i,j,k)]+W[LINW(i,j+1,k)])*(V[LINV(i,j,k)]-V[LINV(i,j,k+1)]))-
                            (fabs(W[LINW(i,j,k-1)]+W[LINW(i,j+1,k-1)])*(V[LINV(i,j,k-1)]-V[LINV(i,j,k)]))
                    )
            );

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

    if (i <= imax && j <= jmax && k <= kmax - 1) {
        if (isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
            w_xx = (W[LINW(i+1,j,k)] - 2*W[LINW(i,j,k)] + W[LINW(i-1,j,k)])/(dx*dx);
            w_yy = (W[LINW(i,j+1,k)] - 2*W[LINW(i,j,k)] + W[LINW(i,j-1,k)])/(dy*dy);
            w_zz = (W[LINW(i,j,k+1)] - 2*W[LINW(i,j,k)] + W[LINW(i,j,k-1)])/(dz*dz);

            uw_x = (Real)(0.25)*Dx*(
                    (U[LINU(i,j,k)]+U[LINU(i,j,k+1)])*(W[LINW(i,j,k)]+W[LINW(i+1,j,k)]) -
                    (U[LINU(i-1,j,k)]+U[LINU(i-1,j,k+1)])*(W[LINW(i-1,j,k)]+W[LINW(i,j,k)]) +
                    alpha*(
                            (fabs(U[LINU(i,j,k)]+U[LINU(i,j,k+1)])*(W[LINW(i,j,k)]-W[LINW(i+1,j,k)]))-
                            (fabs(U[LINU(i-1,j,k)]+U[LINU(i-1,j,k+1)])*(W[LINW(i-1,j,k)]-W[LINW(i,j,k)]))
                    )
            );

            vw_y = (Real)(0.25)*Dy*(
                    (V[LINV(i,j,k)]+V[LINV(i,j,k+1)])*(W[LINW(i,j,k)]+W[LINW(i,j+1,k)]) -
                    (V[LINV(i,j-1,k)]+V[LINV(i,j-1,k+1)])*(W[LINW(i,j-1,k)]+W[LINW(i,j,k)]) +
                    alpha*(
                            (fabs(V[LINV(i,j,k)]+V[LINV(i,j,k+1)])*(W[LINW(i,j,k)]-W[LINW(i,j+1,k)]))-
                            (fabs(V[LINV(i,j-1,k)]+V[LINV(i,j-1,k+1)])*(W[LINW(i,j-1,k)]-W[LINW(i,j,k)]))
                    )
            );

            ww_z = (Real)(0.25)*Dz*(
                    (W[LINW(i,j,k)]+W[LINW(i,j,k+1)])*(W[LINW(i,j,k)]+W[LINW(i,j,k+1)]) -
                    (W[LINW(i,j,k-1)]+W[LINW(i,j,k)])*(W[LINW(i,j,k-1)]+W[LINW(i,j,k)]) +
                    alpha*(
                            (fabs(W[LINW(i,j,k)]+W[LINW(i,j,k+1)])*(W[LINW(i,j,k)]-W[LINW(i,j,k+1)]))-
                            (fabs(W[LINW(i,j,k-1)]+W[LINW(i,j,k)])*(W[LINW(i,j,k-1)]-W[LINW(i,j,k)]))
                    )
            );

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

kernel void assignFBordersOpencl(int imax, int jmax, int kmax, global Real *U, global Real *F) {
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    // Set the boundary values for F on the y-z-planes.
    if (j <= jmax && k <= kmax){
        F[LINF(0,j,k)] = U[LINU(0,j,k)];
        F[LINF(imax,j,k)] = U[LINU(imax,j,k)];
    }
}

kernel void assignGBordersOpencl(int imax, int jmax, int kmax, global Real *V, global Real *G) {
    int i = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    // Set the boundary values for G on the x-z-planes.
    if (i <= imax && k <= kmax){
        G[LING(i,0,k)] = V[LINV(i,0,k)];
        G[LING(i,jmax,k)] = V[LINV(i,jmax,k)];
    }
}

kernel void assignHBordersOpencl(int imax, int jmax, int kmax, global Real *W, global Real *H) {

    int i = get_global_id(1) + 1;
    int j = get_global_id(0) + 1;

    // Set the boundary values for G on the x-z-planes.
    if (i <= imax && j <= jmax){
        H[LINH(i,j,0)] = W[LINW(i,j,0)];
        H[LINH(i,j,kmax)] = W[LINW(i,j,kmax)];
    }
}

kernel void computeRsOpencl(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        global Real *F, global Real *G, global Real *H, global Real *RS) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax && j <= jmax && k <= kmax){
        RS[LINRS(i, j, k)] = ((F[LINF(i, j, k)] - F[LINF(i - 1, j, k)]) / dx +
                (G[LING(i, j, k)] - G[LING(i, j - 1, k)]) / dy +
                (H[LINH(i, j, k)] - H[LINH(i, j, k - 1)]) / dz) / dt;
    }
}

/**
 * Reference: Based on kernel 4 from https://developer.download.nvidia.com/assets/cuda/files/reduction.pdf
 * @param input The array of input values (of size 'elemCount').
 * @param output The output array (of size divCeil(blockCountI, blockExtent1D*2)).
 * @param sdata The local memory to use (of byte size blockExtent1D * sizeof(Real)).
 * @param elemCount The number of input values.
 */
kernel void computePeak(
        global Real *input, global Real *output, local Real *sdata, int elemCount) {
    unsigned int threadIndex = get_local_id(0);
    unsigned int i = get_group_id(0) * get_local_size(0) * 2 + get_local_id(0);

    // Copy the data to the shared memory and do the first reduction step.
    if (i + get_local_size(0) < elemCount){
        sdata[threadIndex] = fmax(fabs(input[i]), fabs(input[i + get_local_size(0)]));
    } else if (i < elemCount){
        sdata[threadIndex] = input[i];
    } else{
        sdata[threadIndex] = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    // Do the reduction in the shared memory.
    for (unsigned int stride = get_local_size(0) / 2; stride > 0; stride >>= 1) {
        if (threadIndex < stride) {
            sdata[threadIndex] = fmax(fabs(sdata[threadIndex]), fabs(sdata[threadIndex + stride]));
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Write the result for this block to global memory.
    if (threadIndex == 0) {
        output[get_group_id(0)] = sdata[0];
    }
}

kernel void computeUvwOpencl(
        Real dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W, global Real *F, global Real *G, global Real *H, global Real *P,
        global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax - 1 && j <= jmax && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i+1,j,k)])){
            U[LINU(i, j, k)] = F[LINF(i, j, k)] - dt / dx * (P[LINP(i + 1, j, k)] - P[LINP(i, j, k)]);
        }
    }

    if (i <= imax && j <= jmax - 1 && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j+1,k)])){
            V[LINV(i, j, k)] = G[LING(i, j, k)] - dt / dy * (P[LINP(i, j + 1, k)] - P[LINP(i, j, k)]);
        }
    }

    if (i <= imax && j <= jmax && k <= kmax - 1){
        if(isWet(Flag[LINFLAG(i,j,k)]) && isWet(Flag[LINFLAG(i,j,k+1)])){
            W[LINW(i, j, k)] = H[LINH(i, j, k)] - dt / dz * (P[LINP(i, j, k + 1)] - P[LINP(i, j, k)]);
        }
    }
}

kernel void computeTemperatureOpencl(
        Real Re, Real Pr, Real alpha,
        Real dt, Real dx, Real dy, Real dz,
        int imax, int jmax, int kmax,
        global Real *U, global Real *V, global Real *W, global Real *T, global Real *T_prev, global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    Real uT_x, vT_y, wT_z, T_xx, T_yy, T_zz;

    if (i <= imax && j <= jmax && k <= kmax){
        if(isWet(Flag[LINFLAG(i,j,k)])){
            uT_x = 1 / dx * (
                    U[LINU(i, j, k)] * ((T_prev[LINT(i, j, k)] + T_prev[LINT(i + 1, j, k)]) / 2) -
                    U[LINU(i - 1, j, k)] * ((T_prev[LINT(i - 1, j, k)] + T_prev[LINT(i, j, k)]) / 2) +
                    alpha * (
                            fabs(U[LINU(i, j, k)])*((T_prev[LINT(i, j, k)] - T_prev[LINT(i + 1, j, k)]) / 2) -
                            fabs(U[LINU(i - 1, j, k)])*((T_prev[LINT(i - 1, j, k)] - T_prev[LINT(i, j, k)]) / 2)
                    )
            );

            vT_y = 1 / dy * (
                    V[LINV(i, j, k)] * ((T_prev[LINT(i, j, k)] + T_prev[LINT(i, j + 1, k)]) / 2) -
                    V[LINV(i, j - 1, k)] * ((T_prev[LINT(i, j - 1, k)] + T_prev[LINT(i, j, k)]) / 2) +
                    alpha * (
                            fabs(V[LINV(i, j, k)])*((T_prev[LINT(i, j, k)] - T_prev[LINT(i, j + 1, k)]) / 2) -
                            fabs(V[LINV(i, j - 1, k)])*((T_prev[LINT(i, j - 1, k)] - T_prev[LINT(i, j, k)]) / 2)
                    )
            );

            wT_z = 1 / dz * (
                    W[LINW(i, j, k)] * ((T_prev[LINT(i, j, k)] + T_prev[LINT(i, j, k + 1)]) / 2) -
                    W[LINW(i, j, k - 1)] * ((T_prev[LINT(i, j, k - 1)] + T_prev[LINT(i, j, k)]) / 2) +
                    alpha * (
                            fabs(W[LINW(i, j, k)])*((T_prev[LINT(i, j, k)] - T_prev[LINT(i, j, k + 1)]) / 2) -
                            fabs(W[LINW(i, j, k - 1)])*((T_prev[LINT(i, j, k - 1)] - T_prev[LINT(i, j, k)]) / 2)
                    )
            );

            T_xx =
                    (T_prev[LINT(i + 1, j, k)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i - 1, j, k)]) / (dx*dx);

            T_yy =
                    (T_prev[LINT(i, j + 1, k)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i, j - 1, k)]) / (dy*dy);

            T_zz =
                    (T_prev[LINT(i, j, k + 1)] - 2 * T_prev[LINT(i, j, k)] + T_prev[LINT(i, j, k - 1)]) / (dz*dz);

            T[LINT(i, j, k)] = T_prev[LINT(i, j, k)] + dt * (
                    (1 / (Re*Pr))*(T_xx + T_yy + T_zz) -
                    uT_x -
                    vT_y -
                    wT_z
            );
        }
    }
}
