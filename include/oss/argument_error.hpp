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
        
        inline const std::string getOption() const {
            return option_;
        }
        inline const std::string getArg() const {
            return arg_;
        }
        inline const std::string getMessage() const {
            return message_;
        }
    };
}