#pragma once

#include <type_traits>

namespace tiling {

template <typename T>
concept EnumUint8 =
    std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, uint8_t>;

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

} // namespace tiling
