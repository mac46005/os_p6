#include "clock_checker.hpp"
#include "argument_processor.hpp"
#include "unistd.h"
#include "../color/ui.hpp"
#include "../msg/msg_manager.hpp"
#include "../logger/logger.hpp"

class UserProcess {
private:
    pid_t pid_;
    pid_t ppid_;
    
    Logger *logger_;
    ArgumentProcessor *argument_processor_;
    ClockChecker *clock_checker_;
    MsgManager *msg_manager_;
public:
    explicit UserProcess(int argc, char **argv);

    int run();
    void cleanUp();
};