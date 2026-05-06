#pragma once
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <functional>
#include "../error/error.hpp"
#include "msgbuffer.hpp"

class MsgManager {
    private:
        int permission_ = 0;
        int msqid_;
        key_t key_;
        pid_t pid_;
    public:
        explicit MsgManager(const char *key, int permission, pid_t pid);
        // POSSIBLE CHANGE
        void sendMessage(long mtype, pid_t sender_pid, ProcessStatus status, int resource, int message_flag);
        void receiveMessage(std::function<void(MsgBuffer)> message, int message_flag);
        void cleanUp();
};