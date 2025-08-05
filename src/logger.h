#pragma once

#include <iostream>
#include <source_location>
#include <string>
namespace logger {
static void
debug(const std::string &message,
      const std::source_location &location = std::source_location::current()) {
    std::cout << "Debug::" << location.file_name() << ":" << location.line() << " in "
              << location.function_name() << "() - " << message << std::endl;
}

static void info(const std::string &message,
                 const std::source_location &location = std::source_location::current()) {
    std::cout << "Info::" << location.file_name() << ":" << location.line() << " in "
              << location.function_name() << "() - " << message << std::endl;
}

static void
warning(const std::string &message,
        const std::source_location &location = std::source_location::current()) {
    std::cout << "Warning::" << location.file_name() << ":" << location.line() << " in "
              << location.function_name() << "() - " << message << std::endl;
}

static void
error(const std::string &message,
      const std::source_location &location = std::source_location::current()) {
    std::cout << "Error::" << location.file_name() << ":" << location.line() << " in "
              << location.function_name() << "() - " << message << std::endl;
}

} // namespace logger
