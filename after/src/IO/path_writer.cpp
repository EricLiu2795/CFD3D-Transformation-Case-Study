
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "IO/byte_streams.hpp"
#include "IO/path_writer.hpp"


const uint32_t PATH_FILE_VERSION = 1u;

bool writePathsToObj(const std::string &filename, const PathCollection &flowCurves) {
    std::ofstream file(filename.c_str(), std::ofstream::binary);
    if (!file.is_open()) {
        std::cerr << "Error in writeLinesToObjFile: File \"" + filename + "\" not found." << std::endl;
        return false;
    }

    file << std::setprecision(5);

    size_t vertexCounter = 1;

    size_t pathGroupIndex = 0;
    for (const auto &flowCurve : flowCurves) {
        size_t pathNodeCount = flowCurve.positions.size();
        if (pathNodeCount < 2) {
            continue;
        }

        for (size_t i = 0; i < pathNodeCount; i++) {
            const glm::vec3 &v = flowCurve.positions.at(i);
            file << "v " << std::setprecision(5) << v.x << " " << v.y << " " << v.z << "\n";
            file << "vt ";
            for (size_t fieldIndex = 0; fieldIndex < flowCurve.attributes.size(); fieldIndex++) {
                file << std::setprecision(5) << flowCurve.attributes.at(fieldIndex).at(i);
                if (fieldIndex != flowCurve.attributes.size()-1) {
                    file << " ";
                }
            }
            file << "\n";
        }

        file << "g line" << pathGroupIndex << "\n";
        file << "l ";
        for (size_t i = 1; i < pathNodeCount+1; i++) {
            file << vertexCounter << " ";
            vertexCounter++;
        }
        file << "\n\n";
        pathGroupIndex++;
    }

    file.close();
    return true;
}

bool writePathsToBinary(const std::string &filename, const PathCollection &flowCurves) {
    std::ofstream file(filename.c_str(), std::ofstream::binary);
    if (!file.is_open()) {
        std::cerr << "Error in writeLinesToBinLinesFile: File \"" + filename + "\" not found." << std::endl;
        return false;
    }

    uint32_t pathCount = uint32_t(flowCurves.size());
    uint32_t numAttributes = uint32_t(flowCurves.empty() ? 0 : flowCurves.at(0).attributes.size());

    ByteWriteStream stream;
    stream.write(uint32_t(PATH_FILE_VERSION));
    stream.write(uint32_t(pathCount));
    stream.write(uint32_t(numAttributes));

    for (uint32_t pathIndex = 0; pathIndex < pathCount; pathIndex++) {
        const FlowCurve &activePath = flowCurves.at(pathIndex);
        size_t pathPointCount = activePath.positions.size();
        stream.write(uint32_t(pathPointCount));
#ifdef FP_FLOAT
        stream.write(activePath.positions.data(), sizeof(glm::vec3) * pathPointCount);
#endif
#ifdef FP_DOUBLE
        std::vector<glm::vec3> floatPoints;
        floatPoints.resize(activePath.positions.size());
        std::transform(activePath.positions.begin(), activePath.positions.end(), floatPoints.begin(),
                [](const auto &position) { return glm::vec3(position); });
        stream.write((void*)&floatPoints.front(), sizeof(glm::vec3)*pathPointCount);
#endif
        assert(numAttributes == activePath.attributes.size());
        for (uint32_t fieldIndex = 0; fieldIndex < numAttributes; fieldIndex++) {
#ifdef FP_FLOAT
            const std::vector<float> &activeField = activePath.attributes.at(fieldIndex);
#endif
#ifdef FP_DOUBLE
            const std::vector<double> &activeFieldDouble = activePath.attributes.at(fieldIndex);
            std::vector<float> activeField;
            activeField.resize(activeFieldDouble.size());
            std::transform(activeFieldDouble.begin(), activeFieldDouble.end(), activeField.begin(),
                    [](double value) { return static_cast<float>(value); });
#endif
            assert(pathPointCount == activeField.size());
            stream.write(activeField.data(), sizeof(float) * pathPointCount);
        }
    }

    file.write((const char*)stream.getBuffer(), std::streamsize(stream.getSize()));
    if (!file) {
        std::cerr << "Error in writeLinesToBinLinesFile: Failed writing file \"" + filename + "\"." << std::endl;
        return false;
    }
    file.close();
    return true;
}
