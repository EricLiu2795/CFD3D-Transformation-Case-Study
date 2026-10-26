
#ifndef FLUID3D_SCENARIO_BUILDER_HPP
#define FLUID3D_SCENARIO_BUILDER_HPP

#include <string>
#include <functional>
#include "Defines.hpp"

class GeometryBuilder
{
public:
    GeometryBuilder(int imax, int jmax, int kmax, unsigned int fillValue);

    void saveGeometryFile(const std::string &filename);

    void loadLayersFromPgm(const std::string &filename, int layerFrom, int layerTo);

    void fillLayersUniform(FlagType value, int layerFrom, int layerTo);

    void fillLayersInRegion(
            FlagType value, int layerFrom, int layerTo, const std::function<bool(int,int,int)> &insidePredicate);

    void pruneDisallowedCells();

    inline uint32_t cellTypeAt(int i, int j, int k) {
        return shapeValues.at(LINFLAG(i,j,k));
    }

private:
    int imax, jmax, kmax;
    std::vector<uint32_t> shapeValues;
};

void buildScenario(
        const std::string &scenarioName, const std::string &shapeFilename, int imax, int jmax, int kmax);

#endif
