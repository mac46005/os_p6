#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include "../color/ui.hpp"
#include "log_level.hpp"

class Logger
{
private:
    std::string debug_log_file_name_ = "debug.log";
    
    const std::string logLevelToString(LogLevel log_level);
    const Color::Colors logLevelToColors(LogLevel log_level);
    const void outputDebugLogTemplate(const LogLevel log_level, std::string topic, std::string message);
protected:
    std::ofstream debug_log_stream_;
    std::string pidToString(pid_t pid);
    std::string pidTab(pid_t pid, bool colored);
public:
    
    void initializeDebugFile();
    void closeDebugFile();
    void logDebugINFO(const std::string topic, const std::string message);
    void logDebugCAUTION(const std::string topic, const std::string message);
    void logDebugWARNING(const std::string topic, const std::string message);
    void setFileName(std::string name);
    void log(const std::string content);
};