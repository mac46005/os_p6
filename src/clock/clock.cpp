#include "../../include/clock/clock.hpp"

Clock::Clock(std::string parent, std::string key): key_(key), parent_(parent) {
    std::string name_of_process = "Clock::Clock()";
    shm_key_ = ftok(key.c_str(), 1);
    if(shm_key_ == -1) {
    }
}