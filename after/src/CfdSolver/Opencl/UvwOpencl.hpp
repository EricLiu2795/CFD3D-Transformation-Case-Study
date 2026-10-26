
#ifndef FLUID3D_UVWOPENCL_HPP
#define FLUID3D_UVWOPENCL_HPP

#include "Defines.hpp"
#include "ClInterface.hpp"

void computeDtOpencl(
        Real Re, Real Pr, Real tau,
        Real &dt, Real dx, Real dy, Real dz, int imax, int jmax, int kmax, int blockExtent1D,
        cl::DispatchQueue &queue, cl::GridRange teamSize1D, cl::Kernel &computeMaximumKernel,
        cl::Buffer &U, cl::Buffer &V, cl::Buffer &W,
        cl::Buffer &openclCollapseArrayU1, cl::Buffer &openclCollapseArrayU2,
        cl::Buffer &openclCollapseArrayV1, cl::Buffer &openclCollapseArrayV2,
        cl::Buffer &openclCollapseArrayW1, cl::Buffer &openclCollapseArrayW2,
        bool useTemperature);

#endif
