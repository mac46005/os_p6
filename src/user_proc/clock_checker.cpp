#include "../../include/user_proc/clock_checker.hpp"

ClockChecker::ClockChecker(int max_sec, int max_nano, Logger *logger): logger_(logger) {
    end_time_.sec = max_sec;
    end_time_.nano = max_nano;
    clock_ = new Clock("User Process", "./src/oss/oss.cpp", logger_);
}

bool ClockChecker::isTimeUp() {
    Time current_time = *clock_->getCurrentTime();
    if (
        (current_time.sec > end_time_.sec)
        ||
        (current_time.sec == end_time_.sec && current_time.nano >= end_time_.nano)
    ) {
        return true;
    } else {
        return false;
    }
}

void ClockChecker::cleanUp() {
    clock_->detach();
}

std::string ClockChecker::toString() {
    return clock_->toString();
}