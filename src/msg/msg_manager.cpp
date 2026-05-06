#include "../../include/msg/msg_manager.hpp"

MsgManager::MsgManager(const char *key, int permission, pid_t pid): permission_(permission), pid_(pid) {
    if ((key_ = ftok(key, 1)) == -1) {
        throw Error("MsgManager", "MsgManager()", "Failed to ftok()", std::strerror(errno));
    }

    if ((msqid_ = msgget(key_, permission_)) == -1) {
        throw Error("MsgManager", "MsgManager()", "Failed to msgget()", std::strerror(errno));
    }
}

void MsgManager::sendMessage(long mtype, pid_t sender_pid, ProcessStatus status, int resource, int message_flag) {
    MsgBuffer buf{};

    buf.mtype = mtype;
    buf.sender_pid = sender_pid;
    buf.status = status;
    buf.resource = resource;

    if ((msgsnd(msqid_, &buf, sizeof(MsgBuffer) - sizeof(long), message_flag)) == -1) {
        throw Error("MsgMnaager", "sendMessage()", "Failed to msgsnd()", std::strerror(errno));
    }
}

void MsgManager::receiveMessage(std::function<void(MsgBuffer)> message, int message_flag) {
    MsgBuffer buf;

    if ((msgrcv(msqid_, &buf, sizeof(MsgBuffer) - sizeof(long), pid_, message_flag)) == -1) {
        throw Error("MsgManager", "recieveMessage()", "Failed to msgrcv()", std::strerror(errno));
    }

    message(buf);
}

void MsgManager::cleanUp() {
    if (msqid_ == -1) {
        return;
    }

    MsgBuffer tmp{};
    errno = 0;
    while (msgrcv(msqid_, &tmp, sizeof(MsgBuffer) - sizeof(long), 0, IPC_NOWAIT) != -1) {
        
    }

    if (errno != ENOMSG) {
        throw Error("MsgManager", "cleanUp()", "Failed to drain messages", std::strerror(errno));
    }

    if (msgctl(msqid_, IPC_RMID, nullptr) == -1) {
        throw Error("MsgManager", "cleanUp()", "Failed to msgctl()", std::strerror(errno));
    }

    msqid_ = -1;
}