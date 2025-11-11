#pragma once
#include "ViewportPoint.h"
#include <functional>

namespace window {
enum class MouseEvent {
    CURSOR_MOVED,
    LEFT_BUTTON_DOWN,
    LEFT_BUTTON_UP,
    RIGHT_BUTTON_DOWN,
    RIGHT_BUTTON_UP,
    SCROLL,
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
