#include "Surface.h"
#include "vulkan/vulkan_core.h"

graphics_context::Surface::Surface(const Instance *instance, const window::Window *window)
    : m_instance(instance), m_surface(create_surface(instance, window)) {}

graphics_context::Surface::~Surface() {
    if (m_surface == VK_NULL_HANDLE) {
        return;
    }
    vkDestroySurfaceKHR(*m_instance, m_surface, nullptr);
}

VkSurfaceKHR graphics_context::Surface::create_surface(const Instance *instance,
                                                       const window::Window *window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*instance, *window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

graphics_context::Surface::Surface(Surface &&other) noexcept
    : m_instance(other.m_instance), m_surface(other.m_surface) {
    other.m_instance = nullptr;
    other.m_surface = VK_NULL_HANDLE;
}

graphics_context::Surface &
graphics_context::Surface::operator=(Surface &&other) noexcept {

    if (this != &other) {
        if (m_surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(*m_instance, m_surface, nullptr);
        }
        m_instance = other.m_instance;
        m_surface = other.m_surface;

        other.m_instance = nullptr;
        other.m_surface = VK_NULL_HANDLE;
    }
    return *this;
}
