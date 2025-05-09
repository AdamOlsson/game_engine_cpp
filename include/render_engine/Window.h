#pragma once

#include "Coordinates.h"
#include "render_engine/WindowConfig.h"
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <functional>
#include <optional>
#include <ostream>

enum class MouseEvent {
    CURSOR_MOVED,
    LEFT_BUTTON_DOWN,
    LEFT_BUTTON_UP,
    RIGHT_BUTTON_DOWN,
    RIGHT_BUTTON_UP
};

enum class KeyEvent {
    R,
    T,
};

enum class KeyState { DOWN, REPEAT, UP };

std::ostream &operator<<(std::ostream &os, const KeyEvent &k);
std::ostream &operator<<(std::ostream &os, const KeyState &k);

using MouseEventCallbackFn = std::function<void(MouseEvent, ViewportPoint &)>;
using KeyboardEventCallbackFn = std::function<void(KeyEvent &, KeyState &)>;

/**
 * @brief Wrapper class for GLFW window api.
 */
class Window {
  public:
    GLFWwindow *window;
    Window(const WindowConfig &window_dims);

    ~Window();
    /**
     * @brief Processes any events GLFW has queued. This function triggers any callback
     * registered.
     */
    void process_window_events();
    bool should_window_close();

    /**
     * @brief Register a callback function for mouse input events.
     * @param cb The callback function which is triggered after a mouse input event.
     */
    void register_mouse_event_callback(MouseEventCallbackFn cb);

    void register_keyboard_event_callback(KeyboardEventCallbackFn cb);

    VkSurfaceKHR createSurface(VkInstance *instance, GLFWwindow &window);

    std::tuple<uint32_t, uint32_t> dimensions();

  private:
    std::optional<MouseEventCallbackFn> mouse_event_cb;
    std::optional<KeyboardEventCallbackFn> keyboard_event_cb;

    static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                      int mods);
    static void keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                                  int mods);
};
