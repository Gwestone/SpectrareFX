#include <stdexcept>
#include <fstream>
#include "FileHelper.h"

std::vector<char> FileHelper::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    //get size of file and create buffer for data
    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    //move to front
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    //close the file
    file.close();
    return buffer;
}
