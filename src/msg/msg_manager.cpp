#include "../../include/msg/msg_manager.hpp"

MsgManager::MsgManager(const char *key, int permission, pid_t pid, Logger *logger): permission_(permission), pid_(pid), logger_(logger) {
    logger_->logDebugINFO("PID" + std::to_string(pid_), "Initialized MsgManager()");
    if ((key_ = ftok(key, 1)) == -1) {
        throw Error("MsgManager", "MsgManager()", "Failed to ftok()", std::strerror(errno));
        // throw std::runtime_error(std::string("MsgManager FAILED to ftok() in MsgManager::MsgManager()\n") + std::string(std::strerror(errno)));
    }

    if ((msqid_ = msgget(key_, permission_)) == -1) {
        throw Error("MsgManager", "MsgManager()", "Failed to msgget()", std::strerror(errno));
        // throw std::runtime_error(std::string("MsgManager FAILED to msgget() in MsgManger::MsgManager()\n") + std::string(std::strerror(errno)));
    }
}

void MsgManager::sendMessage(long mtype, pid_t sender_pid, ProcessStatus status, int resource, int message_flag) {
    MsgBuffer buf{};

    buf.mtype = mtype;
    buf.sender_pid = sender_pid;
    buf.status = status;
    buf.resource = resource;

    logger_->logDebugWARNING("PID " + std::to_string(pid_) + " MsgManager::sendMessage()", "Attempting to to msgsnd()");

    if ((msgsnd(msqid_, &buf, sizeof(MsgBuffer) - sizeof(long), message_flag)) == -1) {
        throw Error("MsgManager", "sendMessage()", "Failed to msgsnd()", std::strerror(errno));
        // throw std::runtime_error(std::string("MsgManager FAILED to msgsnd() in MsgManager::sendMessage()\n") + std::string(std::strerror(errno)));
    }
    logger_->logDebugWARNING("PID " + std::to_string(pid_) + " MsgManager::sendMessage()", "Message successfully sent to PID " + buf.mtype);

}

void MsgManager::recieveMessage(std::function<void(MsgBuffer)> message, int message_flag) {
    MsgBuffer buf;
    logger_->logDebugWARNING("PID " + std::to_string(pid_) + " MsgManager::recieveMessage()", "Attempting to to msgrcv()");

    if ((msgrcv(msqid_, &buf, sizeof(MsgBuffer) - sizeof(long), pid_, message_flag)) == -1) {
        throw Error("MsgManager", "recieveMessage()", "Failed to msgrcv()", std::strerror(errno));
        // throw std::runtime_error(std::string("MsgManager FAILED to msgrcv() in MsgManager::recieveMessage()\n") + std::string(std::strerror(errno)));
    }
    logger_->logDebugWARNING("PID " + std::to_string(pid_) + " MsgManager::recieveMessage()", "Message recieved from PID " + buf.sender_pid);

    message(buf);
}

// bool MsgManager::canRecieveMessage(std::function<void(MsgBuffer)> message) {
//     MsgBuffer buf{};
//     ssize_t n = msgrcv(msqid_, &buf, sizeof(MsgBuffer) - sizeof(long), pid_, IPC_NOWAIT);

//     if (n == -1) {
//         if (errno == ENOMSG) {
//             return false;
//         }
//         throw Error("MsgManager", "canRecieveMessage()", "Failed to msgrcv()", std::strerror(errno));
//         // throw std::runtime_error(std::string("MsgManager FAILED to msgrcv() in MsgManager::canRecieveMessage()\n") + std::string(std::strerror(errno)));
//     }
// }

void MsgManager::cleanUp() {
    logger_->logDebugINFO("PID " + std::to_string(pid_) + " MsgManager::cleanUp()", "Cleaning up data");
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

