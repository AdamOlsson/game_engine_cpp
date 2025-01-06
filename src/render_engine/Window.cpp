#include "render_engine/Window.h"

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

void Window::register_mouse_event_callback(std::function<void(double, double)> cb) {
    this->mouse_event_cb = cb;
    glfwSetMouseButtonCallback(this->window, this->mouse_button_callback);
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

/* ######################################### */
/* ---------! PRIVATE FUNCTIONS !------------*/
/* ######################################### */

void Window::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    if (action == GLFW_PRESS) {
        auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (w->mouse_event_cb.has_value()) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            /*int width, height;*/
            /*glfwGetWindowSize(window, &width, &height);*/
            /*xpos = xpos / width;*/
            /*ypos = ypos / height;*/
            w->mouse_event_cb.value()(xpos, ypos);
        }
    }
}
