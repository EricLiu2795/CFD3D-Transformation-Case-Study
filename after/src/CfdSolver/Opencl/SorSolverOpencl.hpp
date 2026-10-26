
#ifndef FLUID3D_RELAXSOLVEROPENCL_HPP
#define FLUID3D_RELAXSOLVEROPENCL_HPP

#include "Defines.hpp"
#include "ClInterface.hpp"

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
        cl::Kernel &relaxSolverIterationOpenclKernel, cl::Kernel &relaxSolverComputeResidualArrayOpenclKernel);

#endif
