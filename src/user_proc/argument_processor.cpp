#include "../../include/user_proc/argument_processor.hpp"

ArgumentProcessor::ArgumentProcessor(int argc, char **argv): argc_(argc), argv_(argv) {

}

void ArgumentProcessor::process(std::function<void(Options)> useOptions) {
    Options options;
    if (argc_ < 3) {
        throw Error("ArgumentProcessor", "process()", "", std::strerror(errno));
    } else {
        try {
            int sec = std::stoi(argv_[1]);
            int nano = std::stoi(argv_[2]);

            options.sec = sec;
            options.nano = nano;
            
        } catch (std::exception &e) {
            throw Error("Argument", "processor", "Failed to convert arguments into int", std::strerror(errno));
        }
    }
    useOptions(options);
}