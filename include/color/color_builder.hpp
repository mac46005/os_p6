#pragma once
#include <string>
#include "colors.hpp"
// Author: Marco Preciado
// Date: Sat Feb 28 10:25:30 PM CST 2026
// Purpose: Beatify output

namespace Color {
    class ColorBuilder {
    public:
        ColorBuilder &appendForeground(Colors color, std::string message) {
           output_ += prefix_;
           switch (color) {
               case Colors::BLACK:
                   output_ += "90m";
                   break;
               case Colors::RED:
                   output_ += "91m";
                   break;
               case Colors::GREEN:
                   output_ += "92m";
                   break;
               case Colors::YELLOW:
                   output_ += "93m";
                   break;
               case Colors::BLUE:
                   output_ += "94m";
                   break;
               case Colors::MAGENTA:
                   output_ += "95m";
                   break;
               case Colors::CYAN:
                   output_ += "96m";
                   break;
               case Colors::WHITE:
                   output_ += "97m";
                   break;
               case Colors::DEFAULT:
                   output_ += "0m";
                   break;
               default:
                   output_ += "0m";
                   break;
           }
           output_ += message;
           output_ += prefix_ + "99m";

           return *this;
        }
        std::string build() {
            return output_;
        }
    private:
        std::string prefix_{"\033["};
        std::string output_{""};
    };
}
