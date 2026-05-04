#pragma once
#include <sys/types.h>
#include <iostream>
#include <iomanip>
#include "color_builder.hpp"


namespace Color {
    inline void label(ColorBuilder &cb, std::string label) {
        cb.appendForeground(Colors::YELLOW, label + ": ");
    };
    
    inline void optionalContainerStart(ColorBuilder &cb) {
        cb.appendForeground(Colors::BLUE, "[ ");
    };
    inline void optionalContainerEnd(ColorBuilder &cb) {
        cb.appendForeground(Colors::BLUE, " ]");
    }
    inline void title(ColorBuilder &cb, std::string title) {
        cb.appendForeground(Colors::CYAN, title);
    };
    inline void option(ColorBuilder &cb, std::string option) {
        cb.appendForeground(Colors::MAGENTA, "-" + option);
    };
    inline void optionArgument(ColorBuilder &cb, std::string arg) {
        cb.appendForeground(Colors::YELLOW, arg);
    };
    inline void space(ColorBuilder &cb) {
        cb.appendForeground(Colors::DEFAULT, " ");
    };
    inline void newLine(ColorBuilder &cb) {
        cb.appendForeground(Colors::DEFAULT, "\n");
    };
    inline void tab(ColorBuilder &cb) {
        cb.appendForeground(Colors::DEFAULT, "\t");
    };
    inline void purpose(ColorBuilder &cb, std::string message) {
        label(cb, "Purpose");
        tab(cb);
        cb.appendForeground(Colors::DEFAULT, message);
    };
    inline void appendScopeOperator(ColorBuilder &cb) {
        cb.appendForeground(Colors::DEFAULT, "::");
    }
    inline void defaultOutput(ColorBuilder &cb, std::string message) {
        cb.appendForeground(Colors::DEFAULT, message);
    }

    inline void appendWarningMark(ColorBuilder &cb) {
        std::string warning_line = "WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING";
        cb.appendForeground(Colors::YELLOW, warning_line);
        Color::newLine(cb);
        cb.appendForeground(Colors::YELLOW, warning_line);
        Color::newLine(cb);
        cb.appendForeground(Colors::YELLOW, warning_line);
        Color::newLine(cb);
    }

    inline void appendErrorMark(ColorBuilder &cb) {
        std::string error_line = "ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR";
        cb.appendForeground(Colors::RED, error_line);
        newLine(cb);
        cb.appendForeground(Colors::RED, error_line);
        newLine(cb);
        cb.appendForeground(Colors::RED, error_line);
        newLine(cb);
    }

    inline void argumentInfo(ColorBuilder &cb, std::string opt, std::string arg, std::string description) {
        tab(cb);
        option(cb, opt);
        space(cb);
        optionArgument(cb, arg);
        tab(cb);
        cb.appendForeground(Colors::DEFAULT, description);
    };

    inline void printError(std::string subject, std::string message, std::string err) {
        ColorBuilder cb;
        cb.appendForeground(Colors::RED, "ERROR ERROR ERROR ERROR ERROR ERROR");
        newLine(cb);
        cb.appendForeground(Colors::YELLOW, "From: ");
        cb.appendForeground(Colors::CYAN, subject);
        newLine(cb);
        cb.appendForeground(Colors::RED, message);
        newLine(cb);
        cb.appendForeground(Colors::YELLOW, "errno: ");
        newLine(cb);
        cb.appendForeground(Colors::RED, err);
        newLine(cb);
        cb.appendForeground(Colors::RED, "ERROR ERROR ERROR ERROR ERROR ERROR");
        newLine(cb);
        std::cout << cb.build();
    }

    inline void printInfo(std::string subject, std::string message) {
        ColorBuilder cb;
        cb.appendForeground(Colors::YELLOW, "INFO INFO INFO INFO INFO INFO INFO INFO INFO");
        newLine(cb);
        label(cb, "From");
        space(cb);
        title(cb, subject);
        newLine(cb);
        label(cb, "Message:");
        newLine(cb);
        defaultOutput(cb, message);
        newLine(cb);
        cb.appendForeground(Colors::YELLOW, "INFO INFO INFO INFO INFO INFO INFO INFO INFO");
        newLine(cb);
        newLine(cb);
        std::cout << cb.build();
    }
};
