#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum class LogLevel {

    /// @brief Detailed info for diagnosing issues
    DEBUG,

    /// @brief General operational messages
    INFO,

    /// @brief Indications of potential problems
    WARNING,

    /// @brief Errors that affect functionality
    ERROR,

    /// @brief Severe issues that may halt the system
    CRITICAL
};

class Logger {
public:
    static Logger& getInstance();

    void log(LogLevel level, const std::string& message, const std::string& file = "", int line = 0);

    void setLogLevel(LogLevel level);
    void enableFileLogging(const std::string& filename);
    void disableFileLogging();

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logFile;
    LogLevel currentLogLevel;
    std::mutex logMutex;

    std::string getCurrentTimestamp();
    std::string getLogLevelString(LogLevel level);
    void writeLog(LogLevel level, const std::string& message, const std::string& file, int line);

};

#define LOG_DEBUG(message) Logger::getInstance().log(LogLevel::DEBUG, message, __FILE__, __LINE__)
#define LOG_INFO(message) Logger::getInstance().log(LogLevel::INFO, message, __FILE__, __LINE__)
#define LOG_WARNING(message) Logger::getInstance().log(LogLevel::WARNING, message, __FILE__, __LINE__)
#define LOG_ERROR(message) Logger::getInstance().log(LogLevel::ERROR, message, __FILE__, __LINE__)
#define LOG_CRITICAL(message) Logger::getInstance().log(LogLevel::CRITICAL, message, __FILE__, __LINE__)

#endif