
#ifndef FLUID3D_PGM_IMAGE_HPP
#define FLUID3D_PGM_IMAGE_HPP

#include <string>
#include <vector>

std::vector<unsigned int> readPgmImage(const std::string &filename, int &width, int &height, int &levels);

std::vector<unsigned int> nearestNeighborScalePgm2D(
        const std::vector<unsigned int> &pixelsIn,
        int widthSrc,
        int heightSrc,
        int widthDst,
        int heightDst
);

void nearestNeighborScale2D(
        const std::vector<unsigned int> &pixelsIn,
        int widthSrc,
        int heightSrc,
        std::vector<unsigned int> &pixelsOut,
        int widthDst,
        int heightDst
);

#endif
