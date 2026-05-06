#pragma once
#include "argument_processor.hpp"
#include "oss_clock.hpp"
#include "../msg/msg_manager.hpp"

namespace OSS {
    class OSS {
        private:
            pid_t pid_ = 0;
            bool needs_help_ = false;
            ArgumentProcessor *argument_processor_;
            OSSClock *oss_clock_;
            MsgManager *msg_manager_;

            Time next_table_dump_{0, 500000000};
            const Time TABLE_DUMP_INCREMENT{0, 500000000};
            inline bool shouldPrintTables() {
                Time *now = oss_clock_->getCurrentTime();
                return Clock::gteq(next_table_dump_, *now);
            }
            inline void advanceNextTableDump() {
                Clock::addTimeToPtrTime(&next_table_dump_, TABLE_DUMP_INCREMENT);
            }
        public:
            explicit OSS(int argc, char **argv);
            int run();
            void cleanUp();
            void shutDownChildren();
    };
}