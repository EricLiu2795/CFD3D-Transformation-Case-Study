
#include "ParticleTracer/trajectory_output.hpp"

static bool isNodeOutOfRange(const glm::ivec3 &staggeredCell, const glm::ivec3 &upperBound) {
    return glm::any(glm::lessThan(staggeredCell, glm::ivec3(1,1,1)))
            || glm::any(glm::greaterThanEqual(staggeredCell, upperBound));
}

static Real trilinearBlend(const fpvec3 &frac, const fpvec3 &compFrac,
        Real c000, Real c100, Real c010, Real c110,
        Real c001, Real c101, Real c011, Real c111) {
    return compFrac.x*compFrac.y*compFrac.z*c000
            + frac.x*compFrac.y*compFrac.z*c100
            + compFrac.x*frac.y*compFrac.z*c010
            + frac.x*frac.y*compFrac.z*c110
            + compFrac.x*compFrac.y*frac.z*c001
            + frac.x*compFrac.y*frac.z*c101
            + compFrac.x*frac.y*frac.z*c011
            + frac.x*frac.y*frac.z*c111;
}

fpvec3 mapWorldToStaggered(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent, int imax, int jmax, int kmax) {
    return (walkerPosition - domainOrigin) / domainExtent * fpvec3(imax, jmax, kmax) + fpvec3(1, 1, 1);
}
struct StaggeredField {
    Real *values;
    int imax, jmax, kmax;
};

using VelocityField = StaggeredField;

Real fetchWAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *W = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax+1,jmax+1,kmax))) {
        return 0;
    }
    return W[LINW(staggeredCell.x, staggeredCell.y, staggeredCell.z)];
}
Real fetchVAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *V = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax+1,jmax,kmax+1))) {
        return 0;
    }
    return V[LINV(staggeredCell.x, staggeredCell.y, staggeredCell.z)];
}

Real fetchUAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *U = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax,jmax+1,kmax+1))) {
        return 0;
    }
    return U[LINU(staggeredCell.x, staggeredCell.y, staggeredCell.z)];
}

Real trilinearSampleW(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(0.5, 0.5, 1);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchWAtNode(staggeredCell + glm::ivec3(0,0,0), field),
            fetchWAtNode(staggeredCell + glm::ivec3(1,0,0), field),
            fetchWAtNode(staggeredCell + glm::ivec3(0,1,0), field),
            fetchWAtNode(staggeredCell + glm::ivec3(1,1,0), field),
            fetchWAtNode(staggeredCell + glm::ivec3(0,0,1), field),
            fetchWAtNode(staggeredCell + glm::ivec3(1,0,1), field),
            fetchWAtNode(staggeredCell + glm::ivec3(0,1,1), field),
            fetchWAtNode(staggeredCell + glm::ivec3(1,1,1), field));
    return blendedValue;
}

Real trilinearSampleV(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(0.5, 1, 0.5);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchVAtNode(staggeredCell + glm::ivec3(0,0,0), field),
            fetchVAtNode(staggeredCell + glm::ivec3(1,0,0), field),
            fetchVAtNode(staggeredCell + glm::ivec3(0,1,0), field),
            fetchVAtNode(staggeredCell + glm::ivec3(1,1,0), field),
            fetchVAtNode(staggeredCell + glm::ivec3(0,0,1), field),
            fetchVAtNode(staggeredCell + glm::ivec3(1,0,1), field),
            fetchVAtNode(staggeredCell + glm::ivec3(0,1,1), field),
            fetchVAtNode(staggeredCell + glm::ivec3(1,1,1), field));
    return blendedValue;
}

Real trilinearSampleU(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(1, 0.5, 0.5);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchUAtNode(staggeredCell + glm::ivec3(0,0,0), field),
            fetchUAtNode(staggeredCell + glm::ivec3(1,0,0), field),
            fetchUAtNode(staggeredCell + glm::ivec3(0,1,0), field),
            fetchUAtNode(staggeredCell + glm::ivec3(1,1,0), field),
            fetchUAtNode(staggeredCell + glm::ivec3(0,0,1), field),
            fetchUAtNode(staggeredCell + glm::ivec3(1,0,1), field),
            fetchUAtNode(staggeredCell + glm::ivec3(0,1,1), field),
            fetchUAtNode(staggeredCell + glm::ivec3(1,1,1), field));
    return blendedValue;
}

