#include "../../include/oss/oss.hpp"

OSS::OSS::OSS(int argc, char **argv) {
    try {
        pid_ = getpid();
        argument_processor_ = new ArgumentProcessor(argc, argv);

        argument_processor_->processOptions(
            [this](Options options) {
                if (options.needsHelp == true) {
                    needs_help_ = true;
                } else {
                    oss_clock_ = new OSSClock(
                        "./src/oss/oss.cpp",
                        options.childTimeLimit,
                        options.launchLimit,
                        0,
                        100
                    );

                    msg_manager_ = new MsgManager("msgq.txt", 0644 | IPC_CREAT, pid_);
                }
            }
        );
    } catch (const ArgumentError &e) {
        
    } catch (Error &e) {
        
    } catch (std::exception &e) {
        
    }
}