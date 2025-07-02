#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/Pipeline.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "render_engine/ui/ElementProperties.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace ui {

class UIPipeline {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    VkDescriptorSetLayout m_descriptor_set_layout;
    DescriptorPool m_descriptor_pool;
    DescriptorSet m_descriptor_set;
    Pipeline m_pipeline;

    VertexBuffer m_vertex_buffer; // Not used, but there to prevent Vulkan warnings

    VkDescriptorSetLayout create_descriptor_set_layout();
    DescriptorSet create_descriptor_set(std::vector<UniformBuffer> &uniform_buffers);

    Pipeline create_pipeline(SwapChainManager &swap_chain_manager);

  public:
    UIPipeline(std::shared_ptr<CoreGraphicsContext> ctx,
               SwapChainManager &swap_chain_manager,
               std::vector<UniformBuffer> &uniform_buffers);
    ~UIPipeline();

    void render(const VkCommandBuffer &command_buffer,
                const ui::ElementProperties::ContainerProperties &ui_element);
};
} // namespace ui
