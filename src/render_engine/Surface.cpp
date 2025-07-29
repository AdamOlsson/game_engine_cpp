#include "Surface.h"

Surface::Surface(const Instance *instance, const window::Window *window)
    : m_instance(instance), surface(create_surface(instance, window)) {}

Surface::~Surface() { vkDestroySurfaceKHR(*m_instance, surface, nullptr); }

VkSurfaceKHR Surface::create_surface(const Instance *instance,
                                     const window::Window *window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*instance, *window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}
