#include "window/Window.h"
#include "window/WindowConfig.h"

using namespace window;

Window::Window(const WindowConfig &config) : m_config(config) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(config.dims.width, config.dims.height, config.title,
                                nullptr, nullptr);

    glfwSetWindowUserPointer(m_window, this);

    // High dpi: Window framebuffer size: (1600, 1600)
    // Low dpi: Window framebuffer size: (800, 800)
    glfwGetFramebufferSize(m_window, &m_frame_buffer.width, &m_frame_buffer.height);
    glfwGetWindowSize(m_window, &m_window_size.width, &m_window_size.height);
    m_px_per_coord.x = static_cast<double>(m_frame_buffer.width) / m_window_size.width;
    m_px_per_coord.y = static_cast<double>(m_frame_buffer.height) / m_window_size.height;
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
    glfwSetScrollCallback(m_window, this->mouse_scroll_callback);
}

void Window::register_keyboard_event_callback(KeyboardEventCallbackFn cb) {
    this->keyboard_event_cb = cb;
    glfwSetKeyCallback(m_window, this->keyboard_callback);
}

/* ######################################### */
/* ---------! PRIVATE FUNCTIONS !------------*/
/* ######################################### */

void Window::mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    if (w->mouse_event_cb.has_value()) {
        auto scroll_offset = interface::ViewportPoint(xoffset, yoffset);
        w->mouse_event_cb.value()(MouseEvent::SCROLL, scroll_offset);
    }
}

interface::ViewportPoint Window::to_viewport(const Window *w, const double xpos,
                                             const double ypos) {
    const double half_width = w->m_window_size.width / 2.0;
    const double half_height = w->m_window_size.height / 2.0;
    return interface::ViewportPoint((xpos - half_width) * w->m_px_per_coord.x,
                                    (ypos - half_height) * w->m_px_per_coord.y);
}

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    if (w->mouse_event_cb.has_value()) {
        auto p = to_viewport(w, xpos, ypos);
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
        auto p = to_viewport(w, xpos, ypos);
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
