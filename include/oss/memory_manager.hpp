#pragma once
#include <queue>
#include "../logger/logger.hpp"
#include "pcb.hpp"
#include "../msg/msgbuffer.hpp"

namespace OSS {
    struct MemoryResult {
        bool page_fault = false;
        bool granted = false;
        int frame = -1;
        int page = -1;
    };
    struct MemoryStats {
        int memory_references = 0;
        int reads = 0;
        int writes = 0;
        int page_faults = 0;
        int page_hits = 0;
        int replacement = 0;
        int dirty_replacement = 0;
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
            MemoryStats stats_{};



            int findFreeFrame();
            int getReplacementFrame();
            void invalidateVictimPage(PCB &current_pcb, int victim_frame);

        public:
            explicit MemoryManager(Logger *logger);
            MemoryResult accessMemory(PCB &pcb, MsgBuffer msg);
            void freeProcessFrames(pid_t pid);
            void printMemoryLayout();
            void printStats();
            MemoryStats getStats() const;
            
    };
}