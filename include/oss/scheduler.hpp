#pragma once
#include <vector>
#include <sys/wait.h>
#include "pcb_queue.hpp"
#include "oss_clock.hpp"
#include "../msg/msg_manager.hpp"
#include "pcb.hpp"
#include "../logger/logger.hpp"
#include <unistd.h>
#include <signal.h>

namespace OSS {
    class OssOutput;
    
    struct PCBInfo {
        int max_process_count_ = 0;
        int max_simultaneous_count_ = 0;
        int process_count_ = 0;
        int simultaneous_count_ = 0;
        const int MAX_PCB = 18;
        int pcb_count_ = 0;
        
        inline bool hasOpenPCBSlot(){
            if (pcb_count_ < MAX_PCB){
                return true;
            }
            return false;
        }
        inline bool isSimulCountReached() {
            if (simultaneous_count_ == max_simultaneous_count_) {
                return true;
            }
            return false;
        }
        inline bool isProcCountReached() {
            if (process_count_ == max_process_count_) {
                return true;
            }
            return false;
        }
        inline bool hasChildrenInSystem() {
            if (pcb_count_ > 0) {
                return true;
            }
            return false;
        }
    };

    class Scheduler {
    private:
        OSSClock *oss_clock_;
        OssOutput *oss_output_;
        MsgManager *msg_manager_;
        Logger *logger_;

        bool is_running_linear_process_ = false;
        pid_t linear_process_pid_ = -1;

        PCBInfo pcb_info_{};

        PCBQueue *pcb_ready_queue_;
        std::vector<PCB> pcb_blocked_list;
        std::vector<PCB> completed_processes;
        
        PCB current_process_running_ = PCB{.pid = -1};
        PCB createPCB(pid_t pid);
        void forkProcess();
        void terminateProcess();

        
    public:
        explicit Scheduler(
            int max_proc, 
            int max_simul, 
            OSSClock *oss_clock, 
            OssOutput *oss_output, 
            MsgManager *msg_manager,
            Logger *logger
        );
        
        bool stillHaveChildrenToLaunch();
        bool stillHaveChildrenInSystem();

        void launchChildrenIfAble();
        void launchProcess();
        void updateProcessInReadyQueue();
        
        std::vector<PCB> getCompletedProcesses() const;
        PCBQueue *getReadyQueue();
        std::vector<PCB> getBlockedList();
        PCB getCurrentProcessingRunning();

        void checkLinearProcessStatus();
        
        void requeueCurrentProcess();
        PCB *findBlockedByPid(pid_t pid);
        void cleanUpTerminatedPid(pid_t pid);

        void handleTERMINATE(pid_t pid);
        void handleOSS_CONTROL(MsgBuffer msg);
        void handleMEMORY_REQUEST(MsgBuffer msg);
        void cleanUp();
    };
}