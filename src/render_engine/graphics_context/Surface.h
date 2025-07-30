#pragma once

#include "Instance.h"
#include "render_engine/window/Window.h"
#include "vulkan/vulkan_core.h"

namespace graphics_context {
class Surface {
  private:
    const graphics_context::Instance *m_instance;
    VkSurfaceKHR create_surface(const graphics_context::Instance *instance,
                                const window::Window *window);

  public:
    VkSurfaceKHR surface;

    Surface(const graphics_context::Instance *instance, const window::Window *window);
    ~Surface();

    Surface(Surface &&) noexcept = default;
    Surface &operator=(Surface &&) noexcept = default;
    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;

    operator VkSurfaceKHR() const { return surface; }
};
} // namespace graphics_context
