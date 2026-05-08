#include "../../include/user_proc/user_proc.hpp"
#include <signal.h>


UserProcess *user_process;
void signalHandler(int sig) {
    if (sig == SIGALRM  || sig == SIGINT || sig == SIGTERM) {
        user_process->cleanUp();
    }
}
int main (int argc, char **argv) {
    user_process = new UserProcess(argc, argv);
    int exit_status = user_process->run();
    return exit_status;
}