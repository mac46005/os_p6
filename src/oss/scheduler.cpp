#include "../../include/oss/scheduler.hpp"

OSS::Scheduler::Scheduler(
    int max_proc,
    int max_simul,
    OSSClock *oss_clock,
    OssOutput *oss_output,
    MsgManager *msg_manager,
    Logger *logger
) : oss_clock_(oss_clock), oss_output_(oss_output), msg_manager_(msg_manager), logger_(logger)
{
    pcb_info_.max_process_count_ = max_proc;
    pcb_info_.max_simultaneous_count_ = max_simul;
    pcb_ready_queue_ = new PCBQueue();
}

OSS::PCB OSS::Scheduler::createPCB(pid_t pid)
{
    OSS::PCB pcb{
        .pid = pid,
        .start_time = oss_clock_->getCurrentTime(),        
        .requested_resource = -1
    };
    return pcb;
}

void OSS::Scheduler::forkProcess()
{
    Time current_time = oss_clock_->getCurrentTime();

    Time child_time_limit_ = oss_clock_->getChildTimeLimit();
    Clock::addTimeToPtrTime(&child_time_limit_, current_time);
    logger_->logDebugWARNING("Scheduler::forkProcess()", "New child time limit = " + std::to_string(child_time_limit_.sec) + ":" + std::to_string(child_time_limit_.nano));
    std::string child_runtime_sec_str = std::to_string(child_time_limit_.sec);
    std::string child_runtime_nano_str = std::to_string(child_time_limit_.nano);

    char *args[] = {
        const_cast<char *>("./user_proc"),
        const_cast<char *>(child_runtime_sec_str.c_str()),
        const_cast<char *>(child_runtime_nano_str.c_str()),
        nullptr
    };

    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
        perror("excvp failed");
        _exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        pcb_info_.pcb_count_++;
        pcb_info_.process_count_++;
        PCB pcb = createPCB(pid);
        pcb_ready_queue_->enqueue(pcb);

        if (is_running_linear_process_) {
            linear_process_pid_ = pid;
        }

        logger_->logDebugCAUTION("Scheduler::forkProcess()", "\tPCB Count: " + std::to_string(pcb_info_.pcb_count_) + "\tProcess Count: " + std::to_string(pcb_info_.process_count_) + "\tPID: " + std::to_string(pid) + "\tIs Linear Process: " + std::to_string(is_running_linear_process_));
        // output to log
    }
}

bool OSS::Scheduler::stillHaveChildrenToLaunch()
{
    return !pcb_info_.isProcCountReached();
}
bool OSS::Scheduler::stillHaveChildrenInSystem()
{
    return pcb_info_.hasChildrenInSystem() || current_process_running_.pid != -1;
}

void OSS::Scheduler::launchChildrenIfAble()
{
    if (pcb_info_.hasOpenPCBSlot() && oss_clock_->launchIntervalReached())
    {
        if (
            !pcb_info_.isSimulCountReached()
        )
        {
            logger_->logDebugCAUTION("OSS::Scheduler::launchChildrenIfAble()", "Launching simultaneous process");
            pcb_info_.simultaneous_count_++;
            forkProcess();
        }
        else if (
            !pcb_info_.isProcCountReached() && !is_running_linear_process_
        ) 
        {
            logger_->logDebugCAUTION("OSS::Scheduler::launchChildrenIfAble()", "Launching linear process");
            // ADD FLAG THAT LINEAR PROCESS RUN
            is_running_linear_process_ = true;
            forkProcess();
        }
        oss_clock_->setNewLaunchInterval();
    }
}

std::vector<OSS::PCB> OSS::Scheduler::getCompletedProcesses() const {
    return completed_processes;
}
OSS::PCBQueue *OSS::Scheduler::getReadyQueue() {
    return pcb_ready_queue_;
}

std::vector<OSS::PCB> OSS::Scheduler::getBlockedList() {
    return pcb_blocked_list;
}
OSS::PCB OSS::Scheduler::getCurrentProcessingRunning() {
    return current_process_running_;
}








void OSS::Scheduler::requeueCurrentProcess() {
    if (current_process_running_.pid > 0) {
        pcb_ready_queue_->enqueue(current_process_running_);


        if (is_running_linear_process_ && current_process_running_.pid == linear_process_pid_) {
            is_running_linear_process_ = false;
            linear_process_pid_ = -1;
        }
        logger_->logDebugCAUTION("Scheduler::requeueCurrentProcess()", "Requeued PID " + std::to_string(current_process_running_.pid) + "\tis_running_linear_process_ = " + std::to_string(is_running_linear_process_));
        current_process_running_= PCB{.pid = -1};
        
        
    }
}


void OSS::Scheduler::cleanUpTerminatedPid(pid_t pid) {

    
}

void OSS::Scheduler::terminateProcess()
{
    cleanUpTerminatedPid(current_process_running_.pid);
}




void OSS::Scheduler::handleTERMINATE(pid_t pid) {

    logger_->logDebugWARNING("Scheduler::handleTERMINATE()", "PID " + std::to_string(pid) + " TERMINATED at " + oss_clock_->toString());

    if (pid == current_process_running_.pid) {
        pcb_info_.pcb_count_--;
        if (linear_process_pid_ == pid) {
            linear_process_pid_ = -1;
            is_running_linear_process_ = false;
        }
        current_process_running_.end_time = oss_clock_->getCurrentTime();
        completed_processes.push_back(current_process_running_);
        current_process_running_ = PCB{.pid = -1};
    }
}

void OSS::Scheduler::handleOSS_CONTROL() {
    logger_->logDebugWARNING("Scheduler::handleOSS_CONTROL()", "Requeuing PID " + std::to_string(current_process_running_.pid));
    requeueCurrentProcess();
}

void OSS::Scheduler::updateProcessInReadyQueue()
{
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

    msg_manager_->recieveMessage(
        [this](MsgBuffer msg) {
            switch (msg.status) {
                case ProcessStatus::TERMINATE:
                    handleTERMINATE(msg.sender_pid);
                    break;
                case ProcessStatus::OSS_CONTROL:
                    handleOSS_CONTROL();
                    break;
            }
        },
        0
    );
}

void OSS::Scheduler::checkLinearProcessStatus() {
    if (!is_running_linear_process_ || linear_process_pid_ <= 0) {
        return;
    }

    int status = 0;
    pid_t result = waitpid(linear_process_pid_, &status, WNOHANG);

    if (result == linear_process_pid_) {
        // is_running_linear_process_ = false;
        // linear_process_pid_ = -1;
        // pcb_info_.pcb_count_--;
        cleanUpTerminatedPid(linear_process_pid_);
    }
}



void OSS::Scheduler::cleanUp()
{
    if (current_process_running_.pid > 0) {
        kill(current_process_running_.pid, SIGTERM);
    }

    for (auto &pcb : pcb_blocked_list) {
        if (pcb.pid > 0) {
            kill(pcb.pid, SIGTERM);
        }
    }

    pcb_ready_queue_->traverse(
        [](PCB *pcb) {
            if (pcb->pid > 0) {
                kill(pcb->pid, SIGTERM);
            }
        }
    );

    int status = 0;
    while (waitpid(-1, &status, 0) > 0){};

    delete pcb_ready_queue_;
}