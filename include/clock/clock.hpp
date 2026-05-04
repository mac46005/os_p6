#pragma once
#include "time.hpp"
#include <string>
#include <sys/shm.h>
#include "../error/error.hpp"

class Clock {
private:
    std::string parent_ = "";
    static const int SECOND_TO_NANO = 1000000000;
    const int BUFF_SZ = sizeof(Time);
    
    std::string key_ = "";
    int shm_key_ = 0;
    int shm_id_ = -1;
    Time *current_time_ = nullptr;

public:
    explicit Clock(std::string parent, std::string key);
    void initializeClock();
    Time *getCurrentTime();
    static void addTimeToPtrTime(Time *ptr_time, Time add_time);
    static void subtractTimeToPtrTime(Time *ptr_tim, Time subtract_time);
    static Time floatToTime(float time_f);
    void detach();
    void rmid();

    std::string toString();
};