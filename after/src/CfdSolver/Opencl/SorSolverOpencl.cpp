
#include <iostream>
#include <cmath>
#include "SorSolverOpencl.hpp"

Real collapseSumOpenclReal(
        cl::DispatchQueue &queue, cl::GridRange teamSize1D, cl::Kernel &computeSumKernel,
        cl::Buffer &input, unsigned int valueCount, cl::Buffer &openclCollapseHelperArray, int blockExtent1D) {
    Real sumResult = Real(0);

    auto computeSum = cl::KernelInvoker<cl::Buffer, cl::Buffer, cl::SharedMemArg, int>(computeSumKernel);

    cl::Buffer *reduceSrc = &input;
    cl::Buffer *reduceDst = &openclCollapseHelperArray;

    int blockCount = valueCount;
    int inputLen;
    bool finished = false;

    int iteration = 0;
    while (!finished) {
        inputLen = blockCount;
        blockCount = divCeil(blockCount, blockExtent1D*2);

        if (inputLen != 1) {
            int scratchBufferBytes = blockExtent1D * sizeof(Real);
            cl::LaunchArgs launchArgs(queue, cl::EmptyRange, cl::GridRange(blockCount*blockExtent1D*2), teamSize1D);
            computeSum(launchArgs, *reduceSrc, *reduceDst, cl::Local(scratchBufferBytes), inputLen);
            if (iteration % 2 == 0) {
                reduceSrc = &openclCollapseHelperArray;
                reduceDst = &input;
            } else {
                reduceSrc = &input;
                reduceDst = &openclCollapseHelperArray;
            }
        }

        if (blockCount == 1) {
            finished = true;
        }
        iteration++;
    }

    queue.readBufferAsync(*reduceSrc, CL_NONBLOCKING, 0, sizeof(Real), (void*)&sumResult);
    queue.finish();

    return sumResult;
}

unsigned int collapseSumOpenclUint(
        cl::DispatchQueue &queue, cl::GridRange teamSize1D, cl::Kernel &computeSumKernel,
        cl::Buffer &input, unsigned int valueCount, cl::Buffer &openclCollapseHelperArray, int blockExtent1D) {
    unsigned int sumResult = 0u;

    auto computeSum = cl::KernelInvoker<cl::Buffer, cl::Buffer, cl::SharedMemArg, int>(computeSumKernel);

    cl::Buffer *reduceSrc = &input;
    cl::Buffer *reduceDst = &openclCollapseHelperArray;

    int blockCount = valueCount;
    int inputLen;
    bool finished = false;

    int iteration = 0;
    while (!finished) {
        inputLen = blockCount;
        blockCount = divCeil(blockCount, blockExtent1D*2);

        if (inputLen != 1) {
            int scratchBufferBytes = blockExtent1D * sizeof(Real);
            cl::LaunchArgs launchArgs(queue, cl::EmptyRange, cl::GridRange(blockCount*blockExtent1D*2), teamSize1D);
            computeSum(launchArgs, *reduceSrc, *reduceDst, cl::Local(scratchBufferBytes), inputLen);
            if (iteration % 2 == 0) {
                reduceSrc = &openclCollapseHelperArray;
                reduceDst = &input;
            } else {
                reduceSrc = &input;
                reduceDst = &openclCollapseHelperArray;
            }
        }

        if (blockCount == 1) {
            finished = true;
        }
        iteration++;
    }

    queue.readBufferAsync(*reduceSrc, CL_NONBLOCKING, 0, sizeof(unsigned int), (void*)&sumResult);
    queue.finish();

    return sumResult;
}



