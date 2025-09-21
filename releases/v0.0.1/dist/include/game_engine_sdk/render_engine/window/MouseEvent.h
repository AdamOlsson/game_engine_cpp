#pragma once
#include "ViewportPoint.h"
#include <functional>

namespace window {
enum class MouseEvent {
    CURSOR_MOVED,
    LEFT_BUTTON_DOWN,
    LEFT_BUTTON_UP,
    RIGHT_BUTTON_DOWN,
    RIGHT_BUTTON_UP
};

using MouseEventCallbackFn = std::function<void(MouseEvent, ViewportPoint &)>;
} // namespace window
