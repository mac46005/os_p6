#pragma once
#include <sys/types.h>
enum ProcessStatus {
    TERMINATE,
    MEMORY_REQUEST,
    OSS_CONTROL,
    GRANTED,
    DENIED
};
enum AccessType {
    READ,
    WRITE,
    NONE
};
struct MsgBuffer{
    long mtype;
    pid_t sender_pid;
    
    ProcessStatus status;

    int address;
    int page;
    int offset;
    AccessType access;
};