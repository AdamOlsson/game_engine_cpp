#pragma once

#include <type_traits>

namespace tiling {

template <typename T>
concept EnumUint8 =
    std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, uint8_t>;

} // namespace tiling
