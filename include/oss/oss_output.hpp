#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>


namespace OSS {
    class Output {
        private:
            std::string file_name_;
            std::ofstream operation_log_;
            std::ofstream debug_log_;
            
            int operation_log_line_cout_ = 0;
            const int MAX_OPERATION_LOG_LINES = 10000;

        public:
            void logDebugINFO();
            void logDebugWARNING();
            void logDebugERROR();


    };
}