#pragma once
#include <vector>
#include <sys/wait.h>
#include "oss_clock.hpp"
#include "oss_output.hpp"
#include "pcb.hpp"
#include "../msg/msg_manager.hpp"
#include "pcb_queue.hpp"

namespace OSS {
    struct PCBInfo {
        int max_process_count_ = 0;
        int max_simultaneous_count_ = 0;
        int process_count_ = 0;
        int simultaneous_count_ = 0;
        const int MAX_PCB = 18;
        int pcb_count_ = 0;

        inline const bool hasOpenPCBSlot() const {
            if (pcb_count_ < MAX_PCB) {
                return true;
            }
            return false;
        }
        inline const bool isSimulataneousCountReached() const {
            if (simultaneous_count_ == max_simultaneous_count_) {
                return true;
            }
            return false;
        }
        inline const bool isProcessCountReached() const {
            if (process_count_ == max_process_count_) {
                return true;
            }
            return false;
        }

        inline const bool hasChildrenInSystem() const {
            if (pcb_count_ > 0) {
                return true;
            }
            return false;
        }
    };

    class Scheduler {
        private:
            OSSClock *oss_clock_;
            Output *output_;
            MsgManager *msg_manager_;
            // resource manager

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
            void handleOSSControl();
            void handleTERMINATE(pid_t pid);
            void requeueCurrentProcess();
            void canUnblockBlockedProcesses();
        public:
            explicit Scheduler(
                int max_proc,
                int max_simul,
                Output *output,
                OSSClock *oss_clock,
                MsgManager *msg_manager
            );
            bool stillHaveChildrenToLaunch();
            bool stillHaveChildrenInSystem();
            void launchChildrenIfAble();
            void updateProcessInReadyQueue();
            void cleanUp();
    };

}