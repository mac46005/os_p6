#include "../../include/oss/oss_clock.hpp"

OSS::OSSClock::OSSClock(
    std::string key,
    float child_time_limit,
    float child_launch_limit,
    int time_quantum_sec,
    int time_quantum_nano,
    Logger *logger
) : time_quantum_(Time{time_quantum_sec, time_quantum_nano}), logger_(logger)
{
    clock_ = new Clock("OSS", key, logger_);
    clock_->initClock();
    child_time_limit_ = Clock::floatToTime(child_time_limit);
    child_launch_time_limit_ = Clock::floatToTime(child_launch_limit);
}

void OSS::OSSClock::updateClockByQuantum()
{
    Time *global_time = clock_->getCurrentTime();
    Clock::addTimeToPtrTime(global_time, time_quantum_);
    // logger_->logDebugINFO("OSSClock::updateClockByQuantum()", "Time updated to " + clock_->toString());
}

// DO I NEED THIS?
void OSS::OSSClock::updateOssTimeBy(Time time)
{
}
///

Time OSS::OSSClock::getChildTimeLimit()
{
    Time random_time = generateRandomTimeFromBoundTimeLimit(child_time_limit_);
    return random_time;
}

Time OSS::OSSClock::getCurrentTime()
{
    Time current_time = *clock_->getCurrentTime();
    return current_time;
}


Time OSS::OSSClock::generateRandomTimeFromBoundTimeLimit(Time bound_time) {
    thread_local std::mt19937 rng{blocked_seed_};
    std::uniform_int_distribution<int> sec_dist(0, bound_time.sec);
    
    
    
    Time random_time;
    random_time.sec = sec_dist(rng);
    std::uniform_int_distribution<int> nano_dist(0, bound_time.nano);
    random_time.nano = nano_dist(rng);
    blocked_seed_++;

    return random_time;
}









void OSS::OSSClock::checkIfLaunchIntervalReached() {
    Time current_time_ = getCurrentTime();

    bool reached = ((current_time_.sec > child_launch_time_.sec) || (current_time_.sec == child_launch_time_.sec && current_time_.nano >= child_launch_time_.nano));
    is_launch_interval_time_reached_ = reached;
    // logger_->logDebugWARNING("OSSClock::checkIfLaunchIntervalReached()", "Did launch interval reached: " + std::to_string(reached));
}
void OSS::OSSClock::setNewLaunchInterval()
{
    Time *current_time = clock_->getCurrentTime();
    Time new_launch_time{0, 0};
    Time random_time = generateRandomTimeFromBoundTimeLimit(child_launch_time_limit_);
    new_launch_time.sec = random_time.sec + current_time->sec;
    new_launch_time.nano = random_time.nano + current_time->nano;
    // wow
    if (new_launch_time.nano >= 1000000000) {
        new_launch_time.sec++;
        new_launch_time.nano -= 1000000000;
    }
    child_launch_time_ = new_launch_time;
    is_launch_interval_time_reached_ = false;
}
bool OSS::OSSClock::launchIntervalReached() {
    checkIfLaunchIntervalReached();
    return is_launch_interval_time_reached_;
}






void OSS::OSSClock::cleanUp() {
    clock_->detach();
    clock_->rmid();
}









