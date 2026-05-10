#pragma once
#include "../logger/logger.hpp"
#include "argument_error.hpp"
#include "../error/error.hpp"
#include "../color/ui.hpp"
#include "pcb_queue.hpp"
#include <vector>
#include "pcb.hpp"
#include "../clock/time.hpp"

namespace OSS {
    class OssOutput : public Logger
    {
    private:
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

        std::string infoTab(std::string title, std::string content, bool colored);
        std::string timeToString(Time time);

    public:
        void printHelpMessage();
        explicit OssOutput(std::string operation_log_file_name_);

        // void logProcessLaunch(pid_t pid, OSSClock *clock);
        void printCompletedTable(std::vector<PCB> completed_list);
        std::string pcbToString(PCB pcb, bool colored);
        void cleanUp();
    };
}