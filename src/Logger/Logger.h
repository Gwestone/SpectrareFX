#pragma once

#include <string>
#include <iostream>
#include <chrono>

enum LoggingLevels{
    ERROR = 0, WARN = 1, INFO = 2
};

class Logger {
private:
    enum MESSAGE_COLOR_CODES{
        RESET = 0, RED = 31, YELLOW = 33, CYAN = 36
    };
    LoggingLevels currentLevel = LoggingLevels::INFO;
public:
    void printInfo(const std::string& message) const;
    void printWarn(const std::string& message) const;
    void printError(const std::string& message) const;
    void setLoggingLevel(LoggingLevels level);
    static const std::string asciiPatternStart;
    static const std::string asciiPatternEnd;
};