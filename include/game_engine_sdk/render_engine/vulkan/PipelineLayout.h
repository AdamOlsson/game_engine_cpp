#pragma once

#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorSetLayout.h"
#include "vulkan/vulkan_core.h"
#include <vector>
namespace vulkan {
class PipelineLayout {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    VkPipelineLayout m_pipeline_layout;

    VkPipelineLayout create_graphics_pipeline_layout(
        const std::optional<vulkan::DescriptorSetLayout> &descriptor_set_layout,
        const std::vector<VkPushConstantRange> &push_constant_range);

    VkPipelineLayout create_graphics_pipeline_layout(
        const vulkan::DescriptorSetLayout *descriptor_set_layout,
        const std::vector<VkPushConstantRange> &push_constant_range);

  public:
    PipelineLayout() = default;

    PipelineLayout(
        std::shared_ptr<graphics_context::GraphicsContext> ctx,
        const std::optional<vulkan::DescriptorSetLayout> &descriptor_set_layout,
        const std::vector<VkPushConstantRange> &push_constant_range);

    // Deprecated
    PipelineLayout(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                   const vulkan::DescriptorSetLayout *descriptor_set_layout,
                   const std::vector<VkPushConstantRange> &push_constant_range);

    ~PipelineLayout();

    PipelineLayout(PipelineLayout &&other) noexcept;
    PipelineLayout &operator=(PipelineLayout &&other) noexcept;

    PipelineLayout(const PipelineLayout &other) = delete;
    PipelineLayout &operator=(const PipelineLayout &other) = delete;

    operator VkPipelineLayout() const { return m_pipeline_layout; }
};
} // namespace vulkan
