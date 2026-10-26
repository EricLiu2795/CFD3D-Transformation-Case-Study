
#ifndef FLUID3D_PATH_WRITER_HPP
#define FLUID3D_PATH_WRITER_HPP

#include <string>
#include <vector>
#include "Defines.hpp"
#include "ParticleTracer/particle_integrators.hpp"

typedef FlowCurves PathCollection;

bool writePathsToObj(const std::string &filename, const PathCollection &flowCurves);

bool writePathsToBinary(const std::string &filename, const PathCollection &flowCurves);

#endif
