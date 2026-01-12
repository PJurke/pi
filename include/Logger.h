#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>

enum class LogLevel {

    /// @brief Extremely detailed tracing info
    TRACE,

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

    void increaseIndent();
    void decreaseIndent();
    int getIndentation() const;

    void recordScopeDuration(const std::string& scopeName, double durationMs, bool isRoot);
    void registerScopeStart(const std::string& scopeName);
    void printPerformanceSummary();

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logFile;
    LogLevel currentLogLevel;
    std::mutex logMutex;

    // Profiling data
    struct ScopeStats {
        double totalDurationMs = 0.0;
        int count = 0;
        bool isRoot = false;
    };
    std::map<std::string, ScopeStats> scopeStatsMap;
    std::vector<std::string> insertionOrder;
    std::chrono::steady_clock::time_point appStartTime;
    
    // We will use thread_local in the implementation, but we can expose helper methods here.
    // No member variable needed for thread_local indentation as it is static/global per thread.

    std::string getCurrentTimestamp();
    std::string getLogLevelString(LogLevel level);
    std::string getColorCode(LogLevel level);
    void writeLog(LogLevel level, const std::string& message, const std::string& file, int line);

};

#define LOG_TRACE(message) Logger::getInstance().log(LogLevel::TRACE, message, __FILE__, __LINE__)
#define LOG_DEBUG(message) Logger::getInstance().log(LogLevel::DEBUG, message, __FILE__, __LINE__)
#define LOG_INFO(message) Logger::getInstance().log(LogLevel::INFO, message, __FILE__, __LINE__)
#define LOG_WARNING(message) Logger::getInstance().log(LogLevel::WARNING, message, __FILE__, __LINE__)
#define LOG_ERROR(message) Logger::getInstance().log(LogLevel::ERROR, message, __FILE__, __LINE__)
#define LOG_CRITICAL(message) Logger::getInstance().log(LogLevel::CRITICAL, message, __FILE__, __LINE__)

#endif