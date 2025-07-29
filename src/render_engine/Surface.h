#pragma once

#include "render_engine/Instance.h"
#include "render_engine/window/Window.h"
#include "vulkan/vulkan_core.h"

class Surface {
  private:
    Instance *m_instance;
    VkSurfaceKHR create_surface(Instance &instance, window::Window &window);

  public:
    VkSurfaceKHR surface;

    Surface(Instance &instance, window::Window &window);
    ~Surface();

    Surface(Surface &&) noexcept = default;
    Surface &operator=(Surface &&) noexcept = default;
    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;

    operator VkSurfaceKHR() const { return surface; }
};
