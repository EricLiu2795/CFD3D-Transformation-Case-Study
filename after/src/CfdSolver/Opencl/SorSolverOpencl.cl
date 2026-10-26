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
 
kernel void assignXYPlanesPressureBordersOpencl(
        int imax, int jmax, int kmax, global Real *P) {
    int i = get_global_id(1) + 1;
    int j = get_global_id(0) + 1;

    // Set the boundary values for the pressure on the x-y-planes.
    if (i <= imax && j <= jmax) {
        P[LINP(i, j, 0)] = P[LINP(i, j, 1)];
        P[LINP(i, j, kmax + 1)] = P[LINP(i, j, kmax)];
    }
}

kernel void assignXZPlanesPressureBordersOpencl(
        int imax, int jmax, int kmax, global Real *P) {
    int i = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;
    // Set the boundary values for the pressure on the x-z-planes.
    if (i <= imax && k <= kmax) {
        P[LINP(i, 0, k)] = P[LINP(i, 1, k)];
        P[LINP(i, jmax + 1, k)] = P[LINP(i, jmax, k)];
    }
}

kernel void assignYZPlanesPressureBordersOpencl(
        int imax, int jmax, int kmax, global Real *P) {
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;
    // Set the boundary values for the pressure on the y-z-planes.
    if (j <= jmax && k <= kmax) {
        P[LINP(0, j, k)] = P[LINP(1, j, k)];
        P[LINP(imax + 1, j, k)] = P[LINP(imax, j, k)];
    }
}

kernel void assignBorderConditionsPressureInDomainOpencl(
        int imax, int jmax, int kmax, global Real *P, global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax && j <= jmax && k <= kmax && !isWet(Flag[LINFLAG(i, j, k)])) {
        int numDirectFlag = 0;
        Real P_prev = (Real)(0);

        if (B_R(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i + 1, j, k)];
            numDirectFlag++;
        }

        if (B_L(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i - 1, j, k)];
            numDirectFlag++;
        }

        if (B_U(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j + 1, k)];
            numDirectFlag++;
        }

        if (B_D(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j - 1, k)];
            numDirectFlag++;
        }

        if (B_B(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j, k - 1)];
            numDirectFlag++;
        }

        if (B_F(Flag[LINFLAG(i, j, k)])) {
            P_prev += P[LINP(i, j, k + 1)];
            numDirectFlag++;
        }

        if (numDirectFlag == 0) {
            P[LINP(i, j, k)] = 0;
        } else {
            P[LINP(i, j, k)] = P_prev / (Real)(numDirectFlag);
        }
    }
}

kernel void clonePressureOpencl(int imax, int jmax, int kmax, global Real *P, global Real *P_prev) {
    int i = get_global_id(2);
    int j = get_global_id(1);
    int k = get_global_id(0);

    if (i <= imax + 1 && j <= jmax + 1 && k <= kmax + 1) {
        P_prev[LINP(i, j, k)] = P[LINP(i, j, k)];
    }
}

/**
 * Reference: Based on kernel 4 from https://developer.download.nvidia.com/assets/cuda/files/reduction.pdf
 * @param input The array of input values (of size 'elemCount').
 * @param output The output array (of size divCeil(blockCountI, blockExtent1D*2)).
 * @param sdata The local memory to use (of byte size blockExtent1D * sizeof(Real)).
 * @param elemCount The number of input values.
 */