void relaxSolverOpencl(
        Real omg, Real eps, int itermax, LinearSystemSolverType linearSystemSolverType, bool shallWriteOutput,
        Real dx, Real dy, Real dz, int imax, int jmax, int kmax,
        int tileSizeX, int blockExtentY, int blockExtentZ, int blockExtent1D,
        cl::DispatchQueue &queue, cl::GridRange teamSize1D, cl::GridRange teamSize2D, cl::GridRange teamSize3D,
        cl::Buffer &P, cl::Buffer &P_prev, cl::Buffer &RS, cl::Buffer &Flag,
        cl::Buffer &openclCollapseArrayResidual1, cl::Buffer &openclCollapseArrayResidual2,
        cl::Buffer &openclCollapseArrayNumCells1, cl::Buffer &openclCollapseArrayNumCells2,
        cl::Kernel &assignXYPlanesPressureBordersOpenclKernel, cl::Kernel &assignXZPlanesPressureBordersOpenclKernel,
        cl::Kernel &assignYZPlanesPressureBordersOpenclKernel,
        cl::Kernel &assignBorderConditionsPressureInDomainOpenclKernel, cl::Kernel &clonePressureOpenclKernel,
        cl::Kernel &collapseSumOpenclKernelReal, cl::Kernel &collapseSumOpenclKernelUint,
        cl::Kernel &relaxSolverIterationOpenclKernel, cl::Kernel &relaxSolverComputeResidualArrayOpenclKernel) {
    if (linearSystemSolverType == POISSON_RELAX || linearSystemSolverType == POISSON_RELAX_PARALLEL) {
        omg = 1.5;
    } else {
        omg = 1.0;
    }


    cl::LaunchArgs launchArgsXY(queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent2D(jmax, imax, teamSize2D), teamSize2D);
    auto assignXYPlanesPressureBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer>(
            assignXYPlanesPressureBordersOpenclKernel);

    cl::LaunchArgs launchArgsXZ(queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent2D(kmax, imax, teamSize2D), teamSize2D);
    auto assignXZPlanesPressureBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer>(
            assignXZPlanesPressureBordersOpenclKernel);

    cl::LaunchArgs launchArgsYZ(queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent2D(kmax, jmax, teamSize2D), teamSize2D);
    auto assignYZPlanesPressureBordersOpencl = cl::KernelInvoker<int, int, int, cl::Buffer>(
            assignYZPlanesPressureBordersOpenclKernel);

    cl::LaunchArgs launchArgs3D(
            queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent3D(kmax, jmax, imax, teamSize3D), teamSize3D);
    auto assignBorderConditionsPressureInDomainOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(
            assignBorderConditionsPressureInDomainOpenclKernel);

    cl::LaunchArgs launchArgsWholeDomain3D(
            queue, cl::EmptyRange,
            ClInterface::get()->alignedExtent3D(kmax+2, jmax+2, imax+2, teamSize3D), teamSize3D);
    auto clonePressureOpencl = cl::KernelInvoker<int, int, int, cl::Buffer, cl::Buffer>(clonePressureOpenclKernel);

    auto relaxSolverIterationOpencl = cl::KernelInvoker<
            Real, Real, Real, Real, Real, int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
            relaxSolverIterationOpenclKernel);

    auto relaxSolverComputeResidualArrayOpencl = cl::KernelInvoker<
            Real, Real, Real, int, int, int, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(
            relaxSolverComputeResidualArrayOpenclKernel);


    const Real coeff = Real(omg / (2.0 * (1.0 / (dx*dx) + 1.0 / (dy*dy) + 1.0 / (dz*dz))));
    Real residual = 1e9;
    int it = 0;
    while (it < itermax && residual > eps) {
        assignXYPlanesPressureBordersOpencl(launchArgsXY, imax, jmax, kmax, P);
        assignXZPlanesPressureBordersOpencl(launchArgsXZ, imax, jmax, kmax, P);
        assignYZPlanesPressureBordersOpencl(launchArgsYZ, imax, jmax, kmax, P);
        assignBorderConditionsPressureInDomainOpencl(launchArgs3D, imax, jmax, kmax, P, Flag);

        clonePressureOpencl(launchArgsWholeDomain3D, imax, jmax, kmax, P, P_prev);

        relaxSolverIterationOpencl(launchArgs3D, omg, dx, dy, dz, coeff, imax, jmax, kmax, P, P_prev, RS, Flag);
        relaxSolverComputeResidualArrayOpencl(
                launchArgs3D, dx, dy, dz, imax, jmax, kmax, P, RS, Flag,
                openclCollapseArrayResidual1, openclCollapseArrayNumCells1);

        residual = collapseSumOpenclReal(
                queue, teamSize1D, collapseSumOpenclKernelReal, openclCollapseArrayResidual1,
                imax*jmax*kmax, openclCollapseArrayResidual2, blockExtent1D);
        unsigned int fluidCellCount = collapseSumOpenclUint(
                queue, teamSize1D, collapseSumOpenclKernelUint, openclCollapseArrayNumCells1,
                imax*jmax*kmax, openclCollapseArrayNumCells2, blockExtent1D);
        residual = std::sqrt(residual / Real(fluidCellCount));

        it++;
    }

    if (((residual > eps && it == itermax) || std::isnan(residual)) && shallWriteOutput) {
        std::cerr << "\nSOR solver reached maximum number of iterations without converging (res: "
                  << residual << ")." << std::endl;
    }
    if (std::isnan(residual)) {
        std::cerr << "\nResidual in SOR solver is not a number." << std::endl;
        exit(1);
    }
}
