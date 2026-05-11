#pragma once
#include <queue>
#include "../logger/logger.hpp"
#include "pcb.hpp"


namespace OSS {
    struct MemoryResult {
        bool page_fault = false;
        bool granted = false;
        int frame = -1;
        int page = -1;
    };
    class MemoryManager {
        private:
            Logger *logger_;

            

            struct Frame {
                bool occupied = false;
                bool dirty = false;
                pid_t pid = -1;
                int page = -1;
            };

            const int FRAME_COUNT = 64;

            Frame frame_table_[64];
            std::queue<int> fifo_queue_;


            int findFreeFrame();


        public:
            explicit MemoryManager(Logger *logger);
            int accessMemory(PCB &pcv);
            void printMemoryLayout();
            
    };
}