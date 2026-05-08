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
struct MsgBuffer{
    long mtype;
    pid_t sender_pid;
    // char str_data[100];
    int resource;
    int time_slice_nano;
    ProcessStatus status;
    // int is_done;
};