
#include <algorithm>
#include <glm/glm.hpp>
#include "IO/text_file_utils.hpp"
#include "IO/pgm_image.hpp"
#include "IO/geometry_io.hpp"
#include "IO/scenario_builder.hpp"

enum CellKind {
    CELL_NO_SLIP, CELL_FREE_SLIP, CELL_OUTFLOW, CELL_INFLOW, CELL_FLUID,
    CELL_NO_SLIP_HOT, CELL_FREE_SLIP_HOT, CELL_INFLOW_HOT,
    CELL_NO_SLIP_COLD, CELL_FREE_SLIP_COLD, CELL_INFLOW_COLD,
    CELL_COUPLING
};

GeometryBuilder::GeometryBuilder(int imax, int jmax, int kmax, unsigned int fillValue)
        : imax(imax), jmax(jmax), kmax(kmax) {
    shapeValues.resize((imax+2)*(jmax+2)*(kmax+2), fillValue);

    #pragma omp parallel for shared(imax, jmax, kmax) default(none)
    for (int i = 1; i <= imax; i++) {
        for (int j = 1; j <= jmax; j++) {
            for (int k = 1; k <= kmax; k++) {
                shapeValues[LINFLAG(i,j,k)] = CELL_FLUID;
            }
        }
    }
}

void GeometryBuilder::saveGeometryFile(const std::string &filename) {
    saveGeometryData(filename, imax + 2, jmax + 2, kmax + 2, shapeValues);
}

void GeometryBuilder::fillLayersUniform(FlagType value, int layerFrom, int layerTo) {
#if _OPENMP >= 201107
    #pragma omp parallel for shared(imax, jmax, layerFrom, layerTo, value) default(none)
#endif
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = layerFrom; k <= layerTo; k++) {
                shapeValues[LINFLAG(i,j,k)] = value;
            }
        }
    }
}

void GeometryBuilder::fillLayersInRegion(
        FlagType value, int layerFrom, int layerTo, const std::function<bool(int,int,int)> &insidePredicate) {
#if _OPENMP >= 201107
    #pragma omp parallel for shared(imax, jmax, layerFrom, layerTo, insidePredicate, value) default(none)
#endif
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = layerFrom; k <= layerTo; k++) {
                if (insidePredicate(i,j,k)) {
                    shapeValues[LINFLAG(i,j,k)] = value;
                }
            }
        }
    }
}

void GeometryBuilder::loadLayersFromPgm(const std::string &filename, int layerFrom, int layerTo) {
    int pgmCols, pgmRows, levels;
    std::vector<unsigned int> pgmSource = readPgmImage(filename, pgmCols, pgmRows, levels);
    std::vector<unsigned int> pgmResized = nearestNeighborScalePgm2D(pgmSource, pgmCols, pgmRows, imax+2, jmax+2);

#if _OPENMP >= 201107
    #pragma omp parallel for shared(imax, jmax, layerFrom, layerTo, pgmResized) default(none)
#endif
    for (int i = 0; i <= imax+1; i++) {
        for (int j = 0; j <= jmax+1; j++) {
            for (int k = layerFrom; k <= layerTo; k++) {
                shapeValues[LINFLAG(i,j,k)] = pgmResized[i*(imax+2) + j];
            }
        }
    }
}

bool isCellAllowed(int i, int j, int k, int imax, int jmax, int kmax, const FlagType *Flag) {
    bool wetLeft = i > 0 && Flag[LINFLAG(i-1,j,k)] == CELL_FLUID;
    bool wetRight = i <= imax && Flag[LINFLAG(i+1,j,k)] == CELL_FLUID;
    bool fluidBelow = j > 0 && Flag[LINFLAG(i,j-1,k)] == CELL_FLUID;
    bool fluidAbove = j <= jmax && Flag[LINFLAG(i,j+1,k)] == CELL_FLUID;
    bool wetBack = j > 0 && Flag[LINFLAG(i,j,k-1)] == CELL_FLUID;
    bool wetFront = j <= jmax && Flag[LINFLAG(i,j,k+1)] == CELL_FLUID;
    return (Flag[LINFLAG(i,j,k)] == CELL_FLUID
            || ((!wetLeft || !wetRight) && (!fluidBelow || !fluidAbove)
                && (!wetBack || !wetFront)));
}

