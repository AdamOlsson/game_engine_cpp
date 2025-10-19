#pragma once

#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSet.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorBufferInfo.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorImageInfo.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorPool.h"

namespace graphics_pipeline {
struct QuadPipelineDescriptorSetOpts {
    unsigned int num_sets = 2;
    std::vector<vulkan::DescriptorBufferInfo> storage_buffer_refs = {};
    std::vector<vulkan::DescriptorBufferInfo> uniform_buffer_refs = {};
    std::vector<vulkan::DescriptorImageInfo> combined_image_sampler_infos = {};
};

class QuadPipelineDescriptorSet {
  private:
    SwapDescriptorSet m_descriptor_set;

    SwapDescriptorSet
    build_descriptor_set(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                         vulkan::DescriptorPool &descriptor_pool,
                         QuadPipelineDescriptorSetOpts &&opts);

  public:
    QuadPipelineDescriptorSet(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                              vulkan::DescriptorPool &descriptor_pool,
                              QuadPipelineDescriptorSetOpts &&opts);

    vulkan::DescriptorSetLayout &get_layout();
    SwapDescriptorSet *handle();
    vulkan::DescriptorSet get_next();
};

} // namespace graphics_pipeline
