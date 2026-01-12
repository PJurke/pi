#include "../include/Logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : currentLogLevel(LogLevel::INFO) {
    appStartTime = std::chrono::steady_clock::now();
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void Logger::enableFileLogging(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile.is_open()) {
        logFile.close();
    }

    logFile.open(filename, std::ios::app);
}

void Logger::disableFileLogging() {
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile.is_open()) {
        logFile.close();
    }
}

// Thread-local indentation level
thread_local int g_indentationLevel = 0;

void Logger::increaseIndent() {
    g_indentationLevel++;
}

void Logger::decreaseIndent() {
    if (g_indentationLevel > 0) {
        g_indentationLevel--;
    }
}

int Logger::getIndentation() const {
    return g_indentationLevel;
}

void Logger::registerScopeStart(const std::string& scopeName) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    // If this is the first time we see this scope, add it to insertion order
    if (scopeStatsMap.find(scopeName) == scopeStatsMap.end()) {
        insertionOrder.push_back(scopeName);
        // Initialize stats entry so it exists
        scopeStatsMap[scopeName] = ScopeStats(); 
    }
}

void Logger::recordScopeDuration(const std::string& scopeName, double durationMs, bool isRoot) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    scopeStatsMap[scopeName].totalDurationMs += durationMs;
    scopeStatsMap[scopeName].count++;
    scopeStatsMap[scopeName].isRoot = isRoot;
}

void Logger::printPerformanceSummary() {
    std::lock_guard<std::mutex> lock(logMutex);
    
    auto now = std::chrono::steady_clock::now();
    double totalAppTimeMs = std::chrono::duration<double, std::milli>(now - appStartTime).count();

    // Calculate sum of root scopes
    double rootScopesTotalTime = 0.0;
    for (const auto& pair : scopeStatsMap) {
        if (pair.second.isRoot) {
            rootScopesTotalTime += pair.second.totalDurationMs;
        }
    }
    
    // Calculate overhead
    double overheadTime = totalAppTimeMs - rootScopesTotalTime;
    if (overheadTime < 0) overheadTime = 0; // Should not happen unless clock drift or parallel issues

    std::stringstream summary;
    summary << "\n================================================================================\n";
    summary << "                         PERFORMANCE SUMMARY                                    \n";
    summary << "--------------------------------------------------------------------------------\n";
    summary << " Total Application Time: " << std::fixed << std::setprecision(3) << totalAppTimeMs << " ms\n";
    summary << "--------------------------------------------------------------------------------\n";
    summary << std::left << std::setw(40) << " Phase / Scope" 
            << std::right << std::setw(12) << "Count" 
            << std::right << std::setw(15) << "Total Time" 
            << std::right << std::setw(10) << "% App" << "\n";
    summary << "--------------------------------------------------------------------------------\n";

    // Print tracked scopes in insertion order (ONLY ROOTS)
    for (const auto& name : insertionOrder) {
        if (scopeStatsMap.find(name) == scopeStatsMap.end()) continue;
        const ScopeStats& stats = scopeStatsMap.at(name);
        
        if (!stats.isRoot) continue; // Skip nested details for clearer summary

        double percent = (stats.totalDurationMs / totalAppTimeMs) * 100.0;

        summary << std::left << std::setw(40) << name 
                << std::right << std::setw(12) << stats.count 
                << std::right << std::setw(12) << std::fixed << std::setprecision(3) << stats.totalDurationMs << " ms" 
                << std::right << std::setw(9) << std::fixed << std::setprecision(2) << percent << "%\n";
    }

    // Print Overhead
    double overheadPercent = (overheadTime / totalAppTimeMs) * 100.0;
    summary << std::left << std::setw(40) << "Other / Overhead" 
            << std::right << std::setw(12) << "-" 
            << std::right << std::setw(12) << std::fixed << std::setprecision(3) << overheadTime << " ms" 
            << std::right << std::setw(9) << std::fixed << std::setprecision(2) << overheadPercent << "%\n";

    summary << "================================================================================\n";

    // Print to console
    std::cerr << summary.str();

    // Print to file if enabled
    if (logFile.is_open()) {
        logFile << summary.str();
    }
}

#include <iomanip>
#include <filesystem>

std::string Logger::getCurrentTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream stringStream;
    stringStream << std::put_time(std::localtime(&time), "%H:%M:%S");
    stringStream << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return stringStream.str();
}

std::string Logger::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:    return "TRACE";
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO ";
        case LogLevel::WARNING:  return "WARN ";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT ";
        default:                 return "UNKNO";
    }
}

std::string Logger::getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:    return "\033[90m";      // Gray
        case LogLevel::DEBUG:    return "\033[36m";      // Cyan
        case LogLevel::INFO:     return "\033[32m";      // Green
        case LogLevel::WARNING:  return "\033[33m";      // Yellow
        case LogLevel::ERROR:    return "\033[31m";      // Red
        case LogLevel::CRITICAL: return "\033[1;31m";    // Bold Red
        default:                 return "\033[0m";       // Reset
    }
}

void Logger::writeLog(LogLevel level, const std::string& message,  const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = getLogLevelString(level);

    // Extract filename from path
    std::string filename = std::filesystem::path(file).filename().string();
    std::string location = filename + ":" + std::to_string(line);

    // Create indentation string (2 spaces per level)
    std::string indentation(g_indentationLevel * 2, ' ');

    std::stringstream formattedEntry;
    formattedEntry << timestamp << " | "
                   << levelStr << " | "
                   << std::left << std::setw(28) << location << " | "
                   << indentation << message;

    // Console entry (with colors)
    if (level >= currentLogLevel) {
        std::string color = getColorCode(level);
        std::string reset = "\033[0m";
        
        std::cerr << color << formattedEntry.str() << reset << std::endl;
    }

    // File logging if enabled
    if (logFile.is_open()) {
        logFile << formattedEntry.str() << std::endl;
    }
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    writeLog(level, message, file, line);
}