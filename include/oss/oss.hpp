#pragma once
#include "argument_processor.hpp"
#include "oss_clock.hpp"
#include "oss_output.hpp"
#include "scheduler.hpp"
#include "../msg/msg_manager.hpp"
#include "signal_flags.hpp"
#include "memory_manager.hpp"

namespace OSS {
    class OSS {
    private:
        pid_t pid_ = 0;
        bool needs_help_ = false;
        OssOutput *oss_output_;
        ArgumentProcessor *argument_processor_;
        OSSClock *oss_clock_;
        MemoryManager *memory_manager_;
        Scheduler *scheduler_;
        MsgManager *msg_manager_;
        Time next_table_dump_{0, 500000000};
        Time next_deadlock_check_{1,0};
        inline bool shouldPrintTables() {
            Time now = oss_clock_->getCurrentTime();
            return (now.sec > next_table_dump_.sec) || (now.sec == next_table_dump_.sec && now.nano >= next_table_dump_.nano);
        }
        inline void advanceNextTableDump() {
            Clock::addTimeToPtrTime(&next_table_dump_, Time{0, 500000000});
        }
        inline bool shouldResolveDeadlock() {
            Time now = oss_clock_->getCurrentTime();
            return (now.sec > next_deadlock_check_.sec) || (now.sec == next_deadlock_check_.sec && now.nano >= next_deadlock_check_.nano);
        }
        inline void advanceNextDeadlockResolve() {
            Clock::addTimeToPtrTime(&next_deadlock_check_, Time{1,0});
        }
    public:
        explicit OSS(int argc, char ** argv);
        int run();
        void cleanUp();
        void shutDownChildren();
    };
}