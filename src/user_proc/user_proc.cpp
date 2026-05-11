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
    // logger_->logDebugINFO("UserProcess::UserProcess()", "PID " + std::to_string(pid_) + "\tInitialized at " + clock_checker_->toString());
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
                },
                0
            );

            if (recieve.status == ProcessStatus::GRANTED) {
                logger_->logDebugCAUTION("UserProcesss::run()", "ProcessStatus::GRANTED");
                continue;
            }


            if (recieve.status != ProcessStatus::OSS_CONTROL) {
                logger_->logDebugWARNING(
                    "UserProcess::run()",
                    "Unexpected message status"
                );

                continue;
            }



            if (clock_checker_->isTimeUp())
            {
                logger_->logDebugCAUTION("UserProcess::run()", "Time is Up. Sending MSG to OSS and TERMINATING @ " + clock_checker_->toString());
                msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, 0, 0, 0, AccessType::NONE, 0);

                break;
            }
            else
            {
                // logger_->logDebugCAUTION("UserProcess::run()", "Time is not Up, sending OSS_CONTROL to OSS  @ " + clock_checker_->toString());
                // msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::OSS_CONTROL, 0, 0, 0, AccessType::NONE, 0);

                int page = rand() % 16;
                int offset = rand() % 1024;
                int address = page * 1024 + offset;
                bool is_write = (rand() % 100) < 20;
                logger_->logDebugINFO("UserProcess::run()", "is_write: " + std::to_string(is_write) + "\tChose page: " + std::to_string(page) + "\toffset: " + std::to_string(offset) + "\taddress: " + std::to_string(address));
                
                AccessType access = is_write ? AccessType::WRITE : AccessType::READ;
                // logger_->logDebugWARNING("UserProcess::run()", "Attempting to send message to OSS");
                msg_manager_->sendMessage(
                    ppid_,
                    pid_,
                    ProcessStatus::MEMORY_REQUEST,
                    address,
                    page,
                    offset,
                    access,
                    0
                );
                // logger_->logDebugINFO("UserProcess::run()", "Message sent successfully");
            }
        }
        // logger_->logDebugWARNING("UserProcess::run()", "Terminated");   
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