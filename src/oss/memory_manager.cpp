#include "../../include/oss/memory_manager.hpp"

OSS::MemoryManager::MemoryManager(Logger *logger): logger_(logger) {
    for (int i = 0; i < FRAME_COUNT; i++) {
        frame_table_[i] = Frame{};        
    }
}

int OSS::MemoryManager::findFreeFrame() {
    for (int i =0; i < FRAME_COUNT; i++) {
        if (!frame_table_[i].occupied) {
            return i;
        }
    }
    return -1;
}

int OSS::MemoryManager::getReplacementFrame() {
    while (!fifo_queue_.empty()) {
        int frame = fifo_queue_.front();
        fifo_queue_.pop();

        if (frame >= 0 && frame < FRAME_COUNT && frame_table_[frame].occupied) {
            return frame;
        }
    }
    return -1;
}

void OSS::MemoryManager::invalidateVictimPage(PCB &current_pcb, int victim_frame) {
    pid_t victim_pid = frame_table_[victim_frame].pid;
    int victim_page = frame_table_[victim_frame].page;

    if (victim_pid == current_pcb.pid && victim_page >= 0 && victim_page < PAGE_COUNT) {
        current_pcb.page_table[victim_page].valid = false;
        current_pcb.page_table[victim_page].frame = -1;
    }
}


OSS::MemoryResult OSS::MemoryManager::accessMemory(PCB &pcb, MsgBuffer msg){
   MemoryResult result{};
   result.page = msg.page;

   if (msg.page < 0 || msg.page >= PAGE_COUNT) {
        logger_->logDebugWARNING(
            "MemoryManager::accessMemory()",
            "DENIED: PID " + std::to_string(pcb.pid) +
            " requested invalid page " + std::to_string(msg.page)
        );

        result.granted = false;
        return result;
   }

   stats_.memory_references++;
   pcb.memory_references++;

   if (msg.access == AccessType::READ) {
    stats_.reads++;
   } else if (msg.access == AccessType::WRITE) {
    stats_.writes++;
   }

   PageTableEntry &entry = pcb.page_table[msg.page];

   if (entry.valid) {
    int frame = entry.frame;

    if (frame >= 0 && frame < FRAME_COUNT && frame_table_[frame].occupied) {
        if (msg.access == AccessType::WRITE) {
            frame_table_[frame].dirty = true;
        }

        stats_.page_hits++;

        logger_->logDebugINFO(
            "MemoryManager::accessMemory()",
            "PAGE HIT: PID " + std::to_string(pcb.pid) +
            " page " + std::to_string(msg.page) +
            " in frame " + std::to_string(frame)
        );

        result.granted = true;
        result.page_fault = false;
        return result;
    }
    entry.valid = false;
    entry.frame = -1;
   }

   stats_.page_faults++;
   pcb.page_faults++;

   int frame = findFreeFrame();

   if (frame == -1) {
    frame = getReplacementFrame();

    if (frame == 01) {
        logger_->logDebugWARNING(
            "MemoryManager::accessMemory()",
            "DENIED: no frame available for PID " + std::to_string(pcb.pid)
        );

        result.granted = false;
        return result;
    }

    stats_.replacement++;

    if (frame_table_[frame].dirty) {
        stats_.dirty_replacement++;
    }

    logger_->logDebugWARNING(
        "MemoryManagement::accessMemory()",
        "FIFO REPLACE: frame " + std::to_string(frame) +
        " old PID " + std::to_string(frame_table_[frame].pid) + 
        " old PAGE " + std::to_string(frame_table_[frame].page) +
        " dirty " + std::to_string(frame_table_[frame].dirty)
    );

    invalidateVictimPage(pcb, frame);
   }

   frame_table_[frame].occupied = true;
   frame_table_[frame].dirty = (msg.access == AccessType::WRITE);
   frame_table_[frame].pid = pcb.pid;
   frame_table_[frame].page = msg.page;

   entry.valid = true;
   entry.frame = frame;

   fifo_queue_.push(frame);

   logger_->logDebugWARNING(
    "MemoryManager::accessMemory()",
    "PAGE FAULT: PID " + std::to_string(pcb.pid) +
    " page " + std::to_string(msg.page) +
    " loaded into frame " + std::to_string(frame)
   );

   result.granted = true;
   result.page_fault = true;
   return result;
}

void OSS::MemoryManager::freeProcessFrames(pid_t pid) {
    for (int i = 0; i < FRAME_COUNT; i++) {
        if (frame_table_[i].occupied && frame_table_[i].pid == pid) {
            logger_->logDebugINFO(
                "MemoryManager::freeProcessFrames()",
                "Freeing frame " + std::to_string(i) +
                " from PID " + std::to_string(pid) +
                " page " + std::to_string(frame_table_[i].page)
            );

            frame_table_[i] = Frame{};
        }


    }
}

void OSS::MemoryManager::printMemoryLayout() {
    logger_->logDebugINFO(
        "MemoryManager::printMemoryLayout()",
        "Frame table snapshot"
    );

    for (int i = 0; i < FRAME_COUNT; i++) {
        if (frame_table_[i].occupied) {
            logger_->logDebugINFO(
                "MemoryManager::printMemoryLayout()",
                "Frame " + std::to_string(i) +
                " PID " + std::to_string(frame_table_[i].pid) +
                " PAGE " + std::to_string(frame_table_[i].page) +
                " DIRTY " + std::to_string(frame_table_[i].dirty)
            );
        }
    }
}


void OSS::MemoryManager::printStats() {
    double fault_percent = 0.0;

    if (stats_.memory_references > 0) {
        fault_percent = static_cast<double>(stats_.page_faults) / static_cast<double>(stats_.memory_references) * 100.0;
    }

    logger_->logDebugINFO("MemoryManager::printStats()", "===== Memory Statistics =====");
    logger_->logDebugINFO("MemoryManager::printStats()", "Memory references: " + std::to_string(stats_.memory_references));
    logger_->logDebugINFO("MemoryManager::printStats()", "Reads: " + std::to_string(stats_.reads));
    logger_->logDebugINFO("MemoryManager::printStats()", "Writes: " + std::to_string(stats_.writes));
    logger_->logDebugINFO("MemoryManager::printStats()", "Page hits: " + std::to_string(stats_.page_hits));
    logger_->logDebugINFO("MemoryManager::printStats()", "Page faults: " + std::to_string(stats_.page_faults));
    // logger_->logDebugINFO("MemoryManager::printStats()", "Replacements: " + std::to_string(stats_.replacements));
    // logger_->logDebugINFO("MemoryManager::printStats()", "Dirty replacements: " + std::to_string(stats_.dirty_replacements));
    logger_->logDebugINFO("MemoryManager::printStats()", "Page fault rate: " + std::to_string(fault_percent) + "%");
}

OSS::MemoryStats OSS::MemoryManager::getStats() const {
    return stats_;
}