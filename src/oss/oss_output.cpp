#include "../../include/oss/oss_output.hpp"


OSS::Output::Output(std::string operation_log_file_name): operation_log_file_name_(operation_log_file_name) {

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


const Color::ColorBuilder OSS::Output::debugConsoleTemplate(const LogLevel log_level, const Color::Colors border_color,const std::string topic,const std::string message, const OSSClock *oss_clock) const {
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
    
    if (oss_clock != nullptr) {
        Color::label(cb, "time");
        Color::tab(cb);
        cb.appendForeground(Color::Colors::DEFAULT, currentTimeToString(oss_clock));
        Color::newLine(cb);
    }
    
    Color::label(cb, "message");
    Color::newLine(cb);
    cb.appendForeground(Color::Colors::DEFAULT, message);
    return cb;
}
const std::string OSS::Output::debugLogTemplate(const LogLevel log_level,const std::string topic, const std::string message, const OSSClock *oss_clock) const {
    std::string output = "";
    std::string space = " ";
    std::string tab = "\t";
    std::string new_line = "\n";

    output += logLevelToString(log_level);
    output += tab + "topic: " + topic;
    output += tab +  "time:" + space + currentTimeToString(oss_clock);
    output += tab + "message:" + space + message;
    output += new_line;
}
void OSS::Output::logDebugINFO(const std::string topic, const std::string message) {
    Color::ColorBuilder cb;
    std::string sb;

    cb.appendForeground(Color::Colors::DEFAULT, Color::border());
    Color::newLine(cb);
    cb.appendForeground(Color::Colors::YELLOW, "topic:\t");
    cb.appendForeground(Color::Colors::CYAN, topic);
    Color::newLine(cb);
    cb.appendForeground(Color::Colors::YELLOW, "message:\n");
    cb.appendForeground(Color::Colors::DEFAULT, message);
    cb.appendForeground(Color::Colors::DEFAULT, Color::border());

} 