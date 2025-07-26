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
};
