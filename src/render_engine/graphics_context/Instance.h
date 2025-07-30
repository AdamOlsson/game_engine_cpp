#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>

namespace graphics_context {
class Instance {
  private:
    bool m_enable_validation_layers;
    VkInstance m_instance;

    VkInstance create_instance();
    std::vector<const char *> get_required_extensions();

    void print_enabled_extensions();

  public:
    Instance(bool enable_validation_layers);
    ~Instance();

    Instance(Instance &&) noexcept = default;
    Instance &operator=(Instance &&) noexcept = default;
    Instance(const Instance &) = delete;
    Instance &operator=(const Instance &) = delete;

    operator VkInstance() const { return m_instance; }
};

} // namespace graphics_context
