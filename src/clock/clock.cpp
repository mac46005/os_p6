#include "../../include/clock/clock.hpp"
Clock::Clock(std::string parent, std::string key): key_(key), parent_(parent) {
    std::string name_of_process = "Clock::Clock()";
    shm_key_ = ftok(key.c_str(), 1);
    if(shm_key_ == -1) {
        throw Error(parent_, name_of_process, "Failed to ftok()", strerror(errno));
    }

    shm_id_ = shmget(shm_key_, BUFF_SZ, IPC_CREAT | 0700);
    if (shm_id_ == -1) {
        throw Error(parent_, name_of_process, "Failed to shmget()", strerror(errno));
    }

    current_time_ = (Time *)shmat(shm_id_, 0, 0);
    if (current_time_ == (Time *)-1) {
        throw Error(parent_, name_of_process, "Failed to shmat()", strerror(errno));
    }
}

void Clock::initializeClock() {
    current_time_->sec = 0;
    current_time_->nano = 0;
}

Time *Clock::getCurrentTime() {
    return current_time_;
}

void Clock::roundTime(Time &time) {
    if (time.nano >= Clock::SECOND_TO_NANO) {
        time.sec++;
        time.nano -= Clock::SECOND_TO_NANO;
    } else if (time.nano < 0) {
        if (time.nano < (-Clock::SECOND_TO_NANO)) {
            time.nano += Clock::SECOND_TO_NANO;
            time.sec--;
        }
    }
}
bool Clock::gteq(Time a, Time b) {
    return (a.sec > b.sec) || (a.sec == b.sec && a.nano >= b.nano);
}
bool Clock::lteq(Time a, Time b) {
    return (b.sec > a.sec) || (b.sec == a.sec && b.nano >= a.nano);
}

Time Clock::addTime(Time a, Time b) {
    Time newTime = Time();
    newTime.sec = a.sec + b.sec;
    newTime.nano = a.nano + b.nano;
    roundTime(newTime);
    return newTime;
}
Time Clock::subtractTime(Time a, Time b) {
    Time newTime = Time();
    newTime.sec = a.sec - b.sec;
    newTime.nano = a.nano - b.nano;
    roundTime(newTime);
    return newTime;
}

void Clock::addTimeToPtrTime(Time *ptr_time, Time add_time) {
    ptr_time->sec = ptr_time->sec + add_time.sec;
    ptr_time->nano = ptr_time->nano + add_time.nano;

    if (ptr_time->nano >= Clock::SECOND_TO_NANO) {
        ptr_time->sec++;
        ptr_time->nano -= Clock::SECOND_TO_NANO;
    }
}

Time Clock::floatToTime(float time_f) {
    Time time{0,0};
    int sec = 0;
    if (time_f >= 1.0) {
        sec = static_cast<int>(time_f);
    }

    float nano_dec = time_f - sec;
    int nano = (nano_dec * Clock::SECOND_TO_NANO);
    time.sec = sec;
    time.nano  = nano;

    return time;
}

void Clock::detach() {
    shmdt(current_time_);
}

void Clock::rmid() {
    shmctl(shm_id_, IPC_RMID, NULL);
}

std::string Clock::toString() {
    std::string time_str = std::to_string(current_time_->sec) + ":" + std::to_string(current_time_->nano);
    return time_str;
}