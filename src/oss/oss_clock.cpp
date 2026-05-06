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

Time OSS::OSSClock::generateRandomTimeFromBoundTimeLimit(const Time bound_time) {
    thread_local std::mt19937 rng{block_seed_};
    std::uniform_int_distribution<int> sec_dist(0, bound_time.sec);

    Time random_time;
    random_time.sec = sec_dist(rng);
    std::uniform_int_distribution<int> nano_dist(0, bound_time.nano);
    random_time.nano = nano_dist(rng);
    block_seed_++;

    return random_time;
}




void OSS::OSSClock::updateClockByQuantum() {
    Time *global_time = clock_->getCurrentTime();
    Clock::addTimeToPtrTime(global_time, quantum_time_);
}

Time *OSS::OSSClock::getCurrentTime() const {
    return clock_->getCurrentTime();
}

Time OSS::OSSClock::getChildTimeLimit() {
    Time random_time = generateRandomTimeFromBoundTimeLimit(child_time_limit_);
    return random_time;
}
bool OSS::OSSClock::checkIfLaunchIntervalReached() {
    Time *current_time = getCurrentTime();
    bool reached = Clock::gteq(*current_time, child_launch_time_);
    is_launch_interval_time_reached_ = reached;
    return is_launch_interval_time_reached_;
}


void OSS::OSSClock::setNewLaunchInterval() {
    Time *current_time = clock_->getCurrentTime();
    Time new_launch_time{0,0};
    Time random_time = generateRandomTimeFromBoundTimeLimit(child_launch_time_limit_);
    new_launch_time = Clock::addTime(random_time, *current_time);
    if (new_launch_time.nano >= 1000000000) {
        new_launch_time.sec++;
        new_launch_time.nano -= 1000000000;
    }
    child_launch_time_ = new_launch_time;
    is_launch_interval_time_reached_ = false;
}











const std::string OSS::OSSClock::toString() const {
    return clock_->toString();
}

void OSS::OSSClock::cleanUp() {
    clock_->detach();
    clock_->rmid();
}