
#ifndef FLUID3D_RAY_BOX_HPP
#define FLUID3D_RAY_BOX_HPP

#include "Defines.hpp"

bool castRayAgainstBox(
        const fpvec3 &lower, const fpvec3 &upper, const fpvec3 &rayStart, const fpvec3 &rayHeading,
        Real &enterDist, Real &exitDist);

#endif
