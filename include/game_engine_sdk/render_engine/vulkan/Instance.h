#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>

namespace vulkan {
class Instance {
  private:
    bool m_enable_validation_layers;
    VkInstance m_instance;

    VkInstance create_instance(const std::vector<const char *> &validation_layers);
    std::vector<const char *> get_required_extensions();

    bool
    check_validation_layer_support(const std::vector<const char *> &validation_layers);

    void print_vulkan_version();
    void print_supported_extensions();
    void print_enabled_extensions(const std::vector<const char *> &extensions);

  public:
    Instance(const std::vector<const char *> &validation_layers);
    ~Instance();

    Instance(Instance &&) noexcept = default;
    Instance &operator=(Instance &&) noexcept = default;
    Instance(const Instance &) = delete;
    Instance &operator=(const Instance &) = delete;

    operator VkInstance() const { return m_instance; }
};

} // namespace vulkan
