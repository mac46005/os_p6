#include "../../include/oss/memory_manager.hpp"

OSS::MemoryManager::MemoryManager(Logger *logger): logger_(logger) {
    for (int i = 0; i < FRAME_COUNT; i++) {
        frame_table_[i].occupied = false;
        frame_table_[i].dirty = false;
        frame_table_[i].pid = -1;
        frame_table_[i].page = -1;
    }
}

int OSS::MemoryManager::findFreeFrame() {
    for (int i =0; i < FRAME_COUNT; i++) {
        if (!frame_table_[i].occupied) {
            return i;
        }
    }
}


OSS::MemoryResult OSS::MemoryManager::accessMemory(PCB &pcb, MsgBuffer msg){
    MemoryResult result{};
    result.page = msg.page;

    if (msg.page < 0 || msg.page >= PAGE_COUNT) {
        logger_->logDebugWARNING(
            "MemoryManager::accessMemory()",
            "Invali page request from PID " + std::to_string(pcb.pid) + " page " + std::to_string(msg.page)
        );

        result.granted = false;
        return result;
    }

    pcb.memory_references++;

    PageTableEntry &entry = pcb.page_table[msg.page];

    if (entry.valid) {
        result.page_fault = false;
        result.granted = true;
        result.frame = entry.frame;

        if (msg.access == AccessType::WRITE) {
            frame_table_[entry.frame].dirty = true;
        }

        logger_->logDebugINFO(
            "MemoryManager::accessMemory()",
            "PAGE HIT: PID " + std::to_string(msg.pid) + 
            " page " + std::to_string(msg.page) +
            " in frame " + std::to_string(entry.frame)
        );

        return result;

    }

    pcb.page_faults++;
    result.page_fault = true;

    int free_frame = findFreeFrame();

    if (free_frame == -1) {
        logger_->logDebugWARNING(
            "MemoryManager::accessMemory()",
            "PAGE FAULT: no free frames available yet. FIFO needed next"
        );

        result.granted = false;
        result.frame = -1;
        return result;
    }

    frame_table_[free_frame].occupied = true;
    frame_table_[free_frame].dirty = (msg.access == AccessType::WRITE);
    frame_table_[free_frame].pid = pcb.pid;
    frame_table_[free_frame].page = msg.page;

    entry.valid = true;
    entry.frame = free_frame;

    logger_->logDebugWARNING(
        "MemoryManager::accessMemory()", 
        "PAGE FAULT: PID " + std::to_string(pcb.pid) +
        " page " + std::to_string(msg.page) + 
        " loaded into frame " + std::to_string(free_frame)
    );

    return result;
}

void OSS::MemoryManager::freeProcessFrames(pid_t pid) {
    for (int i = 0; i < FRAME_COUNT; i++) {
        if (frame_table_[i].occupied && frame_table_[i].pid == pid) {
            frame_table_[i].occupied = false;
            frame_table_[i].dirty = false;
            frame_table_[i].pid = -1;
            frame_table_[i].page = -1;
        }
    }

    logger_->logDebugINFO(
        "MemoryManager::freeProcessFrames()",
        "Freed frames for PID " + std::to_string(pid)
    );
}