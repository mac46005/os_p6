#pragma once
#include <stdexcept>
#include <string>

namespace OSS {
    class ArgumentError : public std::invalid_argument {
    private:
        std::string option_ = "";
        std::string arg_ = "";
        std::string message_ = "";
    public:
        explicit ArgumentError(
            std::string option, 
            std::string arg, 
            const std::string &message
        ):
        option_(option),
        arg_(arg),
        message_(message), 
        std::invalid_argument(message) {}
        
        inline std::string getOption() {
            return option_;
        }
        inline std::string getArg() {
            return arg_;
        }
        inline std::string getMessage() {
            return message_;
        }
    };
}