fpvec3 evalVelocityAt(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        int imax, int jmax, int kmax, Real *U, Real *V, Real *W) {
    Real u = trilinearSampleU(walkerPosition, domainOrigin, domainExtent, {U, imax, jmax, kmax});
    Real v = trilinearSampleV(walkerPosition, domainOrigin, domainExtent, {V, imax, jmax, kmax});
    Real w = trilinearSampleW(walkerPosition, domainOrigin, domainExtent, {W, imax, jmax, kmax});
    return fpvec3(u, v, w);
}
Real fetchdWdyAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field, Real dy) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *W = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax+1,jmax,kmax))) {
        return 0;
    }
    return (W[LINW(staggeredCell.x, staggeredCell.y, staggeredCell.z)]
            - W[LINW(staggeredCell.x, staggeredCell.y+1, staggeredCell.z)])/dy;
}
Real fetchdWdxAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field, Real dx) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *W = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax,jmax+1,kmax))) {
        return 0;
    }
    return (W[LINW(staggeredCell.x, staggeredCell.y, staggeredCell.z)]
            - W[LINW(staggeredCell.x+1, staggeredCell.y, staggeredCell.z)])/dx;
}
Real fetchdVdzAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field, Real dz) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *V = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax+1,jmax,kmax))) {
        return 0;
    }
    return (V[LINV(staggeredCell.x, staggeredCell.y, staggeredCell.z)]
            - V[LINV(staggeredCell.x, staggeredCell.y, staggeredCell.z+1)])/dz;
}
Real fetchdVdxAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field, Real dx) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *V = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax,jmax,kmax+1))) {
        return 0;
    }
    return (V[LINV(staggeredCell.x, staggeredCell.y, staggeredCell.z)]
            - V[LINV(staggeredCell.x+1, staggeredCell.y, staggeredCell.z)])/dx;
}
Real fetchdUdzAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field, Real dz) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *U = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax,jmax+1,kmax))) {
        return 0;
    }
    return (U[LINU(staggeredCell.x, staggeredCell.y, staggeredCell.z)]
            - U[LINU(staggeredCell.x, staggeredCell.y, staggeredCell.z+1)])/dz;
}

Real fetchdUdyAtNode(const glm::ivec3 &staggeredCell, const VelocityField &field, Real dy) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    Real *U = field.values;
    if (isNodeOutOfRange(staggeredCell, glm::ivec3(imax,jmax,kmax+1))) {
        return 0;
    }
    return (U[LINU(staggeredCell.x, staggeredCell.y, staggeredCell.z)]
            - U[LINU(staggeredCell.x, staggeredCell.y+1, staggeredCell.z)])/dy;
}
Real trilinearSample_dWdy(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field, Real dy) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(0.5, 1, 1);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchdWdyAtNode(staggeredCell + glm::ivec3(0,0,0), field, dy),
            fetchdWdyAtNode(staggeredCell + glm::ivec3(1,0,0), field, dy),
            fetchdWdyAtNode(staggeredCell + glm::ivec3(0,1,0), field, dy),
            fetchdWdyAtNode(staggeredCell + glm::ivec3(1,1,0), field, dy),
            fetchdWdyAtNode(staggeredCell + glm::ivec3(0,0,1), field, dy),
            fetchdWdyAtNode(staggeredCell + glm::ivec3(1,0,1), field, dy),
            fetchdWdyAtNode(staggeredCell + glm::ivec3(0,1,1), field, dy),
            fetchdWdyAtNode(staggeredCell + glm::ivec3(1,1,1), field, dy));
    return blendedValue;
}
Real trilinearSample_dWdx(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field, Real dx) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(1, 0.5, 1);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchdWdxAtNode(staggeredCell + glm::ivec3(0,0,0), field, dx),
            fetchdWdxAtNode(staggeredCell + glm::ivec3(1,0,0), field, dx),
            fetchdWdxAtNode(staggeredCell + glm::ivec3(0,1,0), field, dx),
            fetchdWdxAtNode(staggeredCell + glm::ivec3(1,1,0), field, dx),
            fetchdWdxAtNode(staggeredCell + glm::ivec3(0,0,1), field, dx),
            fetchdWdxAtNode(staggeredCell + glm::ivec3(1,0,1), field, dx),
            fetchdWdxAtNode(staggeredCell + glm::ivec3(0,1,1), field, dx),
            fetchdWdxAtNode(staggeredCell + glm::ivec3(1,1,1), field, dx));
    return blendedValue;
}
Real trilinearSample_dVdz(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field, Real dz) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(0.5, 1, 1);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchdVdzAtNode(staggeredCell + glm::ivec3(0,0,0), field, dz),
            fetchdVdzAtNode(staggeredCell + glm::ivec3(1,0,0), field, dz),
            fetchdVdzAtNode(staggeredCell + glm::ivec3(0,1,0), field, dz),
            fetchdVdzAtNode(staggeredCell + glm::ivec3(1,1,0), field, dz),
            fetchdVdzAtNode(staggeredCell + glm::ivec3(0,0,1), field, dz),
            fetchdVdzAtNode(staggeredCell + glm::ivec3(1,0,1), field, dz),
            fetchdVdzAtNode(staggeredCell + glm::ivec3(0,1,1), field, dz),
            fetchdVdzAtNode(staggeredCell + glm::ivec3(1,1,1), field, dz));
    return blendedValue;
}
Real trilinearSample_dVdx(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field, Real dx) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(1, 1, 0.5);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchdVdxAtNode(staggeredCell + glm::ivec3(0,0,0), field, dx),
            fetchdVdxAtNode(staggeredCell + glm::ivec3(1,0,0), field, dx),
            fetchdVdxAtNode(staggeredCell + glm::ivec3(0,1,0), field, dx),
            fetchdVdxAtNode(staggeredCell + glm::ivec3(1,1,0), field, dx),
            fetchdVdxAtNode(staggeredCell + glm::ivec3(0,0,1), field, dx),
            fetchdVdxAtNode(staggeredCell + glm::ivec3(1,0,1), field, dx),
            fetchdVdxAtNode(staggeredCell + glm::ivec3(0,1,1), field, dx),
            fetchdVdxAtNode(staggeredCell + glm::ivec3(1,1,1), field, dx));
    return blendedValue;
}
Real trilinearSample_dUdz(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field, Real dz) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(1, 0.5, 1);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchdUdzAtNode(staggeredCell + glm::ivec3(0,0,0), field, dz),
            fetchdUdzAtNode(staggeredCell + glm::ivec3(1,0,0), field, dz),
            fetchdUdzAtNode(staggeredCell + glm::ivec3(0,1,0), field, dz),
            fetchdUdzAtNode(staggeredCell + glm::ivec3(1,1,0), field, dz),
            fetchdUdzAtNode(staggeredCell + glm::ivec3(0,0,1), field, dz),
            fetchdUdzAtNode(staggeredCell + glm::ivec3(1,0,1), field, dz),
            fetchdUdzAtNode(staggeredCell + glm::ivec3(0,1,1), field, dz),
            fetchdUdzAtNode(staggeredCell + glm::ivec3(1,1,1), field, dz));
    return blendedValue;
}

