#pragma once
#include "ViewportPoint.h"
#include <functional>

namespace window {
enum class MouseEvent : uint16_t {
    CURSOR_MOVED = 0x1,
    LEFT_BUTTON_DOWN = 0x2,
    LEFT_BUTTON_UP = 0x3,
    RIGHT_BUTTON_DOWN = 0x4,
    RIGHT_BUTTON_UP = 0x5,
    SCROLL = 0x6,
};

inline std::ostream &operator<<(std::ostream &os, MouseEvent type) {
    switch (type) {
    case MouseEvent::CURSOR_MOVED:
        return os << "MouseEvent::CURSOR_MOVED";
    case MouseEvent::LEFT_BUTTON_DOWN:
        return os << "MouseEvent::LEFT_BUTTON_DOWN";
    case MouseEvent::LEFT_BUTTON_UP:
        return os << "MouseEvent::LEFT_BUTTON_UP";
    case MouseEvent::RIGHT_BUTTON_DOWN:
        return os << "MouseEvent::RIGHT_BUTTON_DOWN";
    case MouseEvent::RIGHT_BUTTON_UP:
        return os << "MouseEvent::RIGHT_BUTTON_UP";
    case MouseEvent::SCROLL:
        return os << "MouseEvent::SCROLL";
    }
}
using MouseEventCallbackFn = std::function<void(MouseEvent, ViewportPoint &)>;
} // namespace window
