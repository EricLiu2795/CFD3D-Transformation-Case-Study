#ifndef FLUID3D_TEXT_FILE_UTILS_HPP
#define FLUID3D_TEXT_FILE_UTILS_HPP

#include <string>

bool hasPrefix(const std::string& str, const std::string& prefix);

bool hasSuffix(const std::string& str, const std::string& postfix);

void setupOutputDirectory(
        const std::string &resultDirectory, const std::string &curveDirectory, const std::string &shapeDirectory);

void clearStaleOutputFiles(
        const std::string &resultDirectory, const std::string &outputBaseName, const std::string &fileExtension);

bool fileIsPresent(const std::string& filename);

template<class InputIterator>
void tokenizeBySpace(const std::string &inputText, InputIterator& outputList) {
    std::string buffer;
    for (char c : inputText) {
        if (c != ' ' && c != '\t') {
            buffer += c;
        } else {
            if (buffer.length() > 0) {
                outputList.push_back(buffer);
                buffer = "";
            }
        }
    }
    if (buffer.length() > 0) {
        outputList.push_back(buffer);
        buffer = "";
    }
}

#endif
