#include <iostream>
#include <cstdlib>
#include "IO/text_file_utils.hpp"

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#error Missing the <filesystem> header.
#endif

bool hasPrefix(const std::string& str, const std::string& prefix) {
    return prefix.length() <= str.length() && std::equal(prefix.begin(), prefix.end(), str.begin());
}

bool hasSuffix(const std::string& str, const std::string& postfix) {
    return postfix.length() <= str.length() && std::equal(postfix.rbegin(), postfix.rend(), str.rbegin());
}

void setupOutputDirectory(
        const std::string &resultDirectory, const std::string &curveDirectory, const std::string &shapeDirectory) {
    if (!fs::exists(resultDirectory)) {
        if (!fs::create_directory(resultDirectory)) {
            std::cerr << "Output directory could not be created." << std::endl;
            exit(1);
        }
    }
    if (!fs::exists(curveDirectory)) {
        if (!fs::create_directory(curveDirectory)) {
            std::cerr << "Line directory could not be created." << std::endl;
            exit(1);
        }
    }
    if (!fs::exists(shapeDirectory)) {
        if (!fs::create_directory(shapeDirectory)) {
            std::cerr << "Geometry directory could not be created." << std::endl;
            exit(1);
        }
    }
}

void clearStaleOutputFiles(
        const std::string &resultDirectory, const std::string &outputBaseName, const std::string &fileExtension) {
    const std::string outputBaseDot = outputBaseName + ".";
    fs::path dir(resultDirectory);
    fs::directory_iterator end;
    for (fs::directory_iterator it(dir); it != end; ++it) {
        std::string filename = it->path().string();
#ifdef WIN32
        for (std::string::iterator it = filename.begin(); it != filename.end(); ++it) {
            if (*it == '\\') *it = '/';
        }
#endif
        if (hasSuffix(filename, fileExtension) && hasPrefix(filename, outputBaseDot)) {
            fs::remove(it->path());
        }
    }
}

bool fileIsPresent(const std::string& filename) {
    return fs::exists(filename) && !fs::is_directory(filename);
}
