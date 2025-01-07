#pragma once

#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <functional>
#include <optional>

/**
 * @brief Wrapper class for GLFW window api.
 */
class Window {
  public:
    GLFWwindow *window;
    Window(const uint32_t width, const uint32_t height, char const *window_title);

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
    void register_mouse_event_callback(std::function<void(double, double)> cb);

    VkSurfaceKHR createSurface(VkInstance *instance, GLFWwindow &window);

    std::tuple<uint32_t, uint32_t> dimensions();

  private:
    std::optional<std::function<void(double, double)>> mouse_event_cb;

    /**
     * @brief Callback function whoms only purpose is to call the user registered function
     * mouse_event_cb;
     */
    static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                      int mods);
};