void GeometryBuilder::pruneDisallowedCells() {
    bool foundDisallowedCell = true;

    while (foundDisallowedCell) {
        foundDisallowedCell = false;

        for (int j = jmax; j >= 1; j--) {
            for (int i = 1; i <= imax; i++) {
                for (int k = 1; k <= kmax; k++) {
                    if (!isCellAllowed(i, j, k, imax, jmax, kmax, &shapeValues.front())) {
                        shapeValues[LINFLAG(i,j,k)] = CELL_FLUID;
                        foundDisallowedCell = true;
                    }
                }
            }
        }
    }
}

void buildRayleighBenardGeometry(
        const std::string &scenarioName, const std::string &shapeFilename, int imax, int jmax, int kmax) {
    GeometryBuilder geometryBuilder(imax, jmax, kmax, CELL_NO_SLIP);
    geometryBuilder.fillLayersInRegion(CELL_NO_SLIP_HOT, 0, kmax+1, [&](int i, int j, int k) {
        return j == 0 && i > 0 && k > 0 && i <= imax && k <= kmax;
    });
    geometryBuilder.fillLayersInRegion(CELL_NO_SLIP_COLD, 0, kmax+1, [&](int i, int j, int k) {
        return j == jmax+1 && i > 0 && k > 0 && i <= imax && k <= kmax;
    });
    geometryBuilder.saveGeometryFile(shapeFilename);
}


void buildFreeConvectionGeometry(
        const std::string &scenarioName, const std::string &shapeFilename, int imax, int jmax, int kmax) {
    GeometryBuilder geometryBuilder(imax, jmax, kmax, CELL_NO_SLIP);
    geometryBuilder.fillLayersInRegion(CELL_NO_SLIP_HOT, 0, kmax+1, [&](int i, int j, int k) {
        return i == 0 && j > 0 && k > 0 && j <= jmax && k <= kmax;
    });
    geometryBuilder.fillLayersInRegion(CELL_NO_SLIP_COLD, 0, kmax+1, [&](int i, int j, int k) {
        return i == imax+1 && j > 0 && k > 0 && j <= jmax && k <= kmax;
    });
    geometryBuilder.saveGeometryFile(shapeFilename);
}

void buildTowerGeometry(
        const std::string &scenarioName, const std::string &shapeFilename, int imax, int jmax, int kmax) {
    glm::ivec3 towerOrigin(imax/3, 0, kmax/2);
    int width = kmax / 4;
    int height = jmax*2/3;
    glm::ivec3 towerLow = towerOrigin + glm::ivec3(-width/2, 0, -width/2);
    glm::ivec3 towerHigh = towerOrigin + glm::ivec3(width/2, height, width/2);

    GeometryBuilder geometryBuilder(imax, jmax, kmax, CELL_NO_SLIP);

    geometryBuilder.fillLayersInRegion(CELL_NO_SLIP, 0, kmax+1, [&](int i, int j, int k) {
        glm::ivec3 cellPos(i, j, k);
        return glm::all(glm::greaterThanEqual(cellPos, towerLow)) && glm::all(glm::lessThanEqual(cellPos, towerHigh));
    });

    geometryBuilder.fillLayersInRegion(CELL_INFLOW, 0, kmax+1, [&](int i, int j, int k) {
        return i == 0 && j >= 1 && j <= jmax && k >= 1 && k <= kmax;
    });
    geometryBuilder.fillLayersInRegion(CELL_OUTFLOW, 0, kmax+1, [&](int i, int j, int k) {
        return i == imax+1 && j >= 1 && j <= jmax && k >= 1 && k <= kmax;
    });

    geometryBuilder.saveGeometryFile(shapeFilename);
}

void buildStepFlowGeometry(
        const std::string &scenarioName, const std::string &shapeFilename, int imax, int jmax, int kmax) {
    GeometryBuilder geometryBuilder(imax, jmax, kmax, CELL_NO_SLIP);

    geometryBuilder.fillLayersInRegion(CELL_NO_SLIP, 0, kmax+1, [&](int i, int j, int k) {
        return i <= jmax/2 && j <= jmax/2;
    });

    geometryBuilder.fillLayersInRegion(CELL_INFLOW, 0, kmax+1, [&](int i, int j, int k) {
        return i == 0 && j >= 1 && j <= jmax && k >= 1 && k <= kmax
               && geometryBuilder.cellTypeAt(i+1,j,k) == CELL_FLUID;
    });
    geometryBuilder.fillLayersInRegion(CELL_OUTFLOW, 0, kmax+1, [&](int i, int j, int k) {
        return i == imax+1 && j >= 1 && j <= jmax && k >= 1 && k <= kmax
               && geometryBuilder.cellTypeAt(i-1,j,k) == CELL_FLUID;
    });

    geometryBuilder.saveGeometryFile(shapeFilename);
}

