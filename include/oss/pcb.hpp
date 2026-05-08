
#pragma once
#include <sys/types.h>

namespace OSS {
    struct PCB {
        int index = 0;
        pid_t pid = -1;
        int start_sec = 0;
        int start_nano = 0;
        int end_sec = 0;
        int end_nano = 0;
        int resource_allocated[10];
        int requested_resource = -1;
    };
}