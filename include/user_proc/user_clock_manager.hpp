#include "../clock/clock.hpp"
#include <string>

namespace UserProcess {
    class UserClockManager {
        private:
            std::string parent_ = "";
            std::string key_ = "";
            Time endTime_ = {0,0};
            Clock *clock;
        public:
            explicit UserClockManager(std::string parent, std::string key, int sec, int nano);
            bool isTimeUp();
            void cleanUp();
    };
}