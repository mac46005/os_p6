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

                    output_ = new Output(options.logFile);

                    oss_clock_ = new OSSClock(
                        "./src/oss/oss.cpp",
                        options.childTimeLimit,
                        options.launchInterval,
                        0,
                        100
                    );

                    msg_manager_ = new MsgManager("msgq.txt", 0644 | IPC_CREAT, pid_);

                    scheduler_ = new Scheduler(options.maxProc, options.maxSimul, this->output_, this->oss_clock_, this->msg_manager_);
                }
            }
        );
    }
    catch (const ArgumentError &e)
    {
        if (output_)
        {
            output_->logDebugWARNING("ArgumentProcessor", e.what());
        }
        else
        {
            std::cerr << "ArgumentProcessor: " << e.what() << std::endl;
        }
    }
    catch (Error &e)
    {
        if (output_)
        {
            output_->logDebugWARNING(e.getSubject(), e.what());
        }
        else
        {
            std::cerr << e.getSubject() << ": " << e.what() << std::endl;
        }
    }
    catch (std::exception &e)
    {
        if (output_)
        {
            output_->logDebugWARNING("std::exception", e.what());
        }
        else
        {
            std::cerr << "std::exception: " << e.what() << std::endl;
        }
    }
}

int OSS::OSS::run() {
    if (needs_help_) {
        output_->printHelpMessage();
    } else {
        try {
            while(
                !g_timeout && !g_stop &&
                (scheduler_->stillHaveChildrenToLaunch() || scheduler_->stillHaveChildrenInSystem())
            ) {
                scheduler_->launchChildrenIfAble();
                oss_clock_->updateClockByQuantum();
                scheduler_->updateProcessInReadyQueue();
            }
        } catch (Error &e) {
            output_->logDebugWARNING(e.getSubject(), e.getErrMessage());
            return EXIT_FAILURE;
        } catch (std::exception &e) {
            output_->logDebugWARNING("OSS::run()", e.what());
            return EXIT_FAILURE;
        }
    }
    output_->logDebugINFO("OSS", "TERMINATING...");
    cleanUp();
    
    return EXIT_SUCCESS;
}

void OSS::OSS::cleanUp() {
    output_->cleanUp();
    oss_clock_->cleanUp();
    msg_manager_->cleanUp();
    delete output_;
    delete oss_clock_;
    delete msg_manager_;
}