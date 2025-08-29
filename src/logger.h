#pragma once

#include "io.h"
#include "traits.h"
#include <iostream>
#include <source_location>
namespace logger {

template <Printable... Args>
static void log(const std::string &level, const std::source_location &location,
                const Args &...args) {
    std::cout << level << "::" << location.file_name() << ":" << location.line() << " in "
              << location.function_name() << "() - ";
    ((std::cout << args), ...);
    std::cout << std::endl;
}

template <Printable... Args> static void debug(const Args &...args) {
    const std::source_location &location = std::source_location::current();
    log("Debug", location, args...);
}

template <Printable... Args> static void info(const Args &...args) {
    const std::source_location &location = std::source_location::current();
    log("Info", location, args...);
}

template <Printable... Args> static void warning(const Args &...args) {
    const std::source_location &location = std::source_location::current();
    log("Warning", location, args...);
}

template <Printable... Args> static void error(const Args &...args) {
    const std::source_location &location = std::source_location::current();
    log("Error", location, args...);
}

} // namespace logger
