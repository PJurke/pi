#include "../include/Logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : currentLogLevel(LogLevel::INFO) {}

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

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream stringStream;

    stringStream << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return stringStream.str();
}

std::string Logger::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

void Logger::writeLog(LogLevel level, const std::string& message,  const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::stringstream logEntry;
    logEntry << getCurrentTimestamp() 
        << " [" << getLogLevelString(level) << "] "
        << file << ":" << line << " - "
        << message;

    // Console output
    if (level >= currentLogLevel) {
        std::cerr << logEntry.str() << std::endl;
    }

    // File logging if enabled
    if (logFile.is_open()) {
        logFile << logEntry.str() << std::endl;
    }
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    writeLog(level, message, file, line);
}