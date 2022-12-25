#ifndef PARALLEL_FILEHELPER_H
#define PARALLEL_FILEHELPER_H

#include <string>
#include <vector>

class FileHelper {
public:
    static std::vector<char> readFile(const std::string& filename);
};

#endif //PARALLEL_FILEHELPER_H
