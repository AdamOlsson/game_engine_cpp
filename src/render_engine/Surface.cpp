#include "Surface.h"

Surface::Surface(Instance &instance, window::Window &window)
    : m_instance(&instance), surface(create_surface(instance, window)) {}

Surface::~Surface() { vkDestroySurfaceKHR(m_instance->instance, surface, nullptr); }

VkSurfaceKHR Surface::create_surface(Instance &instance, window::Window &window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance.instance, window.window, nullptr, &surface) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}
