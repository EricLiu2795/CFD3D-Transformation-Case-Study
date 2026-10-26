
#include <cstring>
#include "IO/geometry_io.hpp"
#include "CfdSolver/cell_flags.hpp"

std::vector<uint32_t> resizeNearest(
        const std::vector<uint32_t> valuesSrc,
        int widthSrc,
        int heightSrc,
        int depthSrc,
        int widthDst,
        int heightDst,
        int depthDst
) {
    assert(widthSrc >= 3 && heightSrc >= 3 && depthSrc >= 3 && widthDst >= 3 && heightDst >= 3 && depthDst >= 3);
    size_t size = static_cast<size_t>(widthDst * heightDst * depthDst);
    std::vector<uint32_t> valuesDst;
    valuesDst.resize(size);

    if (widthSrc == widthDst && heightSrc == heightDst && depthSrc == depthDst) {
        if (size != 0) {
            memcpy(&valuesDst.front(), &valuesSrc.front(), sizeof(uint32_t) * size);
        }
        return valuesDst;
    }

    for (int x = 0; x <= 1; x++) {
        for (int y = 0; y <= 1; y++) {
            for (int z = 0; z <= 1; z++) {
                valuesDst[static_cast<size_t>(x*(widthDst-1)*heightDst*depthDst + y*(heightDst-1)*depthDst + z*(depthDst-1))]
                        = valuesSrc[static_cast<size_t>(x*(widthSrc-1)*heightSrc*depthSrc + y*(heightSrc-1)*depthSrc + z*(depthSrc-1))];
            }
        }
    }

    for (int x = 1; x < widthDst - 1; x++) {
        for (int y = 1; y < heightDst - 1; y++) {
            int srcX = (x - 1) * (widthSrc - 2) / (widthDst - 2) + 1;
            int fetchY = (y - 1) * (heightSrc - 2) / (heightDst - 2) + 1;
            valuesDst[static_cast<size_t>(x*heightDst*depthDst + y*depthDst + 0)]
                    = valuesSrc[static_cast<size_t>(srcX*heightSrc*depthSrc + fetchY*depthSrc + 0)];
            valuesDst[static_cast<size_t>(x*heightDst*depthDst + y*depthDst + (depthDst-1))]
                    = valuesSrc[static_cast<size_t>(srcX*heightSrc*depthSrc + fetchY*depthSrc + (depthSrc-1))];
        }
    }

    for (int x = 1; x < widthDst - 1; x++) {
        for (int z = 1; z < depthDst - 1; z++) {
            int srcX = (x - 1) * (widthSrc - 2) / (widthDst - 2) + 1;
            int fetchZ = (z - 1) * (depthSrc - 2) / (depthDst - 2) + 1;
            valuesDst[static_cast<size_t>(x*heightDst*depthDst + 0*depthDst + z)]
                    = valuesSrc[static_cast<size_t>(srcX*heightSrc*depthSrc + 0*depthSrc + fetchZ)];
            valuesDst[static_cast<size_t>(x*heightDst*depthDst + (heightDst-1)*depthDst + z)]
                    = valuesSrc[static_cast<size_t>(srcX*heightSrc*depthSrc + (heightSrc-1)*depthSrc + fetchZ)];
        }
    }

    for (int y = 1; y < heightDst - 1; y++) {
        for (int z = 1; z < depthDst - 1; z++) {
            int fetchY = (y-1) * (heightSrc-2) / (heightDst-2) + 1;
            int fetchZ = (z-1) * (depthSrc-2) / (depthDst-2) + 1;
            valuesDst[static_cast<size_t>(0*heightDst*depthDst + y*depthDst + z)]
                    = valuesSrc[static_cast<size_t>(0*heightSrc*depthSrc + fetchY*depthSrc + fetchZ)];
            valuesDst[static_cast<size_t>((widthDst-1)*heightDst*depthDst + y*depthDst + z)]
                    = valuesSrc[static_cast<size_t>((widthSrc-1)*heightSrc*depthSrc + fetchY*depthSrc + fetchZ)];
        }
    }

    for (int x = 1; x < widthDst - 1; x++) {
        for (int y = 1; y < heightDst - 1; y++) {
            for (int z = 1; z < depthDst - 1; z++) {
                int srcX = (x - 1) * (widthSrc - 2) / (widthDst - 2) + 1;
                int fetchY = (y - 1) * (heightSrc - 2) / (heightDst - 2) + 1;
                int fetchZ = (z - 1) * (depthSrc - 2) / (depthDst - 2) + 1;
                valuesDst[static_cast<size_t>(x*heightDst*depthDst + y*depthDst + z)]
                        = valuesSrc[static_cast<size_t>(srcX*heightSrc*depthSrc + fetchY*depthSrc + fetchZ)];
            }
        }
    }

    return valuesDst;
}

