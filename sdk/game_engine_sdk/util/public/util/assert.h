#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace assert {
#ifdef NDEBUG
#define DEBUG_ASSERT(condition, message) ((void)0)
#else

#define DEBUG_ASSERT(condition, message)                                                 \
    do {                                                                                 \
        if (!(condition)) {                                                              \
            std::cerr << "Assertion failed: (" << #condition << "), "                    \
                      << "function " << __FUNCTION__ << ", file " << __FILE__            \
                      << ", line " << __LINE__ << ".\n"                                  \
                      << "Message: " << message << std::endl;                            \
            std::abort();                                                                \
        }                                                                                \
    } while (false)
#endif

#ifdef NDEBUG
#define DEBUG_CODE(code) ((void)0)
#else
#define DEBUG_CODE(code)                                                                 \
    do {                                                                                 \
        code                                                                             \
    } while (false)

#endif

} // namespace assert
