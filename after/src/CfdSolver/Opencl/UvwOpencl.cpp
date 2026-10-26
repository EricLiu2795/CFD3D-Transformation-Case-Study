
#include <algorithm>
#include <iostream>
#include "UvwOpencl.hpp"

void computeDtOpencl(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax, int blockExtent1D,
        cl::DispatchQueue &queue, cl::GridRange teamSize1D, cl::Kernel &computeMaximumKernel,
        cl::Buffer &U, cl::Buffer &V, cl::Buffer &W,
        cl::Buffer &openclCollapseArrayU1, cl::Buffer &openclCollapseArrayU2,
        cl::Buffer &openclCollapseArrayV1, cl::Buffer &openclCollapseArrayV2,
        cl::Buffer &openclCollapseArrayW1, cl::Buffer &openclCollapseArrayW2,
        bool useTemperature) {
    Real uPeak = Real(0.0), vPeak = Real(0.0), wPeak = Real(0.0);

    auto computePeak = cl::KernelInvoker<cl::Buffer, cl::Buffer, cl::SharedMemArg, int>(computeMaximumKernel);

    cl::Buffer *U_foldInput = &U;
    cl::Buffer *V_foldInput = &V;
    cl::Buffer *W_foldInput = &W;
    cl::Buffer *U_foldOutput = &openclCollapseArrayU1;
    cl::Buffer *V_foldOutput = &openclCollapseArrayV1;
    cl::Buffer *W_foldOutput = &openclCollapseArrayW1;

    int blockCountI = (imax+1)*(jmax+2)*(kmax+2);
    int blockCountJ = (imax+2)*(jmax+1)*(kmax+2);
    int blockCountK = (imax+2)*(jmax+2)*(kmax+1);
    int lengthI;
    int lengthJ;
    int lengthK;
    bool finished = false;

    int iteration = 0;
    while (!finished) {
        lengthI = blockCountI;
        lengthJ = blockCountJ;
        lengthK = blockCountK;
        blockCountI = divCeil(blockCountI, blockExtent1D*2);
        blockCountJ = divCeil(blockCountJ, blockExtent1D*2);
        blockCountK = divCeil(blockCountK, blockExtent1D*2);

        if (lengthI != 1) {
            int scratchBufferBytes = blockExtent1D * sizeof(Real);
            cl::LaunchArgs launchArgs(queue, cl::EmptyRange, cl::GridRange(blockCountI*blockExtent1D*2), teamSize1D);
            computePeak(
                    launchArgs, *U_foldInput, *U_foldOutput, cl::Local(scratchBufferBytes), lengthI);
            if (iteration % 2 == 0) {
                U_foldInput = &openclCollapseArrayU1;
                U_foldOutput = &openclCollapseArrayU2;
            } else {
                U_foldInput = &openclCollapseArrayU2;
                U_foldOutput = &openclCollapseArrayU1;
            }
        }
        if (lengthJ != 1) {
            int scratchBufferBytes = blockExtent1D * sizeof(Real);
            cl::LaunchArgs launchArgs(queue, cl::EmptyRange, cl::GridRange(blockCountJ*blockExtent1D*2), teamSize1D);
            computePeak(
                    launchArgs, *V_foldInput, *V_foldOutput, cl::Local(scratchBufferBytes), lengthJ);
            if (iteration % 2 == 0) {
                V_foldInput = &openclCollapseArrayV1;
                V_foldOutput = &openclCollapseArrayV2;
            } else {
                V_foldInput = &openclCollapseArrayV2;
                V_foldOutput = &openclCollapseArrayV1;
            }
        }
        if (lengthK != 1) {
            int scratchBufferBytes = blockExtent1D * sizeof(Real);
            cl::LaunchArgs launchArgs(queue, cl::EmptyRange, cl::GridRange(blockCountK*blockExtent1D*2), teamSize1D);
            computePeak(
                    launchArgs, *W_foldInput, *W_foldOutput, cl::Local(scratchBufferBytes), lengthK);
            if (iteration % 2 == 0) {
                W_foldInput = &openclCollapseArrayW1;
                W_foldOutput = &openclCollapseArrayW2;
            } else {
                W_foldInput = &openclCollapseArrayW2;
                W_foldOutput = &openclCollapseArrayW1;
            }
        }


        if (blockCountI == 1 && blockCountJ == 1 && blockCountK == 1) {
            finished = true;
        }
        iteration++;
    }

    queue.readBufferAsync(*U_foldInput, CL_NONBLOCKING, 0, sizeof(Real), (void*)&uPeak);
    queue.readBufferAsync(*V_foldInput, CL_NONBLOCKING, 0, sizeof(Real), (void*)&vPeak);
    queue.readBufferAsync(*W_foldInput, CL_NONBLOCKING, 0, sizeof(Real), (void*)&wPeak);
    queue.finish();

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