Real trilinearSample_dUdy(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        const VelocityField &field, Real dy) {
    const int imax = field.imax, jmax = field.jmax, kmax = field.kmax;
    fpvec3 staggeredCoord = mapWorldToStaggered(walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax) - fpvec3(1, 1, 0.5);
    glm::ivec3 staggeredCell = glm::ivec3(staggeredCoord);
    fpvec3 frac = glm::fract(staggeredCoord);
    fpvec3 compFrac = fpvec3(1.0) - frac;
    Real blendedValue = trilinearBlend(frac, compFrac,
            fetchdUdyAtNode(staggeredCell + glm::ivec3(0,0,0), field, dy),
            fetchdUdyAtNode(staggeredCell + glm::ivec3(1,0,0), field, dy),
            fetchdUdyAtNode(staggeredCell + glm::ivec3(0,1,0), field, dy),
            fetchdUdyAtNode(staggeredCell + glm::ivec3(1,1,0), field, dy),
            fetchdUdyAtNode(staggeredCell + glm::ivec3(0,0,1), field, dy),
            fetchdUdyAtNode(staggeredCell + glm::ivec3(1,0,1), field, dy),
            fetchdUdyAtNode(staggeredCell + glm::ivec3(0,1,1), field, dy),
            fetchdUdyAtNode(staggeredCell + glm::ivec3(1,1,1), field, dy));
    return blendedValue;
}

fpvec3 evalCurlAt(
        const fpvec3 &walkerPosition, const fpvec3 &domainOrigin, const fpvec3 &domainExtent,
        int imax, int jmax, int kmax, Real dx, Real dy, Real dz, Real *U, Real *V, Real *W) {
    VelocityField fieldU{U, imax, jmax, kmax}, fieldV{V, imax, jmax, kmax}, fieldW{W, imax, jmax, kmax};
    Real gradUy = trilinearSample_dUdy(walkerPosition, domainOrigin, domainExtent, fieldU, dy);
    Real gradUz = trilinearSample_dUdz(walkerPosition, domainOrigin, domainExtent, fieldU, dz);
    Real gradVx = trilinearSample_dVdx(walkerPosition, domainOrigin, domainExtent, fieldV, dx);
    Real gradVz = trilinearSample_dVdz(walkerPosition, domainOrigin, domainExtent, fieldV, dz);
    Real gradWx = trilinearSample_dWdx(walkerPosition, domainOrigin, domainExtent, fieldW, dx);
    Real gradWy = trilinearSample_dWdy(walkerPosition, domainOrigin, domainExtent, fieldW, dy);
    return fpvec3(gradWy - gradVz, gradUz - gradWx, gradVx - gradUy);
}

void appendTrajectoryFeatures(
        FlowCurve &activePath, const SamplingGrid &grid,
        Real *U, Real *V, Real *W, Real *P, Real *T) {
    const fpvec3 &domainOrigin = grid.domainOrigin, &domainExtent = grid.domainExtent;
    const int imax = grid.imax, jmax = grid.jmax, kmax = grid.kmax;
    const Real dx = grid.dx, dy = grid.dy, dz = grid.dz;

    if (activePath.attributes.empty()) {
        activePath.attributes.resize(2);
    }

    fpvec3 walkerPosition = activePath.positions.back();
    activePath.attributes.at(0).push_back(glm::length(evalCurlAt(
            walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax, dx, dy, dz, U, V, W)));
    activePath.attributes.at(1).push_back(glm::length(evalVelocityAt(
            walkerPosition, domainOrigin, domainExtent, imax, jmax, kmax, U, V, W)));

}
