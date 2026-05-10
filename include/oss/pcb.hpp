#pragma once
#include <sys/types.h>
#include "../clock/time.hpp"


namespace OSS {
    struct PCB {
        int index = 0;
        pid_t pid = -1;
        Time start_time;
        Time end_time;
        int resource_allocated[10];
        int requested_resource = -1;
    };
}