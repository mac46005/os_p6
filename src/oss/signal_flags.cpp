#include "../../include/oss/signal_flags.hpp"

volatile sig_atomic_t g_timeout = 0;
volatile sig_atomic_t g_stop = 0;
void signalHandler(int sig) {
    if (sig == SIGALRM) {
        g_timeout = 1;
    } else if (sig == SIGINT || sig == SIGTERM) {
        g_stop = 1;
    }
}