#pragma once

#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
class DescriptorSetLayout {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    VkDescriptorSetLayout m_layout;

  public:
    DescriptorSetLayout() = default;
    DescriptorSetLayout(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                        VkDescriptorSetLayout &layout);

    ~DescriptorSetLayout();

    DescriptorSetLayout(DescriptorSetLayout &&) noexcept;
    DescriptorSetLayout &operator=(DescriptorSetLayout &&) noexcept;
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    operator VkDescriptorSetLayout() const { return m_layout; }
};
