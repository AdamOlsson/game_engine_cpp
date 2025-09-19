#pragma once

#include <ostream>
namespace window {

enum class KeyEvent : uint32_t {
    // Letter keys
    W = 87, // GLFW_KEY_W
    A = 65, // GLFW_KEY_A
    S = 83, // GLFW_KEY_S
    D = 68, // GLFW_KEY_D
    R = 82, // GLFW_KEY_R
    T = 84, // GLFW_KEY_T
    H = 72, // GLFW_KEY_H
    J = 74, // GLFW_KEY_J
    K = 75, // GLFW_KEY_K
    L = 76, // GLFW_KEY_L

    // Additional common keys you might want to add:
    Q = 81, // GLFW_KEY_Q
    E = 69, // GLFW_KEY_E
    I = 73, // GLFW_KEY_I
    O = 79, // GLFW_KEY_O
    P = 80, // GLFW_KEY_P
    U = 85, // GLFW_KEY_U
    Y = 89, // GLFW_KEY_Y
    F = 70, // GLFW_KEY_F
    G = 71, // GLFW_KEY_G
    Z = 90, // GLFW_KEY_Z
    X = 88, // GLFW_KEY_X
    C = 67, // GLFW_KEY_C
    V = 86, // GLFW_KEY_V
    B = 66, // GLFW_KEY_B
    N = 78, // GLFW_KEY_N
    M = 77, // GLFW_KEY_M

    // Special keys
    SPACE = 32,       // GLFW_KEY_SPACE
    ENTER = 257,      // GLFW_KEY_ENTER
    TAB = 258,        // GLFW_KEY_TAB
    BACKSPACE = 259,  // GLFW_KEY_BACKSPACE
    DELETE_KEY = 261, // GLFW_KEY_DELETE
    ESCAPE = 256,     // GLFW_KEY_ESCAPE

    // Arrow keys
    RIGHT = 262, // GLFW_KEY_RIGHT
    LEFT = 263,  // GLFW_KEY_LEFT
    DOWN = 264,  // GLFW_KEY_DOWN
    UP = 265,    // GLFW_KEY_UP

    // Function keys
    F1 = 290,  // GLFW_KEY_F1
    F2 = 291,  // GLFW_KEY_F2
    F3 = 292,  // GLFW_KEY_F3
    F4 = 293,  // GLFW_KEY_F4
    F5 = 294,  // GLFW_KEY_F5
    F6 = 295,  // GLFW_KEY_F6
    F7 = 296,  // GLFW_KEY_F7
    F8 = 297,  // GLFW_KEY_F8
    F9 = 298,  // GLFW_KEY_F9
    F10 = 299, // GLFW_KEY_F10
    F11 = 300, // GLFW_KEY_F11
    F12 = 301, // GLFW_KEY_F12

    // Number keys
    KEY_0 = 48, // GLFW_KEY_0
    KEY_1 = 49, // GLFW_KEY_1
    KEY_2 = 50, // GLFW_KEY_2
    KEY_3 = 51, // GLFW_KEY_3
    KEY_4 = 52, // GLFW_KEY_4
    KEY_5 = 53, // GLFW_KEY_5
    KEY_6 = 54, // GLFW_KEY_6
    KEY_7 = 55, // GLFW_KEY_7
    KEY_8 = 56, // GLFW_KEY_8
    KEY_9 = 57, // GLFW_KEY_9

    // Modifier keys
    LEFT_SHIFT = 340,    // GLFW_KEY_LEFT_SHIFT
    RIGHT_SHIFT = 344,   // GLFW_KEY_RIGHT_SHIFT
    LEFT_CONTROL = 341,  // GLFW_KEY_LEFT_CONTROL
    RIGHT_CONTROL = 345, // GLFW_KEY_RIGHT_CONTROL
    LEFT_ALT = 342,      // GLFW_KEY_LEFT_ALT
    RIGHT_ALT = 346,     // GLFW_KEY_RIGHT_ALT

