#pragma once

#include "Instance.h"
#include "vulkan/vulkan_core.h"
#include "window/Window.h"

namespace vulkan::context {
class Surface {
  private:
    const Instance *m_instance;
    VkSurfaceKHR m_surface;

    VkSurfaceKHR create_surface(const Instance *instance, const window::Window *window);

  public:
    Surface(const Instance *instance, const window::Window *window);
    ~Surface();

    Surface(Surface &&other) noexcept;
    Surface &operator=(Surface &&other) noexcept;
    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;

    operator VkSurfaceKHR() const { return m_surface; }
};
} // namespace vulkan::context
