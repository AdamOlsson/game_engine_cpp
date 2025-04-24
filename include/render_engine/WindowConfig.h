#pragma once

#include <cstdint>

struct WindowDimensions {
    uint32_t width;
    uint32_t height;
    WindowDimensions(uint32_t width, uint32_t height) : width(width), height(height) {}
};

struct WindowConfig {
    WindowDimensions dims;
    char const *title;
};
