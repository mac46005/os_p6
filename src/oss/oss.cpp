#include "../../include/oss/oss.hpp"

OSS::OSS::OSS(int argc, char **argv)
{
    try
    {
        pid_ = getpid();
        argument_processor_ = new ArgumentProcessor(argc, argv);
        argument_processor_->processOptions(
            [this](Options options)
            {
                if (options.needsHelp == true)
                {
                    needs_help_ = options.needsHelp;
                }
                else
                {
                    oss_output_ = new OssOutput(options.logFile);

                    oss_output_->initializeDebugFile();
                    
                    oss_clock_ = new OSSClock(
                        "./src/oss/oss.cpp",
                        options.childTimeLimit,
                        options.launchInterval,
                        0,
                        1000000,
                        oss_output_
                    );


                    msg_manager_ = new MsgManager(
                        "msgq.txt", 0644 | IPC_CREAT, pid_, 
                        oss_output_
                    );

                    scheduler_ = new Scheduler(
                        options.maxProc,
                        options.maxSimul,
                        this->oss_clock_,
                        this->oss_output_,
                        this->msg_manager_,
                        oss_output_
                    );

                    
                }
            });
    }
    catch (ArgumentError &e)
    {
        // oss_output_->printArgumentErrorMessage(e);
    }
    catch (Error &e)
    {
        // oss_output_->printOssErrorMessage(e);
    }
    catch (std::exception &e)
    {
        Color::printError("OSS::OSS::OSS(int argc, char **argv)", e.what(), std::string(std::strerror(errno)));
    }
}

int OSS::OSS::run()
{
    if (needs_help_)
    {
        oss_output_->printHelpMessage();
    }
    else
    {
        try
        {
            while (
                !g_timeout && !g_stop &&
                (scheduler_->stillHaveChildrenToLaunch() || scheduler_->stillHaveChildrenInSystem() ))
            {
                scheduler_->checkLinearProcessStatus();
                scheduler_->launchChildrenIfAble();
                scheduler_->updateProcessInReadyQueue();
                oss_clock_->updateClockByQuantum();

                if (shouldPrintTables()) {
                    
                    advanceNextTableDump();
                }

                if (shouldResolveDeadlock()) {
                    advanceNextDeadlockResolve();
                }
            }
        }
        catch (Error &e)
        {
            return EXIT_FAILURE;
        }
        catch (std::exception &e)
        {
            return EXIT_FAILURE;
        }
        
    }
    Color::printInfo("OSS", "Terminating...");
    // oss_output_->logFinalReport(resource_manager_, scheduler_);
    cleanUp();
    return EXIT_SUCCESS;
}


void OSS::OSS::cleanUp() {
    

    oss_clock_->cleanUp();
    msg_manager_->cleanUp();
    oss_output_->cleanUp();
    scheduler_->cleanUp();

    delete scheduler_;
    delete msg_manager_;
    delete oss_clock_;
    delete oss_output_;

}

