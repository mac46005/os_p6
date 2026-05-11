#include "../../include/oss/oss_output.hpp"


OSS::OssOutput::OssOutput(std::string operation_log_file_name): operation_log_file_name_(operation_log_file_name) {
    int fd = open(operation_log_file_name.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        throw std::runtime_error("OSS::Output::Output() failed to open file: " + operation_log_file_name);
    }

    close(fd);

    operation_log_stream_.open(operation_log_file_name, std::ios::out | std::ios::app);

}

void OSS::OssOutput::appendOption(
    Color::ColorBuilder &cb,
    std::string option,
    std::string arg
){
    Color::optionalContainerStart(cb);
    Color::space(cb);
    Color::option(cb, option);
    Color::space(cb);
    Color::optionArgument(cb, arg);
    Color::space(cb);
    Color::optionalContainerEnd(cb);
}
void OSS::OssOutput::appendOptionInfo(Color::ColorBuilder &cb, std::string option, std::string arg, std::string description)
{
    appendOption(cb, option, arg);
    Color::newLine(cb);
    Color::tab(cb);
    Color::defaultOutput(cb, description);
    Color::newLine(cb);
}
void OSS::OssOutput::printHelpMessage()
{
    Color::ColorBuilder cb;

    Color::title(cb, "Project 5: The Resource Manager");
    Color::newLine(cb);
    Color::label(cb, "USAGE");
    Color::space(cb);
    Color::title(cb, "oss");
    Color::space(cb);
    appendOption(cb, "n", "proc");
    Color::space(cb);
    appendOption(cb, "s", "simul");
    Color::space(cb);
    appendOption(cb, "t", "childTimeLimit");
    Color::space(cb);
    appendOption(cb, "i", "launchInterval");
    Color::space(cb);
    appendOption(cb, "f", "logfile");
    Color::newLine(cb);

    Color::label(cb, "PURPOSE");
    cb.appendForeground(Color::Colors::DEFAULT, 
        "This program simulates an operating system that launches several worker processes.\n"
        "The simulated oss manages and schedules workers to work a certain time limit and also manage resources.\n"
        "All workers can decide how to utilize \"working\" time and resources.");
    Color::newLine(cb);

    Color::label(cb, "ARGUMENTS");
    Color::newLine(cb);
    appendOptionInfo(cb, "n", "proc", "Max processes to launch");
    appendOptionInfo(cb, "s", "simul", "Max simultaneous processes to launch");
    appendOptionInfo(cb, "t", "childTimeLimit", "Total bound time limit for children to execute for");
    appendOptionInfo(cb, "i", "launchInterval", "Total bound time launch interval");
    appendOptionInfo(cb, "f", "logfile", "The name of the log file to output oss operations");

    std::cout << cb.build();
}

// const std::string OSS::OssOutput::currentTimeToString(const OSSClock *oss_clock) const {
//     return oss_clock->toString();
// }



const std::string OSS::OssOutput::repeatStr(const int n, const std::string content) const {
    std::string output = "";
    for (int i = 0; i < n; i++) {
        output += content;
    }
    return output;
}



void OSS::OssOutput::writeToOperationLog(const std::string &line) {
    std::cout << line << std::endl;
    if (operation_log_stream_.is_open() && operation_log_line_cout_ < MAX_OPERATION_LOG_LINES) {
        operation_log_stream_ << line << std::endl;
        operation_log_line_cout_++;
    }
}
std::string OSS::OssOutput::timeToString(Time time) {
    std::string str = std::to_string(time.sec) + ":" + std::to_string(time.nano);
    return str;
}

std::string OSS::OssOutput::infoTab(std::string title, std::string content, bool colored) {
    std::string tab = "";
    if (colored) {
        Color::ColorBuilder cb;
        cb.appendForeground(Color::Colors::YELLOW, title + ": ");
        Color::space(cb);
        cb.appendForeground(Color::Colors::DEFAULT, content);
        tab = cb.build();

    } else {
        tab = title + " " + content;
    }

    return tab;
}

std::string OSS::OssOutput::pcbToString(PCB pcb, bool colored) {
    std::string str = "";

    if (colored) {
        Color::ColorBuilder cb;
        cb.append(pidTab(pcb.pid, true));
        Color::tab(cb);
        cb.append(infoTab("started at", timeToString(pcb.start_time), true));
        Color::tab(cb);
        cb.append(infoTab("ended at", timeToString(pcb.end_time), true));
        str = cb.build();
    } else {
        str = pidTab(pcb.pid, false) + "\t" + infoTab("started", timeToString(pcb.start_time), false) + "\t" + infoTab("ended", timeToString(pcb.end_time), false);
    }
    return str;
}


void OSS::OssOutput::printCompletedTable(std::vector<PCB> completed_list) {
    Color::ColorBuilder cb;
    std::string sb = "";

    std::string title = "## Completed Process list ###################################################################";

    debug_log_stream_ << title << std::endl;
    cb.appendForeground(Color::Colors::GREEN, title);

    Color::newLine(cb);
    for (auto p : completed_list) {
        cb.append(pcbToString(p, true));
        Color::newLine(cb);

        debug_log_stream_ << pcbToString(p, false) << std::endl;
    }

    std::cout << cb.build() << std::endl;
    
    
}
// void OSS::OssOutput::logProcessLaunch(pid_t pid, OSSClock *clock) {
//     writeToOperationLog("OSS launched process pid " + std::to_string(pid) + " at time " + clock->toString());
// }



void OSS::OssOutput::cleanUp() {
    closeDebugFile();
    operation_log_stream_.close();
}