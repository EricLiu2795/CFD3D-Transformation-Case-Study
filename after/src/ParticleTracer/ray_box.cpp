
#include "ParticleTracer/ray_box.hpp"
#include <cmath>

#define BIAS (Real(0.00001))

bool castRayAgainstSlab(
        Real rayStartX, Real rayHeadingX, Real minBoundX, Real maxBoundX, Real &enterDist, Real &exitDist);

bool castRayAgainstBox(
        const fpvec3 &lower, const fpvec3 &upper, const fpvec3 &rayStart, const fpvec3 &rayHeading,
        Real &enterDist, Real &exitDist) {
    enterDist = std::numeric_limits<Real>::lowest();
    exitDist = std::numeric_limits<Real>::max();
    for (int i = 0; i < 3; i++) {
        if (!castRayAgainstSlab(rayStart[i], rayHeading[i], lower[i], upper[i], enterDist, exitDist)) {
            return false;
        }
    }

    return true;
}

bool castRayAgainstSlab(
        Real rayStartX, Real rayHeadingX, Real minBoundX, Real maxBoundX, Real &enterDist, Real &exitDist) {
    if (std::abs(rayHeadingX) < BIAS) {
        if (rayStartX < minBoundX || rayStartX > maxBoundX) {
            return false;
        }
    } else {
        float t0 = (minBoundX - rayStartX) / rayHeadingX;
        float t1 = (maxBoundX - rayStartX) / rayHeadingX;
        if (t0 > t1) {
            float tmp = t0;
            t0 = t1;
            t1 = tmp;
        }

        if (t0 > enterDist) {
            enterDist = t0;
        }
        if (t1 < exitDist) {
            exitDist = t1;
        }
        if (enterDist > exitDist) {
            return false;
        }
        if (exitDist < 0) {
            return false;
        }
    }
    return true;
}
