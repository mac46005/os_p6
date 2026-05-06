#include "../../include/user_proc/user_clock_manager.hpp"

UserProcess::UserClockManager::UserClockManager(std::string parent, std::string key, int sec, int nano): parent_(parent), key_(key) {
    endTime_.sec = sec;
    endTime_.nano = nano;
    clock = new Clock(parent, key);
}

bool UserProcess::UserClockManager::isTimeUp() {
    Time *currentTime = clock->getCurrentTime();
    if ((currentTime->sec > endTime_.sec) || currentTime->sec == endTime_.sec && currentTime->nano > endTime_.nano) {
        return true;
    }
    return false;
}

void UserProcess::UserClockManager::cleanUp() {
    clock->detach();
    delete clock;
}
