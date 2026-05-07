#include "../../include/oss/oss_output.hpp"


OSS::Output::Output(std::string operation_log_file_name): operation_log_file_name_(operation_log_file_name) {
    int fd = open(operation_log_file_name.c_str(), O_CREAT | O_WRONLY | O_TRUNC | 0644);
    if (fd == -1) {
        throw std::runtime_error("OSS::Output::Output() failed to open file: " + operation_log_file_name);
    }

    close(fd);

    fd = open(debug_log_file_name_.c_str(), O_CREAT | O_WRONLY | O_TRUNC | 0644);
    if (fd == -1) {
        throw std::runtime_error("OSS::Output::Output() failed to open file: " + debug_log_file_name_);
    }

    close(fd);

    operation_log_stream_.open(operation_log_file_name, std::ios::out | std::ios::app);
    debug_log_stream_.open(debug_log_file_name_, std::ios::out | std::ios::app);

}

void OSS::Output::appendOption(
    Color::ColorBuilder &cb,
    std::string option,
    std::string arg)
{
    Color::optionalContainerStart(cb);
    Color::space(cb);
    Color::option(cb, option);
    Color::space(cb);
    Color::optionArgument(cb, arg);
    Color::space(cb);
    Color::optionalContainerEnd(cb);
}
void OSS::Output::appendOptionInfo(Color::ColorBuilder &cb, std::string option, std::string arg, std::string description)
{
    appendOption(cb, option, arg);
    Color::newLine(cb);
    Color::tab(cb);
    Color::defaultOutput(cb, description);
    Color::newLine(cb);
}
void OSS::Output::printHelpMessage()
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
    Color::defaultOutput(
        cb,
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

const std::string OSS::Output::currentTimeToString(const OSSClock *oss_clock) const {
    return oss_clock->toString();
}



const std::string OSS::Output::repeatStr(const int n, const std::string content) const {
    std::string output = "";
    for (int i = 0; i < n; i++) {
        output += content;
    }
    return output;
}


const std::string OSS::Output::logLevelToString(LogLevel log_level) const {
    switch (log_level)
    {
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::ERROR:
        return "ERROR";
    }
}


const Color::ColorBuilder OSS::Output::debugConsoleTemplate(const LogLevel log_level, const Color::Colors border_color,const std::string topic,const std::string message) const {
    Color::ColorBuilder cb;
    cb.appendForeground(border_color, Color::border());
    Color::newLine(cb);
    cb.appendForeground(border_color, Color::border());
    Color::newLine(cb);
    cb.appendForeground(border_color, repeatStr(8, std::string(logLevelToString(log_level))));
    Color::newLine(cb);
    Color::label(cb, "topic");
    Color::tab(cb);
    cb.appendForeground(Color::Colors::CYAN, topic);
    Color::newLine(cb); 
    Color::label(cb, "message");
    Color::newLine(cb);
    cb.appendForeground(Color::Colors::DEFAULT, message);
    return cb;
}


const std::string OSS::Output::debugLogTemplate(const LogLevel log_level,const std::string topic, const std::string message) const {
    std::string output = "";
    std::string space = " ";
    std::string tab = "\t";
    std::string new_line = "\n";

    output += logLevelToString(log_level);
    output += tab + "topic: " + topic;
    output += tab + "message:" + space + message;
    output += new_line;
}


void OSS::Output::logDebugINFO(const std::string topic, const std::string message) {
    Color::ColorBuilder cb;
    std::string sb;
    LogLevel lvl = LogLevel::INFO;
    cb = debugConsoleTemplate(lvl, Color::Colors::DEFAULT, topic, message);
    sb = debugLogTemplate(lvl, topic, message);

    std::cout << cb.build();
    debug_log_stream_ << sb << std::endl;
}


void OSS::Output::logDebugWARNING(const std::string topic, const std::string message) {
    Color::ColorBuilder cb;
    std::string sb;
    LogLevel lvl = LogLevel::WARNING;
    cb = debugConsoleTemplate(lvl, Color::Colors::YELLOW, topic, message);
    sb = debugLogTemplate(lvl, topic, message);

    std::cout << cb.build();
    debug_log_stream_ << sb << std::endl;
}


void OSS::Output::logDebugERROR(const std::string topic, const std::string message) {
    Color::ColorBuilder cb;
    std::string sb;
    LogLevel lvl = LogLevel::ERROR;
    cb = debugConsoleTemplate(lvl, Color::Colors::RED, topic, message);
    sb = debugLogTemplate(lvl, topic, message);

    std::cout << cb.build();
    debug_log_stream_ << sb << std::endl;
}





void OSS::Output::writeToOperationLog(const std::string &line) {
    std::cout << line << std::endl;
    if (operation_log_stream_.is_open() && operation_log_line_cout_ < MAX_OPERATION_LOG_LINES) {
        operation_log_stream_ << line << std::endl;
        operation_log_line_cout_++;
    }
}






void OSS::Output::logProcessLaunch(pid_t pid, OSSClock *clock) {
    writeToOperationLog("OSS launched process pid " + std::to_string(pid) + " at time " + clock->toString());
}



void OSS::Output::cleanUp() {
    debug_log_stream_.close();
    operation_log_stream_.close();
}