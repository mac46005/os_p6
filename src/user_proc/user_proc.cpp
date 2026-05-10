#include "../../include/user_proc/user_proc.hpp"
#include <ctime>

UserProcess::UserProcess(int argc, char **argv)
{
    pid_ = getpid();
    ppid_ = getppid();

    logger_ = new Logger();

    

    argument_processor_ = new ArgumentProcessor(argc, argv);

    argument_processor_->process(
        [this](Options options)
        {
            clock_checker_ = new ClockChecker(options.sec, options.nano, logger_);
        }
    );

    srand(getpid() ^ time(nullptr));
    msg_manager_ = new MsgManager("msgq.txt", 0644, pid_, logger_);
    logger_->logDebugINFO("UserProcess::UserProcess()", "PID " + std::to_string(pid_) + "\tInitialized at " + clock_checker_->toString());
}

int UserProcess::run()
{
    try
    {
        MsgBuffer recieve{};
        while (true)
        {
            msg_manager_->recieveMessage(
                [this, &recieve](MsgBuffer msg)
                {
                    recieve = msg;

                    // dont forget that parent can force kill you
                },
                0
            );

            if (clock_checker_->isTimeUp())
            {
                logger_->logDebugCAUTION("UserProcess::run()", "Time is Up. Sending MSG to OSS and TERMINATING @ " + clock_checker_->toString());
                msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);

                break;
            }
            else
            {
                logger_->logDebugCAUTION("UserProcess::run()", "Time is not Up, sending OSS_CONTROL to OSS  @ " + clock_checker_->toString());
                msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::OSS_CONTROL, -1, 0);
            }
        }
        logger_->logDebugWARNING("UserProcess::run()", "Terminated");   
        cleanUp();
    }
    catch (std::exception &e)
    {
        logger_->logDebugWARNING(std::string("PID ") + std::to_string(pid_) + " FUALT", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void UserProcess::cleanUp()
{
    clock_checker_->cleanUp();
}