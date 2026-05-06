#pragma once
#include "../clock/clock.hpp"
#include <random>
namespace OSS {
    class OSSClock {
        private:
            int block_seed_ = 4600;
            Clock *clock_;
            Time oss_work_time_{0,0};
            Time child_time_limit_{0,0};
            Time child_launch_time_limit_{0,0};
            Time child_launch_time_{0,0};
            Time quantum_time_{0,0};
            bool is_launch_interval_time_reached_ = false;
            Time generateRandomTimeFromBoundTimeLimit(const Time bound_time);
        public:
            explicit OSSClock(
                std::string key,
                float child_time_limit,
                float child_launch_limit,
                int time_quantum_sec,
                int time_quantum_nano
            );

            void updateClockByQuantum();
            Time *getCurrentTime() const;
            Time getChildTimeLimit();
            bool checkIfLaunchIntervalReached();
            void setNewLaunchInterval();
            const std::string toString() const;
            void cleanUp();

    };
}