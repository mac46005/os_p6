#include "../../include/oss/oss.hpp"
OSS::OSS *oss;

int main (int argc, char **argv) {
    signal(SIGALRM, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    // alarm(13);

    oss = new OSS::OSS(argc, argv);
    int exit_status = oss->run();
    return exit_status;
}