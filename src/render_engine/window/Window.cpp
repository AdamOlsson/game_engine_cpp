#include "render_engine/window/Window.h"
#include "render_engine/window/WindowConfig.h"
using namespace window;

Window::Window(const WindowConfig &config) : m_config(config) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(config.dims.width, config.dims.height, config.title,
                                nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::should_window_close() { return glfwWindowShouldClose(m_window); }

void Window::process_window_events() { glfwPollEvents(); }

bool Window::is_minimized() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    glfwWaitEvents();
    return width == 0 || height == 0;
}

void Window::register_mouse_event_callback(MouseEventCallbackFn cb) {
    this->mouse_event_cb = cb;
    glfwSetMouseButtonCallback(m_window, this->mouse_button_callback);
    glfwSetCursorPosCallback(m_window, this->cursor_position_callback);
}

void Window::register_keyboard_event_callback(KeyboardEventCallbackFn cb) {
    this->keyboard_event_cb = cb;
    glfwSetKeyCallback(m_window, this->keyboard_callback);
}

/* ######################################### */
/* ---------! PRIVATE FUNCTIONS !------------*/
/* ######################################### */

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    if (w->mouse_event_cb.has_value()) {
        WindowDimension dims = w->m_config.dims;
        auto p =
            ViewportPoint(xpos - dims.width / 2.0f, -1.0f * (ypos - dims.height / 2.0f));
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
    if (w != nullptr && w->mouse_event_cb.has_value()) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        WindowDimension dims = w->m_config.dims;
        auto p =
            ViewportPoint(xpos - dims.width / 2.0f, -1.0f * (ypos - dims.height / 2.0f));
        w->mouse_event_cb.value()(m_event, p);
    }
}

void Window::keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                               int mods) {

    auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    if (w != nullptr && w->keyboard_event_cb.has_value()) {
        KeyState state = static_cast<KeyState>(action);
        KeyEvent key_event = static_cast<KeyEvent>(key);
        w->keyboard_event_cb.value()(key_event, state);
    }
}