kernel void collapseSumOpenclKernelReal(global Real *input, global Real *output, local Real *sdata, int elemCount) {
    unsigned int threadIndex = get_local_id(0);
    unsigned int i = get_group_id(0) * get_local_size(0) * 2 + get_local_id(0);

    // Copy the data to the shared memory and do the first reduction step.
    if (i + get_local_size(0) < elemCount){
        sdata[threadIndex] = input[i] + input[i + get_local_size(0)];
    } else if (i < elemCount){
        sdata[threadIndex] = input[i];
    } else{
        sdata[threadIndex] = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    // Do the reduction in the shared memory.
    for (unsigned int stride = get_local_size(0) / 2; stride > 0; stride >>= 1) {
        if (threadIndex < stride) {
            sdata[threadIndex] += sdata[threadIndex + stride];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Write the result for this block to global memory.
    if (threadIndex == 0) {
        output[get_group_id(0)] = sdata[0];
    }
}

/**
 * Reference: Based on kernel 4 from https://developer.download.nvidia.com/assets/cuda/files/reduction.pdf
 * @param input The array of input values (of size 'elemCount').
 * @param output The output array (of size divCeil(blockCountI, blockExtent1D*2)).
 * @param sdata The local memory to use (of byte size blockExtent1D * sizeof(unsigned int)).
 * @param elemCount The number of input values.
 */
kernel void collapseSumOpenclKernelUint(
        global unsigned int *input, global unsigned int *output, local unsigned int *sdata, int elemCount) {
    unsigned int threadIndex = get_local_id(0);
    unsigned int i = get_group_id(0) * get_local_size(0) * 2 + get_local_id(0);

    // Copy the data to the shared memory and do the first reduction step.
    if (i + get_local_size(0) < elemCount){
        sdata[threadIndex] = input[i] + input[i + get_local_size(0)];
    } else if (i < elemCount){
        sdata[threadIndex] = input[i];
    } else{
        sdata[threadIndex] = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    // Do the reduction in the shared memory.
    for (unsigned int stride = get_local_size(0) / 2; stride > 0; stride >>= 1) {
        if (threadIndex < stride) {
            sdata[threadIndex] += sdata[threadIndex + stride];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Write the result for this block to global memory.
    if (threadIndex == 0) {
        output[get_group_id(0)] = sdata[0];
    }
}

kernel void relaxSolverIterationOpencl(
        Real omg, Real dx, Real dy, Real dz, Real coeff, int imax, int jmax, int kmax,
        global Real *P, global Real *P_prev, global Real *RS, global FlagType *Flag) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax && j <= jmax && k <= kmax) {
        if (isWet(Flag[LINFLAG(i,j,k)])) {
            P[LINP(i, j, k)] = ((Real)(1.0) - omg) * P_prev[LINP(i, j, k)] + coeff *
                    ((P_prev[LINP(i + 1, j, k)] + P_prev[LINP(i - 1, j, k)]) / (dx * dx)
                     + (P_prev[LINP(i, j + 1, k)] + P_prev[LINP(i, j - 1, k)]) / (dy * dy)
                     + (P_prev[LINP(i, j, k + 1)] + P_prev[LINP(i, j, k - 1)]) / (dz * dz)
                     - RS[LINRS(i, j, k)]);
        }
    }
}

kernel void relaxSolverComputeResidualArrayOpencl(
        Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        global Real *P, global Real *RS, global FlagType *Flag, global Real *cellResiduals,
        global unsigned int *fluidCellFlags) {
    int i = get_global_id(2) + 1;
    int j = get_global_id(1) + 1;
    int k = get_global_id(0) + 1;

    if (i <= imax && j <= jmax && k <= kmax){
        int flatIndex = (i-1)*jmax*kmax + (j-1)*kmax + k-1;
        if (isWet(Flag[LINFLAG(i,j,k)])){
            cellResiduals[flatIndex] = SQR(
                    (P[LINP(i+1,j,k)] - (Real)(2.0)*P[LINP(i,j,k)] + P[LINP(i-1,j,k)])/(dx*dx)
                    + (P[LINP(i,j+1,k)] - (Real)(2.0)*P[LINP(i,j,k)] + P[LINP(i,j-1,k)])/(dy*dy)
                    + (P[LINP(i,j,k+1)] - (Real)(2.0)*P[LINP(i,j,k)] + P[LINP(i,j,k-1)])/(dz*dz)
                    - RS[LINRS(i,j,k)]
            );
            fluidCellFlags[flatIndex] = 1;
        } else {
            cellResiduals[flatIndex] = (Real)(0.0);
            fluidCellFlags[flatIndex] = 0;
        }
    }
}
