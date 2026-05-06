#include "../../include/oss/oss_clock.hpp"

OSS::OSSClock::OSSClock(
    std::string key,
    float child_time_limit,
    float child_launch_time,
    int time_quantum_sec,
    int time_quantum_nano
): quantum_time_(Time{time_quantum_sec, time_quantum_nano}) {
    clock_ = new Clock("OssClock", key);
    child_time_limit_ = Clock::floatToTime(child_time_limit);
    child_launch_time_limit_ = Clock::floatToTime(child_launch_time);
}

void OSS::OSSClock::updateClockByQuantum() {
    Time *global_time = clock_->getCurrentTime();
    Clock::addTimeToPtrTime(global_time, quantum_time_);
}

Time *OSS::OSSClock::getCurrentTime() const {
    return clock_->getCurrentTime();
}