void buildElevationGeometry(
        const std::string &scenarioName, const std::string &shapeFilename,
        const std::string &elevationFilename,
        int imax, int jmax, int kmax) {
    int pgmCols = 0;
    int pgmRows = 0;
    int levels = 1;
    std::vector<unsigned int> elevationPgmData = readPgmImage(elevationFilename, pgmCols, pgmRows, levels);
    std::vector<unsigned int> elevationInts;
    nearestNeighborScale2D(elevationPgmData, pgmCols, pgmRows, elevationPgmData, imax, kmax);
    std::vector<Real> elevationFloats;
    elevationFloats.resize(elevationPgmData.size());
    std::transform(elevationPgmData.begin(), elevationPgmData.end(), elevationFloats.begin(),
            [levels](unsigned int sampleValue) { return Real(sampleValue) / Real(levels); });

    GeometryBuilder geometryBuilder(imax, jmax, kmax, CELL_NO_SLIP);
    geometryBuilder.fillLayersInRegion(CELL_NO_SLIP, 1, kmax, [&](int i, int j, int k) {
        if (i < 1 || i > imax || k < 1 || k > kmax) {
            return false;
        }
        Real elevationValue = elevationFloats.at((i-1) * kmax + (k-1));
        int columnHeight = static_cast<int>(elevationValue * Real(jmax));
        return j <= columnHeight;
    });
    geometryBuilder.pruneDisallowedCells();

    geometryBuilder.fillLayersInRegion(CELL_INFLOW, 0, kmax+1, [&](int i, int j, int k) {
        return i == 0 && j >= 1 && j <= jmax && k >= 1 && k <= kmax
               && geometryBuilder.cellTypeAt(i+1,j,k) == CELL_FLUID;
    });
    geometryBuilder.fillLayersInRegion(CELL_OUTFLOW, 0, kmax+1, [&](int i, int j, int k) {
        return i == imax+1 && j >= 1 && j <= jmax && k >= 1 && k <= kmax
               && geometryBuilder.cellTypeAt(i-1,j,k) == CELL_FLUID;
    });

    geometryBuilder.saveGeometryFile(shapeFilename);
}

void buildScenario(
        const std::string &scenarioName, const std::string &shapeFilename, int imax, int jmax, int kmax) {
    if (scenarioName == "natural_convection") {
        buildFreeConvectionGeometry(scenarioName, shapeFilename, imax, jmax, kmax);
    } else if (hasPrefix(scenarioName, "rayleigh_benard")) {
        buildRayleighBenardGeometry(scenarioName, shapeFilename, imax, jmax, kmax);
    } else if (hasPrefix(scenarioName, "flow_over_step")) {
        buildStepFlowGeometry(scenarioName, shapeFilename, imax, jmax, kmax);
    } else if (hasPrefix(scenarioName, "single_tower")) {
        buildTowerGeometry(scenarioName, shapeFilename, imax, jmax, kmax);
    } else if (hasPrefix(scenarioName, "terrain_1")) {
        buildElevationGeometry(scenarioName, shapeFilename, "../geometry-pgm/heightmap1.pgm", imax, jmax, kmax);
    } else if (hasPrefix(scenarioName, "fuji_san")) {
        buildElevationGeometry(scenarioName, shapeFilename, "../geometry-pgm/Fuji-san.pgm", imax, jmax, kmax);
    } else if (hasPrefix(scenarioName, "zugspitze")) {
        buildElevationGeometry(scenarioName, shapeFilename, "../geometry-pgm/Zugspitze.pgm", imax, jmax, kmax);
    } else if (hasPrefix(scenarioName, "HK_Habor")) {
        buildElevationGeometry(scenarioName, shapeFilename, "../geometry-pgm/HK_Habor.pgm", imax, jmax, kmax);
    }
}
