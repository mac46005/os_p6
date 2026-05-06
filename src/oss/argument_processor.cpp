#include "../../include/oss/argument_processor.hpp"

OSS::ArgumentProcessor::ArgumentProcessor(int argc, char **argv): argc_(argc), argv_(argv) {}

void OSS::ArgumentProcessor::processOptions(std::function<void(Options)> useProcessedOptions) {
    int maxProcesses = 100;
    int maxSimulations = 15;

    Options options{};
    int opt;
    while ((opt = getopt(argc_, argv_, opstr_)) != -1) {
        switch (opt) {
            case 'h':
                options.needsHelp = true;
                break;
            case 'n':
                try {
                    options.maxProc = std::stoi(optarg);
                } catch (std::exception &e) {
                    throw ArgumentError("n", "proc", "Please input a valid whole number");
                }

                if (options.maxProc > maxProcesses) {
                    throw ArgumentError("n", "proc", "Requires a number that is less than 100");
                }

                break;
            case 's':
                try {
                    options.maxSimul = std::stoi(optarg);
                } catch (std::exception &e) {
                    throw ArgumentError("s", "simul", "Please input a valid whole number");
                }

                if (options.maxSimul > options.maxProc) {
                    throw ArgumentError("s", "simul", "Requires a number that is less than or equal [-n proc]");
                } else if (options.maxSimul > maxSimulations) {
                    throw ArgumentError("s", "simul", "Requires a numbet that is less than 16");
                }

                break;
            case 't':
                try {
                    options.childTimeLimit = std::stof(optarg);
                } catch (std::exception &e) {
                    throw ArgumentError("t", "childTimeLimit", "Requires a valid decimal number");
                }
                break;
            case 'i':
                try {
                    options.launchLimit = std::stof(optarg);
                } catch (std::exception &e) {
                    throw ArgumentError("i", "launchInterval", "Requires a valid decimal number");
                }
                break;
            case 'f':
                if (optarg != "") {
                    options.logFile = optarg;
                }
                break;
            default:
                options.needsHelp = true;
                break;
        }

        useProcessedOptions(options);
    }
}