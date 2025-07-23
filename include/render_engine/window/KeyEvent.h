#pragma once

#include <ostream>
namespace window {
enum class KeyEvent {
    R,
    T,
};

enum class KeyState { DOWN, REPEAT, UP };

std::ostream &operator<<(std::ostream &os, const KeyEvent &k);
std::ostream &operator<<(std::ostream &os, const KeyState &k);

using KeyboardEventCallbackFn = std::function<void(KeyEvent &, KeyState &)>;
} // namespace window
