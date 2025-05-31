#include "render_engine/Window.h"
#include "render_engine/WindowConfig.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <tuple>

Window::Window(const WindowConfig &config) : m_config(config) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(config.dims.width, config.dims.height, config.title,
                                    nullptr, nullptr);
}

Window::~Window() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

bool Window::should_window_close() { return glfwWindowShouldClose(this->window); }

void Window::process_window_events() { glfwPollEvents(); }

void Window::register_mouse_event_callback(MouseEventCallbackFn cb) {
    this->mouse_event_cb = cb;
    glfwSetMouseButtonCallback(this->window, this->mouse_button_callback);
    glfwSetCursorPosCallback(this->window, this->cursor_position_callback);
    glfwSetWindowUserPointer(this->window,
                             this); // This should preferably be set elsewhere
}

void Window::register_keyboard_event_callback(KeyboardEventCallbackFn cb) {
    this->keyboard_event_cb = cb;
    glfwSetKeyCallback(this->window, this->keyboard_callback);
}

VkSurfaceKHR Window::createSurface(VkInstance *instance, GLFWwindow &window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*instance, &window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

std::tuple<uint32_t, uint32_t> Window::dimensions() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return std::make_tuple(width, height);
}

/* ######################################### */
/* ---------! PRIVATE FUNCTIONS !------------*/
/* ######################################### */

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    if (w->mouse_event_cb.has_value()) {
        WindowDimensions dims = w->m_config.dims;
        auto p = ViewportPoint(xpos - dims.width / 2.0f, ypos - dims.height / 2.0f);
        w->mouse_event_cb.value()(MouseEvent::CURSOR_MOVED, p);
    }
}

void Window::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    MouseEvent m_event;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        switch (action) {
        case GLFW_PRESS:
            m_event = MouseEvent::LEFT_BUTTON_DOWN;
            break;
        case GLFW_RELEASE:
            m_event = MouseEvent::LEFT_BUTTON_UP;
            break;
        default:
            return;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        switch (action) {
        case GLFW_PRESS:
            m_event = MouseEvent::RIGHT_BUTTON_DOWN;
            break;
        case GLFW_RELEASE:
            m_event = MouseEvent::RIGHT_BUTTON_UP;
            break;
        default:
            return;
        }
    }

    auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    if (w->mouse_event_cb.has_value()) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        auto p = ViewportPoint(xpos, ypos);
        w->mouse_event_cb.value()(m_event, p);
    }
}

void Window::keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                               int mods) {

    auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    if (w->keyboard_event_cb.has_value()) {
        KeyState state;
        switch (action) {
        case GLFW_PRESS:
            state = KeyState::DOWN;
            break;
        case GLFW_RELEASE:
            state = KeyState::UP;
            break;
        case GLFW_REPEAT:
            state = KeyState::REPEAT;
            break;
        default:
            return;
        }

        KeyEvent key_event;
        switch (key) {
        case GLFW_KEY_R:
            key_event = KeyEvent::R;
            break;
        case GLFW_KEY_T:
            key_event = KeyEvent::T;
            break;
        default:
            return;
        }

        w->keyboard_event_cb.value()(key_event, state);
    }
}

std::ostream &operator<<(std::ostream &os, const KeyEvent &k) {
    std::string key_string;
    switch (k) {
    case KeyEvent::R:
        key_string = "R";
        break;
    case KeyEvent::T:
        key_string = "T";
        break;
    default:
        key_string = "UNKNOWN";
        break;
    }
    return os << "KeyEvent::" << key_string;
}

std::ostream &operator<<(std::ostream &os, const KeyState &k) {
    std::string state_string;
    switch (k) {
    case KeyState::UP:
        state_string = "UP";
        break;
    case KeyState::REPEAT:
        state_string = "REPEAT";
        break;
    case KeyState::DOWN:
        state_string = "DOWN";
        break;
    default:
        state_string = "UNKNOWN";
        break;
    }
    return os << "KeyEvent::" << state_string;
}
