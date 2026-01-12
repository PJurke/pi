#ifndef SCOPED_LOGGER_H
#define SCOPED_LOGGER_H

#include "Logger.h"
#include <string>
#include <chrono>
#include <iomanip>

class ScopedLogger {
public:
    ScopedLogger(const std::string& name, const std::string& file, int line) 
        : scopeName(name), file(file), line(line) {
        
        // If indentation is 0 *before* we increase it, we are at root level
        isRoot = (Logger::getInstance().getIndentation() == 0);
        
        Logger::getInstance().registerScopeStart(scopeName);
        startTime = std::chrono::steady_clock::now();
        Logger::getInstance().log(LogLevel::INFO, "[START] " + scopeName, file, line);
        Logger::getInstance().increaseIndent();
    }

    ~ScopedLogger() {
        Logger::getInstance().decreaseIndent();
        auto endTime = std::chrono::steady_clock::now();
        double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        std::stringstream msg;
        msg << "[DONE ] " << scopeName << " (" << std::fixed << std::setprecision(3) << durationMs << " ms)";
        
        Logger::getInstance().log(LogLevel::INFO, msg.str(), file, line);
        Logger::getInstance().recordScopeDuration(scopeName, durationMs, isRoot);
    }

private:
    std::string scopeName;
    std::string file;
    int line;
    std::chrono::steady_clock::time_point startTime;
    bool isRoot;
};

#define LOG_SCOPE(name) ScopedLogger scopedLogger(name, __FILE__, __LINE__)

#endif // SCOPED_LOGGER_H
