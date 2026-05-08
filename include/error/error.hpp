#pragma once
#include <stdexcept>
#include <string>
#include <cstring>
#include <cerrno>

class Error : public std::runtime_error {
    private:
        std::string subject_ = "";
        std::string from_ = "";
        std::string err_message_ = "";        
    public:
        explicit Error(
            std::string subject, 
            std::string from, 
            const std::string &message, 
            const std::string err_message
        ): 
        subject_(subject), 
        from_(from), 
        std::runtime_error(message),
        err_message_(err_message) {}

        inline std::string getSubject() {
            return subject_;
        }
        inline std::string getFrom() {
            return from_;
        }
        inline std::string getErrMessage() {
            return err_message_;
        }
    };