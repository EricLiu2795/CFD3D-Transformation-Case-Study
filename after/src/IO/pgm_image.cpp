
#include <array>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>
#include "IO/pgm_image.hpp"

namespace {

int scaleIndex(int dstIdx, int srcExtent, int dstExtent) {
    return (dstIdx-1) * (srcExtent-2) / (dstExtent-2) + 1;
}

int mapNearestIndex(int dstIdx, int srcExtent, int dstExtent) {
    return dstIdx * srcExtent / dstExtent;
}

}

void nearestNeighborScale2D(
        const std::vector<unsigned int> &pixelsIn,
        int widthSrc,
        int heightSrc,
        std::vector<unsigned int> &pixelsOut,
        int widthDst,
        int heightDst
) {
    assert(widthSrc >= 1 && heightSrc >= 1 && widthDst >= 1 && heightDst >= 1);
    size_t size = size_t(widthDst) * size_t(heightDst);
    pixelsOut.resize(size);

    if (widthSrc == widthDst && heightSrc == heightDst) {
        if (size != 0) {
            memcpy(&pixelsOut.front(), &pixelsIn.front(), sizeof(unsigned int) * size);
        }
        return;
    }

    for (int y = 0; y < heightDst; y++) {
        for (int x = 0; x < widthDst; x++) {
            int srcX = mapNearestIndex(x, widthSrc, widthDst);
            int fetchY = mapNearestIndex(y, heightSrc, heightDst);
            pixelsOut[x*heightDst + y*widthDst] = pixelsIn[srcX*heightSrc + fetchY];
        }
    }
}

std::vector<unsigned int> nearestNeighborScalePgm2D(
        const std::vector<unsigned int> &pixelsIn,
        int widthSrc,
        int heightSrc,
        int widthDst,
        int heightDst
) {
    assert(widthSrc >= 3 && heightSrc >= 3 && widthDst >= 3 && heightDst >= 3);
    size_t size = size_t(widthDst) * size_t(heightDst);
    std::vector<unsigned int> pixelsOut;
    pixelsOut.resize(size);

    if (widthSrc == widthDst && heightSrc == heightDst) {
        memcpy(&pixelsOut.front(), &pixelsIn.front(), sizeof(unsigned int) * size);
        return pixelsOut;
    }

    pixelsOut[0*heightDst + 0] = pixelsIn[0*heightSrc + 0];
    pixelsOut[(widthDst-1)*heightDst + 0] = pixelsIn[(widthSrc-1)*heightSrc + 0];
    pixelsOut[0*heightDst + heightDst-1] = pixelsIn[0 + heightSrc-1];
    pixelsOut[(widthDst-1)*heightDst + heightDst-1] = pixelsIn[(widthSrc-1)*heightSrc + heightSrc-1];

    for (int y = 1; y < heightDst-1; y++) {
        int fetchY = scaleIndex(y, heightSrc, heightDst);
        pixelsOut[0*heightDst + y] = pixelsIn[0*heightSrc + fetchY];
        pixelsOut[(widthDst-1)*heightDst + y] = pixelsIn[(widthSrc-1)*heightSrc + fetchY];
    }

    for (int x = 1; x < widthDst-1; x++) {
        int srcX = scaleIndex(x, widthSrc, widthDst);
        pixelsOut[x*heightDst + 0] = pixelsIn[srcX*heightSrc + 0];
        pixelsOut[x*heightDst + heightDst-1] = pixelsIn[srcX*heightSrc + heightSrc-1];
    }

    for (int y = 1; y < heightDst-1; y++) {
        for (int x = 1; x < widthDst-1; x++) {
            int srcX = scaleIndex(x, widthSrc, widthDst);
            int fetchY = scaleIndex(y, heightSrc, heightDst);
            pixelsOut[x*heightDst + y*widthDst] = pixelsIn[srcX*heightSrc + fetchY];
        }
    }

    return pixelsOut;
}

std::vector<unsigned int> readPgmImage(const std::string &filename, int &width, int &height, int &levels) {
    std::ifstream file(filename.c_str(), std::ifstream::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open file \"" << filename << "\"." << std::endl;
        return {};
    }

    std::array<char, 1024> line;
    std::vector<unsigned int> pgmResized;

    file.read(line.data(), 3);
    if (file.gcount() != 3) {
        std::cerr << "Wrong magic field in file \"" << filename << "\"." << std::endl;
        return {};
    }

    do {
        assert(file.getline(line.data(), line.size()));
    } while (line[0] == '#');

    if (sscanf(line.data(), "%d %d\n", &width, &height) != 2) {
        std::cerr << "Malformed dimension line in file \"" << filename << "\"." << std::endl;
        return {};
    }

    assert(file.getline(line.data(), line.size()));
    sscanf(line.data(), "%d\n", &levels);
    pgmResized.resize(size_t(width) * size_t(height));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; ++x) {
            unsigned int value;
            if (!(file >> value)) {
                std::cerr << "Invalid number of lines in file \"" << filename << "\"." << std::endl;
            }
            pgmResized[size_t(x) * size_t(height) + size_t(y)] = value;
        }
    }

    return pgmResized;
}
