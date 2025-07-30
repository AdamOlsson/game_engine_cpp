#include "Surface.h"

graphics_context::Surface::Surface(const Instance *instance, const window::Window *window)
    : m_instance(instance), surface(create_surface(instance, window)) {}

graphics_context::Surface::~Surface() {
    vkDestroySurfaceKHR(*m_instance, surface, nullptr);
}

VkSurfaceKHR graphics_context::Surface::create_surface(const Instance *instance,
                                                       const window::Window *window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*instance, *window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}
