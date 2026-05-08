#include "../../include/user_proc/user_proc.hpp"
#include <ctime>

UserProcess::UserProcess(int argc, char **argv)
{
    pid_ = getpid();
    ppid_ = getppid();

    logger_ = new Logger();

    logger_->logDebugINFO("UserProcess::UserProcess()", "Initializing process...");

    argument_processor_ = new ArgumentProcessor(argc, argv);

    argument_processor_->process(
        [this](Options options)
        {
            clock_checker_ = new ClockChecker(options.sec, options.nano, logger_);
        }
    );

    srand(getpid() ^ time(nullptr));
    msg_manager_ = new MsgManager("msgq.txt", 0644, pid_, logger_);
    
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
                std::cout << "UserProcess " + std::to_string(pid_) + "sending TERMINATION status to oss";
                msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);

                break;
            }
            else
            {
                std::cout << "UserProcess " + std::to_string(pid_) + " sending OSS_CONTROL status to oss";
                msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::OSS_CONTROL, -1, 0);
            }
        }   
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