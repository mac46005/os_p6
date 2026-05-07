#pragma once
#include <functional>
#include <unistd.h>
#include <stdexcept>
#include <string.h>
#include <string>
#include "argument_error.hpp"

namespace OSS {
    struct Options {
        bool needsHelp = false;
        int maxProc = 1;
        int maxSimul = 1;
        float childTimeLimit = 1;
        float launchInterval = 1;
        std::string logFile = "oss.log";
    };

    class ArgumentProcessor{
    private:
        const char *optstr_ = "hn:s:t:i:f:";
        int argc_;
        char **argv_;
    public:
        explicit ArgumentProcessor(int argc, char **argv);
        void processOptions(std::function<void(Options)> useProcessedOptions);
    };
}