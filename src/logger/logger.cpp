#include "../../include/logger/logger.hpp"

void Logger::initializeDebugFile() {
    int fd = open(debug_log_file_name_.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        throw std::runtime_error("Logger::initlializeDebugFile()\t failed to open file: " + debug_log_file_name_);
    }
    close(fd);
    debug_log_stream_.open(debug_log_file_name_, std::ios::out | std::ios::app);
}
void Logger::closeDebugFile() {
    debug_log_stream_.close();
}
const std::string Logger::logLevelToString(LogLevel log_level) {
    switch (log_level) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::CAUTION:
            return "CAUTION";
        case LogLevel::WARNING:
            return "WARNING";
    }
    return "";
}

const Color::Colors Logger::logLevelToColors(LogLevel log_level) {
    switch (log_level) {
        case LogLevel::INFO:
            return Color::Colors::DEFAULT;
        case LogLevel::CAUTION:
            return Color::Colors::YELLOW;
        case LogLevel::WARNING:
            return Color::Colors::RED;
    }
    return Color::Colors::DEFAULT;
}

const void Logger::outputDebugLogTemplate(const LogLevel log_level, std::string topic, std::string message) {
    pid_t pid = getpid();
    Color::ColorBuilder cb;
    cb.appendForeground(logLevelToColors(log_level), logLevelToString(log_level));
    Color::tab(cb);
    cb.appendForeground(Color::Colors::YELLOW, "PID ");
    cb.appendForeground(Color::Colors::CYAN, pidToString(pid));
    Color::tab(cb);
    cb.appendForeground(Color::Colors::CYAN, topic);
    Color::tab(cb);
    cb.appendForeground(Color::Colors::DEFAULT, message);
    std::cout << cb.build() << std::endl;

    std::string log = logLevelToString(log_level) + " " + pidTab(pid,false) + "\t" + topic + "\t" + message;
    debug_log_stream_ <<  log << std::endl;
    debug_log_stream_.flush();
}
std::string Logger::pidToString(pid_t pid) {
    std::string pid_str = std::to_string(pid);
    return pid_str;
}
std::string Logger::pidTab(pid_t pid, bool colored) {
    std::string tab = "";

    if (colored) {
        Color::ColorBuilder cb;
        cb.appendForeground(Color::Colors::YELLOW, "PID");
        Color::space(cb);
        cb.appendForeground(Color::Colors::CYAN, pidToString(pid));
        tab = cb.build();
    } else {
        tab = "PID " + pidToString(pid);
    }

    return tab;
    
}
void Logger::logDebugINFO(const std::string topic, const std::string message) {
    outputDebugLogTemplate(LogLevel::INFO, topic, message);
}

void Logger::logDebugCAUTION(const std::string topic, const std::string message) {
    outputDebugLogTemplate(LogLevel::CAUTION, topic, message);
}

void Logger::logDebugWARNING(const std::string topic, const std::string message) {
    outputDebugLogTemplate(LogLevel::WARNING, topic, message);
}

