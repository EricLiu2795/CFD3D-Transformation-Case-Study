
#ifndef FLUID3D_STREAMLINE_TRACER_HPP
#define FLUID3D_STREAMLINE_TRACER_HPP

#include "ParticleTracer/particle_integrators.hpp"

class FlowlineIntegrator : public SteadyFlowParticleIntegrator
{
public:
    FlowCurves trace(
            const std::vector<fpvec3> &seedPoints, const fpvec3 &domainOrigin, const fpvec3 &domainExtent, Real dt,
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz, Real *U, Real *V, Real *W, Real *P, Real *T) override;
};

#endif
