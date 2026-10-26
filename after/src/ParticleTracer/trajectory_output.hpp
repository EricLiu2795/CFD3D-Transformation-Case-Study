
#ifndef FLUID3D_TRAJECTORY_OUTPUT_HPP
#define FLUID3D_TRAJECTORY_OUTPUT_HPP

#include <vector>
#include <glm/glm.hpp>
#include "Defines.hpp"

struct FlowCurve {
    std::vector<fpvec3> positions;
    std::vector<std::vector<Real>> attributes;
};

typedef std::vector<FlowCurve> FlowCurves;

struct SamplingGrid {
    fpvec3 domainOrigin, domainExtent;
    int imax, jmax, kmax;
    Real dx, dy, dz;
};

void appendTrajectoryFeatures(
        FlowCurve &activePath, const SamplingGrid &grid,
        Real *U, Real *V, Real *W, Real *P, Real *T);


fpvec3 evalVelocityAt(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        int imax, int jmax, int kmax, Real *U, Real *V, Real *W);

fpvec3 evalCurlAt(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        int imax, int jmax, int kmax, Real dx, Real dy, Real dz, Real *U, Real *V, Real *W);

#endif
