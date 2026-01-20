#pragma once

#include "graphics_pipeline/SwapDescriptorSet.h"
#include "vulkan/DescriptorBufferInfo.h"
#include "vulkan/DescriptorImageInfo.h"
#include "vulkan/DescriptorPool.h"

namespace graphics_pipeline::text {
struct TextPipelineDescriptorSetOpts {
    unsigned int num_sets = 2;
    std::vector<vulkan::DescriptorBufferInfo> storage_buffer_refs = {};
    std::vector<vulkan::DescriptorBufferInfo> uniform_buffer_refs = {};
    std::vector<vulkan::DescriptorImageInfo> combined_image_sampler_infos = {};
};

class TextPipelineDescriptorSet {
  private:
    SwapDescriptorSet m_descriptor_set;

    SwapDescriptorSet
    build_descriptor_set(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                         vulkan::DescriptorPool &descriptor_pool,
                         TextPipelineDescriptorSetOpts &&opts);

  public:
    TextPipelineDescriptorSet(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                              vulkan::DescriptorPool &descriptor_pool,
                              TextPipelineDescriptorSetOpts &&opts);

    vulkan::DescriptorSetLayout &get_layout();
    SwapDescriptorSet *handle();
    vulkan::DescriptorSet get_next();
    vulkan::DescriptorSet get_current();
    void rotate();
};

} // namespace graphics_pipeline::text
