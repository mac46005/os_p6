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

        output_->logDebugCAUTION("Scheduler::forkProcess()", std::string("OSS forking process pid: ") + std::string(std::to_string(pid)));
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
            output_->logDebugCAUTION("Scheduler::launchChildrenIfAble()", "forking simul process");
            pcb_info_.simultaneous_count_++;
            forkProcess();
        } else if (
            !pcb_info_.isProcessCountReached() && !is_running_linear_process_
        ) {
            output_->logDebugCAUTION("OSS::launchChildrenIfAble()", "forking proc process");
            is_running_linear_process_ = true;
            forkProcess();
        }

        oss_clock_->setNewLaunchInterval();
    }

}
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
void OSS::Scheduler::canUnblockBlockedProcesses() {
    // will probably need to reimplement a different way
}
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
// UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED

void OSS::Scheduler::requeueCurrentProcess() {
    if (current_process_running_.pid > 0) {
        pcb_ready_queue_->enqueue(current_process_running_);
        current_process_running_ = PCB{.pid = -1};
        if (is_running_linear_process_) {
            is_running_linear_process_ = false;
        }
    }
}
void OSS::Scheduler::handleOSSControl() {
    requeueCurrentProcess();
}
void OSS::Scheduler::handleTERMINATE(pid_t pid) {
    if (pid <= 0) {
        return;
    }

    Time *current_time = oss_clock_->getCurrentTime();
    bool found = false;

    if (current_process_running_.pid == pid) {
        current_process_running_.end_time = *current_time;
        completed_processes.push_back(current_process_running_);
        current_process_running_ = PCB{.pid = -1};
        found = true;
        output_->logDebugWARNING("OSS::handleTERMINATE()", "PID " + std::string(std::to_string(pid)) + " Terminated");
    } else {
        for (auto it = pcb_blocked_list.begin(); it != pcb_blocked_list.end(); ++it)
        {
            if (it->pid == pid)
            {
                it->end_time = *current_time;
                output_->logDebugWARNING("OSS::handleTERMINATE()", "PID " + std::string(std::to_string(pid)) + " Terminated");
                completed_processes.push_back(*it);
                pcb_blocked_list.erase(it);

                found = true;
                break;
            }
        }
    }

    if (!found) {
        PCBQueue *new_queue = new PCBQueue();

        while (!pcb_ready_queue_->isEmpty()) {
            PCB pcb = pcb_ready_queue_->dequeue();

            if (pcb.pid == pid) {
                // release resources here

                pcb.end_time = *current_time;

                output_->logDebugWARNING("OSS::handleTERMINATE()", "PID " + std::string(std::to_string(pid)) + " Terminated");

                completed_processes.push_back(pcb);

                found = true;

            } else {
                new_queue->enqueue(pcb);
            }
        }

        delete pcb_ready_queue_;
        pcb_ready_queue_ = new_queue;
    }

    if (is_running_linear_process_ && linear_process_pid_ == pid) {
        is_running_linear_process_ = false;
        linear_process_pid_ = -1;
    }

    if (found) {
        if (pcb_info_.pcb_count_ > 0) {
            pcb_info_.pcb_count_--;
        }

        // UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
        canUnblockBlockedProcesses();
        // UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED UNIMPLEMENTED
    }
    
}







void OSS::Scheduler::updateProcessInReadyQueue() {
    if (current_process_running_.pid != -1) {
        return;
    }

    if (pcb_ready_queue_->isEmpty()) {
        return;
    }

    current_process_running_ = pcb_ready_queue_->dequeue();

    msg_manager_->sendMessage(
        current_process_running_.pid,
        getpid(),
        ProcessStatus::OSS_CONTROL,
        -1,
        0
    );
    msg_manager_->receiveMessage(
        [this](MsgBuffer msg) {
            switch (msg.status) {
                case ProcessStatus::OSS_CONTROL:
                    handleOSSControl();
                    break;
                case ProcessStatus::TERMINATE:
                    handleTERMINATE(msg.sender_pid);
                    break;
            }
        },
        0
    );
}

void OSS::Scheduler::cleanUp() {
    oss_clock_->cleanUp();
}