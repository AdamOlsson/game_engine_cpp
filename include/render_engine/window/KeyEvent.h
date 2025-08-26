#pragma once

#include <ostream>
namespace window {

enum class KeyEvent { W, A, S, D, R, T };
enum class KeyState { DOWN, REPEAT, UP };

// clang-format off
inline std::ostream &operator<<(std::ostream &os, KeyEvent key) {
    switch (key) {
    case KeyEvent::W: return os << "W";
    case KeyEvent::A: return os << "A";
    case KeyEvent::S: return os << "S";
    case KeyEvent::D: return os << "D";
    case KeyEvent::R: return os << "R";
    case KeyEvent::T: return os << "T";
    }
    return os << "Unknown";
}

inline std::ostream &operator<<(std::ostream &os, const KeyState &state) {
    switch (state) {
    case KeyState::DOWN: return os << "DOWN";
    case KeyState::REPEAT: return os << "REPEAT";
    case KeyState::UP: return os << "UP";
    }
    return os << "Unknown";
}
// clang-format on

using KeyboardEventCallbackFn = std::function<void(KeyEvent &, KeyState &)>;
} // namespace window
