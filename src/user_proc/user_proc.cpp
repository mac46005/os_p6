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

        }   
        cleanUp();
    }
    catch (std::exception &e)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void UserProcess::cleanUp()
{
    clock_checker_->cleanUp();
}