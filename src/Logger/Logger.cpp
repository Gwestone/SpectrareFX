#include "Logger.h"

const std::string Logger::asciiPatternStart = "\033[1;";
const std::string Logger::asciiPatternEnd = "\033[0m";

void Logger::printWarn(const std::string &message) const {
    if (currentLevel >= LoggingLevels::WARN) {
        time_t now = time(0);
        tm *formated_time = gmtime(&now);
        std::cout << asciiPatternStart << MESSAGE_COLOR_CODES::YELLOW << "m" << "[WARN]" << asciiPatternEnd << "[" << formated_time->tm_hour << ":" << formated_time->tm_min << ":" << formated_time->tm_sec << "]:" << message << std::endl;
    }
}

void Logger::printError(const std::string &message) const {

    if (currentLevel >= LoggingLevels::ERROR){
        time_t now = time(0);
        tm *formated_time = gmtime(&now);
        std::cout << asciiPatternStart << MESSAGE_COLOR_CODES::RED << "m" << "[ERROR]" << asciiPatternEnd << "[" << formated_time->tm_hour << ":" << formated_time->tm_min << ":" << formated_time->tm_sec << "]:" << message << std::endl;
    }
}

void Logger::printInfo(const std::string &message) const {
    if (currentLevel >= LoggingLevels::INFO) {
        time_t now = time(0);
        tm *formated_time = gmtime(&now);
        std::cout << asciiPatternStart << MESSAGE_COLOR_CODES::CYAN << "m" << "[INFO]" << asciiPatternEnd << "[" << formated_time->tm_hour << ":" << formated_time->tm_min << ":" << formated_time->tm_sec << "]:" << message << std::endl;
    }
}

void Logger::setLoggingLevel(LoggingLevels level) {
    currentLevel = level;
}
