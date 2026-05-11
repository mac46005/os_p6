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


OSS::Mem