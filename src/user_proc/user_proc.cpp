#include "../../include/user_proc/user_proc.hpp";
#include <iostream>

UserProcess::UserProcess::UserProcess(int argc, char **argv) {
    pid_ = getpid();
    ppid_ = getppid();

    user_clock_manager_ = new UserClockManager("UserProcess", "./src/oss/oss.cpp", std::stoi(argv[1]), std::stoi(argv[2]));
    msg_manager_ = new MsgManager("msgq.txt", 0644, pid_);
}

int UserProcess::UserProcess::run() {
    MsgBuffer recieve{};
    while(true) {
        msg_manager_->receiveMessage(
            [this, &recieve](MsgBuffer msg) {
                recieve = msg;
            },
            0
        );
        std::cout << "UserProcess " + std::to_string(pid_) + " recieved message\n";


        if (user_clock_manager_->isTimeUp()) {
            std::cout << "UserProcess " + std::to_string(pid_) + "sending TERMINATION status to oss";
            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);

            break;
        } else {
            std::cout << "UserProcess " + std::to_string(pid_) + " sending OSS_CONTROL status to oss";
            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::OSS_CONTROL, -1, 0);
        }
    }
    std::cout << "UserProcess " + std::to_string(pid_) + " terminating...";
    cleanUp();
    return EXIT_SUCCESS;
}

void UserProcess::UserProcess::cleanUp() {
    user_clock_manager_->cleanUp();
    delete user_clock_manager_;
}