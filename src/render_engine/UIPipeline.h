#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Pipeline.h"
#include "render_engine/SwapChainManager.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class UIPipeline {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    VkDescriptorSetLayout m_descriptor_set_layout;
    Pipeline m_pipeline;

    VkDescriptorSetLayout create_descriptor_set_layout();
    Pipeline create_pipeline(SwapChainManager &swap_chain_manager);

  public:
    UIPipeline(std::shared_ptr<CoreGraphicsContext> ctx,
               SwapChainManager &swap_chain_manager);
    ~UIPipeline();

    void render(const VkCommandBuffer &command_buffer);
};
