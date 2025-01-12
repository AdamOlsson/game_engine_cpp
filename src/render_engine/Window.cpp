#include "render_engine/Window.h"
#include <cstdint>
#include <tuple>

Window::Window(const uint32_t width, const uint32_t height, char const *window_title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);
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
        auto p = ViewportPoint(xpos, ypos);
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
