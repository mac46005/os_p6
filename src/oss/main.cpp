#include "../../include/oss/oss.hpp";

OSS::OSS *oss;

int main(int argc, char **argv) {
    oss = new OSS::OSS(argc, argv);
    int result = oss->run();
    return result;
}