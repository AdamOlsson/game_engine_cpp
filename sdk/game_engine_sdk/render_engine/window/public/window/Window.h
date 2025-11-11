#pragma once

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "WindowConfig.h"
#include "WindowDimension.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>

namespace window {

/**
 * @brief Wrapper class for GLFW window api.
 */
class Window {
  private:
    WindowConfig m_config;
    GLFWwindow *m_window;

  public:
    Window(const WindowConfig &window_dims);

    ~Window();
    /**
     * @brief Processes any events GLFW has queued. This function triggers any callback
     * registered.
     */
    void process_window_events();
    bool should_window_close();

    bool is_minimized();

    /**
     * @brief Register a callback function for mouse input events.
     * @param cb The callback function which is triggered after a mouse input event.
     */
    void register_mouse_event_callback(MouseEventCallbackFn cb);

    void register_keyboard_event_callback(KeyboardEventCallbackFn cb);

    template <typename T> WindowDimension<T> dimensions() {
        int width, height;
        glfwGetWindowSize(m_window, &width, &height);
        return WindowDimension<T>(static_cast<T>(width), static_cast<T>(height));
    }

    template <typename T> WindowDimension<T> get_framebuffer_size() const {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return WindowDimension<T>(static_cast<T>(width), static_cast<T>(height));
    }

    operator GLFWwindow *() const { return m_window; }

  private:
    std::optional<MouseEventCallbackFn> mouse_event_cb;
    std::optional<KeyboardEventCallbackFn> keyboard_event_cb;

    static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
    static void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                      int mods);
    static void keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                                  int mods);
};

} // namespace window
