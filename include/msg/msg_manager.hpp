#pragma once
#include <string.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdexcept>
#include <functional>
#include <cerrno>
#include <cstring>
#include "msgbuffer.hpp"
#include "../error/error.hpp"
#include "../logger/logger.hpp"

class MsgManager{
private:
    int permission_ = 0;
    int msqid_;
    key_t key_;
    pid_t pid_;

    Logger *logger_;
public:
    explicit MsgManager(const char *key, int permission, pid_t pid, Logger *logger);
    void sendMessage(long mtype, pid_t sender_pid,ProcessStatus status,int resource, int messsage_flag);
    void recieveMessage(std::function<void(MsgBuffer)> message, int message_flag);
    // bool canRecieveMessage(std::function<void(MsgBuffer)> handler);
    void cleanUp();
};