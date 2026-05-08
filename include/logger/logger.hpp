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
    const std::string debug_log_file_name_ = "debug.log";
    std::ofstream debug_log_stream_;
    const std::string logLevelToString(LogLevel log_level);
    const Color::Colors logLevelToColors(LogLevel log_level);
    const void outputDebugLogTemplate(const LogLevel log_level, std::string topic, std::string message);
public:
    void initializeDebugFile();
    void closeDebugFile();
    void logDebugINFO(const std::string topic, const std::string message);
    void logDebugCAUTION(const std::string topic, const std::string message);
    void logDebugWARNING(const std::string topic, const std::string message);
};