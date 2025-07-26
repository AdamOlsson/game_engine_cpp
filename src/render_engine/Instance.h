#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>
class Instance {
  private:
    bool m_enable_validation_layers;

    VkInstance create_instance();
    std::vector<const char *> get_required_extensions();

    void print_enabled_extensions();

  public:
    VkInstance instance;

    Instance(bool enable_validation_layers);
    ~Instance();

    Instance(Instance &&) noexcept = default;
    Instance &operator=(Instance &&) noexcept = default;
    Instance(const Instance &) = delete;
    Instance &operator=(const Instance &) = delete;
};
