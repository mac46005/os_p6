#include "../../include/oss/scheduler.hpp"

OSS::Scheduler::Scheduler(
    int max_proc,
    int max_simul,
    Output *output,
    OSSClock *oss_clock,
    MsgManager *msg_manager
): oss_clock_(oss_clock), output_(output), msg_manager_(msg_manager){
    pcb_info_.max_process_count_ = max_proc;
    pcb_info_.max_simultaneous_count_ = max_simul;
    pcb_ready_queue_ = new PCBQueue();
}

OSS::PCB OSS::Scheduler::createPCB(pid_t pid) {
    Time current_time = *oss_clock_->getCurrentTime();
    OSS::PCB pcb {
        .pid = pid,
        .start_time = current_time
    };
    return pcb;
}

void OSS::Scheduler::forkProcess() {
    Time current_time = *oss_clock_->getCurrentTime();
    Time child_time_limit_ = oss_clock_->getChildTimeLimit();
    Clock::addTimeToPtrTime(&child_time_limit_, current_time);

    std::string child_runtime_sec_str = std::to_string(child_time_limit_.sec);
    std::string child_runtime_nano_str = std::to_string(child_time_limit_.nano);

    char *args[] = {
        const_cast<char *>("./user_proc"),
        const_cast<char *>(child_runtime_sec_str.c_str()),
        const_cast<char *>(child_runtime_nano_str.c_str()),
        nullptr
    };

    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("excvp failed");
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        pcb_info_.pcb_count_++;
        pcb_info_.process_count_++;
        PCB pcb = createPCB(pid);
        pcb_ready_queue_->enqueue(pcb);

        if (is_running_linear_process_) {
            linear_process_pid_ = pid;
        }

        output_->logDebugINFO("Scheduler::forkProcess()", std::string("OSS forking process pid: ") + std::string(std::to_string(pid)));
        output_->logProcessLaunch(pid, oss_clock_);
    }
}

bool OSS::Scheduler::stillHaveChildrenToLaunch() {
    return !pcb_info_.isProcessCountReached();
}
bool OSS::Scheduler::stillHaveChildrenInSystem() {
    return pcb_info_.hasChildrenInSystem() || current_process_running_.pid != -1;
}

void OSS::Scheduler::launchChildrenIfAble() {
    if (pcb_info_.hasOpenPCBSlot() && oss_clock_->checkIfLaunchIntervalReached()) {
        if (
            !pcb_info_.isSimulataneousCountReached()
        ) {
            pcb_info_.simultaneous_count_++;
            forkProcess();
        } else if (
            !pcb_info_.isProcessCountReached() && !is_running_linear_process_
        ) {
            is_running_linear_process_ = true;
            forkProcess();
        }

        oss_clock_->setNewLaunchInterval();
    }
}

