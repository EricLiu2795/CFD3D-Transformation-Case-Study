
#include <iostream>
#include <fstream>
#include "IO/text_file_utils.hpp"
#include "IO/case_config.hpp"

std::map<std::string, std::string> readSettingsFile(const std::string &filename) {
    std::map<std::string, std::string> variables;
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error in loadCaseConfigFromFile: File \"" << filename
                  << "\" does not exist." << std::endl;
        return variables;
    }

    std::string rawLine;
    while (getline(file, rawLine)) {
        while (!rawLine.empty() && (rawLine[rawLine.size()-1] == '\r' || rawLine[rawLine.size()-1] == ' ')) {
            rawLine = rawLine.substr(0, rawLine.size() - 1);
        }

        if (rawLine.empty() || rawLine.at(0) == '#') {
            continue;
        }

        std::vector<std::string> line;
        tokenizeBySpace(rawLine, line);

        if (line.empty()) {
            continue;
        }
        assert(line.size() == 2);

        std::string key = line.at(0);
        std::string value = line.at(1);
        variables.insert(std::make_pair(key, value));
    }

    file.close();
    return variables;
}

std::string fetchStringSetting(const std::map<std::string, std::string> &variables, const std::string &name) {
    auto it = variables.find(name);
    if (it == variables.end()) {
        std::cerr << "Variable '" << name << "' not found in .dat file." << std::endl;
        exit(1);
    }
    return it->second;
}

std::string fetchStringSettingOrDefault(
        const std::map<std::string, std::string> &variables, const std::string &name,
        const std::string &fallback, bool &settingPresent) {
    auto it = variables.find(name);
    if (it == variables.end()) {
        settingPresent = false;
        return fallback;
    }
    return it->second;
}

Real fetchRealSetting(const std::map<std::string, std::string> &variables, const std::string &name) {
    auto it = variables.find(name);
    if (it == variables.end()) {
        std::cerr << "Variable '" << name << "' not found in .dat file." << std::endl;
        exit(1);
    }
    return stringToFp(it->second);
}

Real fetchRealSettingOrDefault(
        const std::map<std::string, std::string> &variables, const std::string &name,
        Real fallback, bool &settingPresent) {
    auto it = variables.find(name);
    if (it == variables.end()) {
        settingPresent = false;
        return fallback;
    }
    return stringToFp(it->second);
}

int fetchIntSetting(const std::map<std::string, std::string> &variables, const std::string &name) {
    auto it = variables.find(name);
    if (it == variables.end()) {
        std::cerr << "Variable '" << name << "' not found in .dat file." << std::endl;
        exit(1);
    }
    return std::stoi(it->second);
}

int fetchIntSettingOrDefault(
        const std::map<std::string, std::string> &variables, const std::string &name,
        int fallback, bool &settingPresent) {
    auto it = variables.find(name);
    if (it == variables.end()) {
        settingPresent = false;
        return fallback;
    }
    return std::stoi(it->second);
}

void loadCaseConfigFromFile(
        const std::string &caseFilename, std::string &scenarioName, std::string &shapeName,
        Real &tEnd, Real &dtWrite, DomainGeometry &geometry,
        Real &UI, Real &VI, Real &WI, Real &PI, Real &TI, Real &GX, Real &GY, Real &GZ,
        Real &Re, Real &Pr, Real &omg, Real &eps, int &itermax, Real &alpha, Real &beta, Real &dt, Real &tau,
        bool &useTemperature, Real &T_h, Real &T_c,
        int &imax, int &jmax, int &kmax, Real &dx, Real &dy, Real &dz) {
    Real &xLength = geometry.xLength, &yLength = geometry.yLength, &zLength = geometry.zLength;
    Real &xOrigin = geometry.xOrigin, &yOrigin = geometry.yOrigin, &zOrigin = geometry.zOrigin;

    std::map<std::string, std::string> variables = readSettingsFile(caseFilename);

    scenarioName = fetchStringSetting(variables, "scenario");
    shapeName = fetchStringSetting(variables, "geometry");

    tEnd = fetchRealSetting(variables, "tEnd");
    dtWrite = fetchRealSetting(variables, "dtWrite");

    xLength = fetchRealSetting(variables, "xLength");
    yLength = fetchRealSetting(variables, "yLength");
    zLength = fetchRealSetting(variables, "zLength");
    xOrigin = fetchRealSetting(variables, "xOrigin");
    yOrigin = fetchRealSetting(variables, "yOrigin");
    zOrigin = fetchRealSetting(variables, "zOrigin");

    UI = fetchRealSetting(variables, "UI");
    VI = fetchRealSetting(variables, "VI");
    WI = fetchRealSetting(variables, "WI");
    PI = fetchRealSetting(variables, "PI");
    TI = fetchRealSettingOrDefault(variables, "TI", 0.0, useTemperature);
    GX = fetchRealSetting(variables, "GX");
    GY = fetchRealSetting(variables, "GY");
    GZ = fetchRealSetting(variables, "GZ");

    Re = fetchRealSetting(variables, "Re");
    Pr = fetchRealSettingOrDefault(variables, "Pr", 0.0, useTemperature);
    omg = fetchRealSetting(variables, "omg");
    eps = fetchRealSetting(variables, "eps");
    itermax = fetchIntSetting(variables, "itermax");
    alpha = fetchRealSetting(variables, "alpha");
    beta = fetchRealSettingOrDefault(variables, "beta", 0.0, useTemperature);
    dt = fetchRealSetting(variables, "dt");
    tau = fetchRealSetting(variables, "tau");

    T_c = fetchRealSettingOrDefault(variables, "T_c", 0.0, useTemperature);
    T_h = fetchRealSettingOrDefault(variables, "T_h", 0.0, useTemperature);

    imax = fetchIntSetting(variables, "imax");
    jmax = fetchIntSetting(variables, "jmax");
    kmax = fetchIntSetting(variables, "kmax");
    dx = xLength / Real(imax);
    dy = yLength / Real(jmax);
    dz = zLength / Real(kmax);
}
