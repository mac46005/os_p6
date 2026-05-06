#include "../../include/user_proc/user_proc.hpp"
UserProcess::UserProcess *userProcess;
int main(int argc, char **argv) {
    userProcess = new UserProcess::UserProcess(argc, argv);
    int result = userProcess->run();
    return result;
}