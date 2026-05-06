#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include "../color/ui.hpp"
#include "oss_clock.hpp"



namespace OSS {
    class Output {
        private:
            enum LogLevel {
                INFO,
                WARNING,
                ERROR
            };
            std::string operation_log_file_name_;
            const std::string debug_log_file_name_ = "debug_log.log";
            std::ofstream operation_log_;
            std::ofstream debug_log_;
            
            int operation_log_line_cout_ = 0;
            const int MAX_OPERATION_LOG_LINES = 10000;
            const std::string currentTimeToString(const OSSClock *oss_clock) const;
            const std::string repeatStr(const int n, const std::string content) const;
            const std::string logLevelToString(LogLevel log_level) const;
            const Color::ColorBuilder debugConsoleTemplate(const LogLevel log_level, const Color::Colors border_color, const std::string topic, const std::string message, const OSSClock *oss_clock) const;
            const std::string debugLogTemplate(const LogLevel log_level, const std::string topic, const std::string message, const OSSClock *oss_clock) const;
        public:
            explicit Output(std::string operation_log_file_name_);

            void logDebugINFO(const std::string topic, const std::string message);
            void logDebugWARNING( const std::string topic, const std::string message);
            void logDebugERROR( const std::string topic, const std::string message);

    };
}