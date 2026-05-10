#pragma once
#include <sys/types.h>
#include "../clock/time.hpp"



namespace OSS {
    const int PAGE_COUNT = 16;

    struct PageTableEntry {
        int frame = -1;
        bool valid = false;
    };

    struct PCB {
        int index = 0;
        pid_t pid = -1;
        Time start_time;
        Time end_time;

        PageTableEntry page_table[PAGE_COUNT];

        int memory_references = 0;
        int page_faults = 0;
    };
}