    // Punctuation keys
    SEMICOLON = 59,     // GLFW_KEY_SEMICOLON
    EQUAL = 61,         // GLFW_KEY_EQUAL
    COMMA = 44,         // GLFW_KEY_COMMA
    MINUS = 45,         // GLFW_KEY_MINUS
    PERIOD = 46,        // GLFW_KEY_PERIOD
    SLASH = 47,         // GLFW_KEY_SLASH
    GRAVE_ACCENT = 96,  // GLFW_KEY_GRAVE_ACCENT
    LEFT_BRACKET = 91,  // GLFW_KEY_LEFT_BRACKET
    BACKSLASH = 92,     // GLFW_KEY_BACKSLASH
    RIGHT_BRACKET = 93, // GLFW_KEY_RIGHT_BRACKET
    APOSTROPHE = 39,    // GLFW_KEY_APOSTROPHE
};

enum class KeyState : uint32_t {
    UP = 0,
    DOWN = 1,
    REPEAT = 2,
};

// clang-format off
inline std::ostream &operator<<(std::ostream &os, KeyEvent key) {
    switch (key) {
    // Letter keys
    case KeyEvent::W: return os << "W";
    case KeyEvent::A: return os << "A";
    case KeyEvent::S: return os << "S";
    case KeyEvent::D: return os << "D";
    case KeyEvent::R: return os << "R";
    case KeyEvent::T: return os << "T";
    case KeyEvent::H: return os << "H";
    case KeyEvent::J: return os << "J";
    case KeyEvent::K: return os << "K";
    case KeyEvent::L: return os << "L";
    case KeyEvent::Q: return os << "Q";
    case KeyEvent::E: return os << "E";
    case KeyEvent::I: return os << "I";
    case KeyEvent::O: return os << "O";
    case KeyEvent::P: return os << "P";
    case KeyEvent::U: return os << "U";
    case KeyEvent::Y: return os << "Y";
    case KeyEvent::F: return os << "F";
    case KeyEvent::G: return os << "G";
    case KeyEvent::Z: return os << "Z";
    case KeyEvent::X: return os << "X";
    case KeyEvent::C: return os << "C";
    case KeyEvent::V: return os << "V";
    case KeyEvent::B: return os << "B";
    case KeyEvent::N: return os << "N";
    case KeyEvent::M: return os << "M";
    
    // Special keys
    case KeyEvent::SPACE: return os << "SPACE";
    case KeyEvent::ENTER: return os << "ENTER";
    case KeyEvent::TAB: return os << "TAB";
    case KeyEvent::BACKSPACE: return os << "BACKSPACE";
    case KeyEvent::DELETE_KEY: return os << "DELETE";
    case KeyEvent::ESCAPE: return os << "ESCAPE";
    
    // Arrow keys
    case KeyEvent::RIGHT: return os << "RIGHT";
    case KeyEvent::LEFT: return os << "LEFT";
    case KeyEvent::DOWN: return os << "DOWN";
    case KeyEvent::UP: return os << "UP";
    
    // Function keys
    case KeyEvent::F1: return os << "F1";
    case KeyEvent::F2: return os << "F2";
    case KeyEvent::F3: return os << "F3";
    case KeyEvent::F4: return os << "F4";
    case KeyEvent::F5: return os << "F5";
    case KeyEvent::F6: return os << "F6";
    case KeyEvent::F7: return os << "F7";
    case KeyEvent::F8: return os << "F8";
    case KeyEvent::F9: return os << "F9";
    case KeyEvent::F10: return os << "F10";
    case KeyEvent::F11: return os << "F11";
    case KeyEvent::F12: return os << "F12";
    
    // Number keys
    case KeyEvent::KEY_0: return os << "0";
    case KeyEvent::KEY_1: return os << "1";
    case KeyEvent::KEY_2: return os << "2";
    case KeyEvent::KEY_3: return os << "3";
    case KeyEvent::KEY_4: return os << "4";
    case KeyEvent::KEY_5: return os << "5";
    case KeyEvent::KEY_6: return os << "6";
    case KeyEvent::KEY_7: return os << "7";
    case KeyEvent::KEY_8: return os << "8";
    case KeyEvent::KEY_9: return os << "9";
    
    // Modifier keys
    case KeyEvent::LEFT_SHIFT: return os << "LEFT_SHIFT";
    case KeyEvent::RIGHT_SHIFT: return os << "RIGHT_SHIFT";
    case KeyEvent::LEFT_CONTROL: return os << "LEFT_CONTROL";
    case KeyEvent::RIGHT_CONTROL: return os << "RIGHT_CONTROL";
    case KeyEvent::LEFT_ALT: return os << "LEFT_ALT";
    case KeyEvent::RIGHT_ALT: return os << "RIGHT_ALT";
    
    // Punctuation keys
    case KeyEvent::SEMICOLON: return os << ";";
    case KeyEvent::EQUAL: return os << "=";
    case KeyEvent::COMMA: return os << ",";
    case KeyEvent::MINUS: return os << "-";
    case KeyEvent::PERIOD: return os << ".";
    case KeyEvent::SLASH: return os << "/";
    case KeyEvent::GRAVE_ACCENT: return os << "`";
    case KeyEvent::LEFT_BRACKET: return os << "[";
    case KeyEvent::BACKSLASH: return os << "\\";
    case KeyEvent::RIGHT_BRACKET: return os << "]";
    case KeyEvent::APOSTROPHE: return os << "'";
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
