#include "user_clock_manager.hpp"
#include "../msg/msg_manager.hpp"

namespace UserProcess {
    class UserProcess {
        private:
            pid_t pid_;
            pid_t ppid_;
            UserClockManager *user_clock_manager_;
            MsgManager *msg_manager_;
            void cleanUp();
        public:
            explicit UserProcess(int argc, char **argv);
            int run();

    };
}