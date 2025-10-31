#pragma once

#include "context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
namespace vulkan {
class DescriptorSetLayout {
  private:
    std::shared_ptr<context::GraphicsContext> m_ctx;
    VkDescriptorSetLayout m_layout;

  public:
    DescriptorSetLayout() = default;
    DescriptorSetLayout(std::shared_ptr<context::GraphicsContext> ctx,
                        VkDescriptorSetLayout &layout);

    ~DescriptorSetLayout();

    DescriptorSetLayout(DescriptorSetLayout &&) noexcept;
    DescriptorSetLayout &operator=(DescriptorSetLayout &&) noexcept;
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    operator VkDescriptorSetLayout() const { return m_layout; }

    const VkDescriptorSetLayout *get_handle() const { return &m_layout; }
};

} // namespace vulkan
