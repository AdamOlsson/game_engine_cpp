#pragma once

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
    Window(const uint32_t width, const uint32_t height, char const *window_title) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        this->window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);
    }

    ~Window() {
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }

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
    void register_mouse_event_callback(std::function<void()> cb);

    VkSurfaceKHR createSurface(VkInstance *instance, GLFWwindow &window) {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(*instance, &window, nullptr, &surface) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        return surface;
    }

  private:
    std::optional<std::function<void()>> mouse_event_cb;

    /**
     * @brief Callback function whoms only purpose is to call the user registered function
     * mouse_event_cb;
     */
    static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                      int mods);
};
