#include "../../include/user_proc/user_proc.hpp";

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


        if (user_clock_manager_->isTimeUp()) {
            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);
            break;
        }
    }

    cleanUp();
    return EXIT_SUCCESS;
}

void UserProcess::UserProcess::cleanUp() {
    user_clock_manager_->cleanUp();
    delete user_clock_manager_;
}