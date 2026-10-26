
#include <fstream>
#include <iostream>
#include "IO/byte_streams.hpp"
#include "IO/geometry_io.hpp"

const uint32_t GEOMETRY_FORMAT_REVISION = 1u;

void saveGeometryData(
        const std::string &shapeFilename, int width, int height, int depth, const std::vector<uint32_t> &values) {
    std::ofstream file(shapeFilename.c_str(), std::ofstream::binary);
    if (!file.is_open()) {
        std::cerr << "Error in saveGeometryData: File \"" + shapeFilename + "\" not found." << std::endl;
        return;
    }
    assert(values.size() == size_t(width) * size_t(height) * size_t(depth));

    ByteWriteStream stream;
    stream.write((uint32_t)GEOMETRY_FORMAT_REVISION);
    stream.write((uint32_t)width);
    stream.write((uint32_t)height);
    stream.write((uint32_t)depth);
    stream.write((void*)&values.front(), sizeof(uint32_t)*values.size());

    file.write((const char*)stream.getBuffer(), std::streamsize(stream.getSize()));
    assert(file.good());
    file.close();
}

std::vector<uint32_t> loadGeometryData(
        const std::string &shapeFilename, int &width, int &height, int &depth) {
    std::ifstream file(shapeFilename.c_str(), std::ifstream::binary);
    if (!file.is_open()) {
        std::cerr << "Error in loadGeometryData: File \"" + shapeFilename + "\" not found." << std::endl;
        return {};
    }

    file.seekg(0, std::ifstream::end);
    size_t size = file.tellg();
    file.seekg(0);
    std::vector<char> buffer(size);
    file.read(buffer.data(), std::streamsize(size));
    if (file.gcount() != std::streamsize(size)) {
        std::cerr << "Error in loadGeometryData: Truncated file \"" << shapeFilename << "\"." << std::endl;
        return {};
    }
    file.close();

    ByteReadStream stream(static_cast<const void*>(buffer.data()), size);
    uint32_t version;
    stream.read(version);
    if (version != GEOMETRY_FORMAT_REVISION) {
        std::cerr << "Error in readMesh3D: Invalid version in file \"" << shapeFilename << "\"." << std::endl;
        return {};
    }

    uint32_t rawWidth, rawHeight, rawDepth;
    stream.read(rawWidth);
    stream.read(rawHeight);
    stream.read(rawDepth);
    width = static_cast<int>(rawWidth);
    height = static_cast<int>(rawHeight);
    depth = static_cast<int>(rawDepth);

    std::vector<uint32_t> values;
    values.resize(rawWidth*rawHeight*rawDepth);
    stream.read((void*)&values.front(), sizeof(uint32_t)*values.size());
    return values;
}
