#pragma once

#include "game_engine_sdk/traits.h"
#include <ostream>

namespace window {
template <Printable T = uint32_t> struct WindowDimension {
    T width;
    T height;

    WindowDimension(T w, T h) : width(w), height(h) {}
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const WindowDimension<T> &dim) {
    return os << "WindowDimension(" << dim.width << ", " << dim.height << ")";
}
} // namespace window
