#pragma once
#include <string>
#include <vector>

class FileHelper {
public:
    static std::vector<char> readFile(const std::string& filename);
};