#pragma once

#include "game_engine_sdk/traits.h"
#include <type_traits>

namespace tiling {
template <typename T>
concept WangEnumUint8 =
    std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, uint8_t> &&
    Printable<T>;

}
