
#include <random>
#include <cassert>
#include "ParticleTracer/particle_integrators.hpp"

std::vector<fpvec3> computeParticleSeedsForScenario(
        const std::string &scenarioName, const fpvec3 &domainOrigin, const fpvec3 &domainExtent, int particleCount) {
    std::vector<fpvec3> seedPoints;

    std::random_device entropySource;
    std::mt19937 rngEngine(entropySource());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    for (int i = 0; i < particleCount; i++) {
        fpvec3 walkerPosition(dist(rngEngine), dist(rngEngine), dist(rngEngine));
        seedPoints.push_back(domainOrigin + walkerPosition*domainExtent);
    }

    return seedPoints;
}
