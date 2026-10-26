
#include "ParticleTracer/streamline_tracer.hpp"
#include "ParticleTracer/ray_box.hpp"

struct FlowFieldData {
    Real *U, *V, *W, *P, *T;
};

void integrateFlowlineParticle(
        FlowCurve &activePath, const fpvec3 &walkerStartPosition, const fpvec3 &domainOrigin,
        const fpvec3 &domainExtent, Real dt, int imax, int jmax, int kmax, Real dx, Real dy, Real dz,
        const FlowFieldData &fields) {
    Real *U = fields.U, *V = fields.V, *W = fields.W, *P = fields.P, *T = fields.T;
    fpvec3 walkerPosition = walkerStartPosition;
    fpvec3 priorWalkerPosition;

    int stepCount = 0;
    const int STEP_LIMIT = 2000;
    Real curveLength = 0.0;
    const Real MAX_CURVE_LENGTH = domainExtent.x + domainExtent.y + domainExtent.z;
    while (stepCount <= STEP_LIMIT && curveLength <= MAX_CURVE_LENGTH) {
        priorWalkerPosition = walkerPosition;
        if (glm::any(glm::lessThan(walkerPosition, domainOrigin))
                || glm::any(glm::greaterThan(walkerPosition, domainOrigin+domainExtent))) {
            if (!activePath.positions.empty()) {
                fpvec3 rayStart = activePath.positions.back();
                fpvec3 rayHeading = glm::normalize(walkerPosition - rayStart);
                Real enterDist, exitDist;
                castRayAgainstBox(domainOrigin, domainOrigin + domainExtent, rayStart, rayHeading, enterDist, exitDist);
                glm::vec3 boundaryWalkerPosition;
                if (enterDist > 0.0f) {
                    boundaryWalkerPosition = rayStart + enterDist * rayHeading;
                } else {
                    boundaryWalkerPosition = rayStart + exitDist * rayHeading;
                }
                activePath.positions.emplace_back(boundaryWalkerPosition);
                appendTrajectoryFeatures(
                        activePath, {domainOrigin, domainExtent, imax, jmax, kmax, dx, dy, dz}, U, V, W, P, T);
            }
            break;
        }

        activePath.positions.push_back(walkerPosition);
        appendTrajectoryFeatures(
                activePath, {domainOrigin, domainExtent, imax, jmax, kmax, dx, dy, dz}, U, V, W, P, T);

        fpvec3 k1 = dt * evalVelocityAt(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax, U, V, W);
        fpvec3 k2 = dt * evalVelocityAt(walkerPosition + k1/Real(2.0), domainOrigin, domainExtent, imax, jmax, kmax,
                U, V, W);
        fpvec3 k3 = dt * evalVelocityAt(walkerPosition + k2/Real(2.0), domainOrigin, domainExtent, imax, jmax, kmax,
                U, V, W);
        fpvec3 k4 = dt * evalVelocityAt(walkerPosition + k3, domainOrigin, domainExtent, imax, jmax, kmax, U, V, W);
        walkerPosition = walkerPosition + k1/Real(6.0) + k2/Real(3.0) + k3/Real(3.0) + k4/Real(6.0);

        Real stepDistance = glm::length(walkerPosition - priorWalkerPosition);
        curveLength += stepDistance;

        if (stepDistance < Real(0.000001)) {
            break;
        }

        priorWalkerPosition = walkerPosition;
        stepCount++;
    }
}

FlowCurves FlowlineIntegrator::trace(
        const std::vector<fpvec3> &seedPoints, const fpvec3 &domainOrigin, const fpvec3 &domainExtent, Real dt,
        int imax, int jmax, int kmax, Real dx, Real dy, Real dz, Real *U, Real *V, Real *W, Real *P, Real *T) {
    FlowCurves flowCurves;
    flowCurves.resize(seedPoints.size());
    for (size_t i = 0; i < seedPoints.size(); i++) {
        const fpvec3 &walkerStartPosition = seedPoints.at(i);
        FlowCurve &activePath = flowCurves.at(i);
        integrateFlowlineParticle(
                activePath, walkerStartPosition, domainOrigin, domainExtent,
                dt, imax, jmax, kmax, dx, dy, dz, {U, V, W, P, T});
    }
    return flowCurves;
}
