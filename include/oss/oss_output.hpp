#pragma once
#include "../logger/logger.hpp"
#include "argument_error.hpp"
#include "../error/error.hpp"
#include "../color/ui.hpp"
#include "pcb_queue.hpp"


namespace OSS {
    class OssOutput : public Logger
    {
    private:
        enum LogLevel
        {
            INFO,
            CAUTION,
            WARNING
        };
        std::string operation_log_file_name_;
        std::ofstream operation_log_stream_;

        int operation_log_line_cout_ = 0;
        const int MAX_OPERATION_LOG_LINES = 10000;
        // const std::string currentTimeToString(const OSSClock *oss_clock) const;
        const std::string repeatStr(const int n, const std::string content) const;
        void writeToOperationLog(const std::string &line);

        void appendOption(
            Color::ColorBuilder &cb,
            std::string option,
            std::string arg
        );
        void appendOptionInfo(
            Color::ColorBuilder &cb, 
            std::string option, 
            std::string arg, 
            std::string description
        );
    public:
        void printHelpMessage();
        explicit OssOutput(std::string operation_log_file_name_);

        // void logProcessLaunch(pid_t pid, OSSClock *clock);

        void cleanUp();
    };
}