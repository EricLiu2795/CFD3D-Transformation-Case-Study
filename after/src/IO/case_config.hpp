
#ifndef FLUID3D_CASE_CONFIG_HPP
#define FLUID3D_CASE_CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include "Defines.hpp"

struct DomainGeometry {
    Real xLength, yLength, zLength;
    Real xOrigin, yOrigin, zOrigin;
};

void loadCaseConfigFromFile(
        const std::string &caseFilename, std::string &scenarioName, std::string &shapeName,
        Real &tEnd, Real &dtWrite, DomainGeometry &geometry,
        Real &UI, Real &VI, Real &WI, Real &PI, Real &TI, Real &GX, Real &GY, Real &GZ,
        Real &Re, Real &Pr, Real &omg, Real &eps, int &itermax, Real &alpha, Real &beta, Real &dt, Real &tau,
        bool &useTemperature, Real &T_h, Real &T_c,
        int &imax, int &jmax, int &kmax, Real &dx, Real &dy, Real &dz);

#endif
