#pragma once
#include <sys/types.h>

enum ProcessStatus {
    TERMINATE,
    REQUEST,
    RELEASE,
    OSS_CONTROL,
    GRANTED,
    DENIED
};

struct MsgBuffer {
    long mtype;
    pid_t sender_pid;
    int resource;
    int time_slice_nano;
    ProcessStatus status;
};