void initFlagNoBarriers(const std::string &scenarioName, int imax, int jmax, int kmax, FlagType *&Flag) {
    #pragma omp parallel for shared(imax, jmax, kmax, Flag) default(none)
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                if (i == 0 || j == 0 || k == 0 || i == imax+1 || j == jmax+1 || k == kmax+1) {
                    Flag[LINFLAG(i,j,k)] = 0x2;
                } else {
                    Flag[LINFLAG(i,j,k)] = 0x1;
                }
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, Flag) default(none)
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                if (!isWet(Flag[LINFLAG(i,j,k)])) {
                    if (i > 0 && (Flag[LINFLAG(i - 1, j, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x20;
                    }
                    if (i <= imax && (Flag[LINFLAG(i + 1, j, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x40;
                    }
                    if (j > 0 && (Flag[LINFLAG(i, j - 1, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x80;
                    }
                    if (j <= jmax && (Flag[LINFLAG(i, j + 1, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x100;
                    }
                    if (k > 0 && (Flag[LINFLAG(i, j, k - 1)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x200;
                    }
                    if (k <= kmax && (Flag[LINFLAG(i, j, k + 1)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x400;
                    }
                }
            }
        }
    }
}

void initFlagFromShapeFile(const std::string &scenarioName, const std::string &shapeFilename,
        int imax, int jmax, int kmax, FlagType *&Flag) {
    int width, height, depth;
    std::vector<uint32_t> shapeValuesRead = loadGeometryData(shapeFilename, width, height, depth);
    std::vector<uint32_t> shapeValues;
    if (imax+2 != width || jmax+2 != height || kmax+2 != depth) {
        shapeValues = resizeNearest(shapeValuesRead, width, height, depth, imax+2, jmax+2, kmax+2);
    } else {
        shapeValues = shapeValuesRead;
    }

    const unsigned int FLAG_BITMASK_TABLE[] = {
            0x2, 0x4, 0x8, 0x10, 0x1,
            0x802, 0x804, 0x810,
            0x1002, 0x1004, 0x1010,
            0x2002
    };
    const unsigned int NUM_PGM_ENTRIES = sizeof(FLAG_BITMASK_TABLE) / sizeof(*FLAG_BITMASK_TABLE);
    SUPPRESS_UNUSED(NUM_PGM_ENTRIES);

    #pragma omp parallel for shared(imax, jmax, kmax, shapeValues, FLAG_BITMASK_TABLE, Flag) default(none)
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                unsigned int pgmEntry = shapeValues[LINFLAG(i,j,k)];
                assert(pgmEntry < NUM_PGM_ENTRIES);
                unsigned int flagBits = FLAG_BITMASK_TABLE[pgmEntry];
                Flag[LINFLAG(i,j,k)] = flagBits;
            }
        }
    }

    #pragma omp parallel for shared(imax, jmax, kmax, Flag) default(none)
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = 0; k <= kmax+1; k++) {
                if (!isWet(Flag[LINFLAG(i,j,k)])) {
                    if (i > 0 && (Flag[LINFLAG(i - 1, j, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x20;
                    }
                    if (i <= imax && (Flag[LINFLAG(i + 1, j, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x40;
                    }
                    if (j > 0 && (Flag[LINFLAG(i, j - 1, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x80;
                    }
                    if (j <= jmax && (Flag[LINFLAG(i, j + 1, k)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x100;
                    }
                    if (k > 0 && (Flag[LINFLAG(i, j, k - 1)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x200;
                    }
                    if (k <= kmax && (Flag[LINFLAG(i, j, k + 1)] & 0x1) == 1) {
                        Flag[LINFLAG(i, j, k)] |= 0x400;
                    }
                }

                int wetNeighborCount = 0;
                if (i > 0) wetNeighborCount += Flag[LINFLAG(i-1,j,k)] & 0x1;
                if (i <= imax) wetNeighborCount += Flag[LINFLAG(i+1,j,k)] & 0x1;
                if (j > 0) wetNeighborCount += Flag[LINFLAG(i,j-1,k)] & 0x1;
                if (j <= jmax) wetNeighborCount += Flag[LINFLAG(i,j+1,k)] & 0x1;
                if (k > 0) wetNeighborCount += Flag[LINFLAG(i,j,k-1)] & 0x1;
                if (k <= kmax) wetNeighborCount += Flag[LINFLAG(i,j,k+1)] & 0x1;

                assert((Flag[LINFLAG(i,j,k)] & 0x1) == 1 || wetNeighborCount <= 3);

                assert((Flag[LINFLAG(i,j,k)] & 0x1) == 0 || (Flag[LINFLAG(i,j,k)] & 0x381E) == 0);

                bool wetLeft = i > 0 && (Flag[LINFLAG(i-1,j,k)] & 0x1) == 1;
                bool wetRight = i <= imax && (Flag[LINFLAG(i+1,j,k)] & 0x1) == 1;
                bool wetDown = j > 0 && (Flag[LINFLAG(i,j-1,k)] & 0x1) == 1;
                bool wetUp = j <= jmax && (Flag[LINFLAG(i,j+1,k)] & 0x1) == 1;
                bool wetBack = j <= jmax && (Flag[LINFLAG(i,j,k-1)] & 0x1) == 1;
                bool wetFront = j <= jmax && (Flag[LINFLAG(i,j,k+1)] & 0x1) == 1;
                assert((Flag[LINFLAG(i,j,k)] & 0x1) == 1
                        || ((!wetLeft || !wetRight) && (!wetDown || !wetUp) && (!wetBack || !wetFront)));
                SUPPRESS_UNUSED(wetLeft);
                SUPPRESS_UNUSED(wetRight);
                SUPPRESS_UNUSED(wetDown);
                SUPPRESS_UNUSED(wetUp);
                SUPPRESS_UNUSED(wetBack);
                SUPPRESS_UNUSED(wetFront);

                int borderCode = int(Flag[LINFLAG(i,j,k)] >> 1) & 0xF;
                assert(borderCode == 0x0 || borderCode == 0x1 || borderCode == 0x2
                        || borderCode == 0x4 || borderCode == 0x8);
                SUPPRESS_UNUSED(borderCode);

                assert(!(i > 0 && j > 0 && k > 0 && i <= imax && j <= jmax && k <= kmax)
                        || ((Flag[LINFLAG(i,j,k)] & 0x1) == 1 || borderCode == 1));
            }
        }
    }
}
