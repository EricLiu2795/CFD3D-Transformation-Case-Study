
#ifndef FLUID3D_PARTICLE_INTEGRATORS_HPP
#define FLUID3D_PARTICLE_INTEGRATORS_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Defines.hpp"
#include "ParticleTracer/trajectory_output.hpp"



class SteadyFlowParticleIntegrator
{
public:
    virtual FlowCurves trace(
            const std::vector<fpvec3> &seedPoints, const fpvec3 &domainOrigin, const fpvec3 &domainExtent, Real dt,
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz, Real *U, Real *V, Real *W, Real *P, Real *T)=0;
};

class TimeVaryingParticleIntegrator
{
public:
    virtual void setSeedPoints(
            const fpvec3 &domainOrigin, const fpvec3 &domainExtent, const std::vector<fpvec3> &seedPoints)=0;

    virtual void advanceTime(
            Real t, Real dt, int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
            Real *U, Real *V, Real *W, Real *P, Real *T)=0;

    virtual FlowCurves getPaths(
            int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
            Real *U, Real *V, Real *W, Real *P, Real *T)=0;
};

std::vector<fpvec3> computeParticleSeedsForScenario(
        const std::string &scenarioName, const fpvec3 &domainOrigin, const fpvec3 &domainExtent, int particleCount);

#endif
