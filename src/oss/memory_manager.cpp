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


OSS::MemoryResult OSS::MemoryManager::accessMemory(PCB &pcb, MsgBuffer) {
    MemoryResult result{};
    result.page = msg.page;

    if (msg.page < 0 || msg.page >= PAGE_COUNT) {
        logger_->logDebugWARNING(
            "MemoryManager::accessMemory",
            "Invalid page request from PID " + std::to_string(pcb.pid) + " page " + std::to_string(msg.page));
        )
    }
}