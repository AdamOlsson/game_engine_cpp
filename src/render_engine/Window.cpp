#include "render_engine/Window.h"

bool Window::should_window_close() { return glfwWindowShouldClose(this->window); }

void Window::process_window_events() { glfwPollEvents(); }

void Window::register_mouse_event_callback(std::function<void()> cb) {
    this->mouse_event_cb = cb;
    glfwSetMouseButtonCallback(this->window, this->mouse_button_callback);
    glfwSetWindowUserPointer(this->window,
                             this); // This should preferably be set elsewhere
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
            w->mouse_event_cb.value()();
        }
    }
}