
#ifndef FLUID3D_GEOMETRY_IO_HPP
#define FLUID3D_GEOMETRY_IO_HPP

#include <string>
#include <vector>
#include <cstdint>

std::vector<uint32_t> loadGeometryData(
        const std::string &shapeFilename, int &width, int &height, int &depth);

void saveGeometryData(
        const std::string &shapeFilename, int width, int height, int depth, const std::vector<uint32_t> &values);

#endif
