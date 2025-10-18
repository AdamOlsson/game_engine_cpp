#pragma once

#include "DescriptorSet.h"
#include "game_engine_sdk/render_engine/descriptors/DescriptorSetLayoutBuilder.h"
#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorBufferInfo.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorImageInfo.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorPool.h"
#include "vulkan/vulkan_core.h"

struct SwapDescriptorSetBuilderOptions {
    VkShaderStageFlags stage_flags = VK_SHADER_STAGE_VERTEX_BIT;
};

class SwapDescriptorSetBuilder {
  private:
    DescriptorSetLayoutBuilder m_descriptor_set_layout_builder;

    size_t m_capacity;

    std::vector<size_t> m_storage_buffer_bindings;
    std::vector<vulkan::DescriptorBufferInfo> m_storage_buffers_infos;

    std::vector<size_t> m_uniform_buffer_bindings;
    std::vector<vulkan::DescriptorBufferInfo> m_uniform_buffers_infos;

    std::vector<VkDescriptorImageInfo> m_combined_image_sampler_infos;
    size_t m_combined_image_sampler_binding;

    std::vector<VkDescriptorSet>
    allocate_descriptor_sets(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                             vulkan::DescriptorPool &descriptor_pool,
                             const VkDescriptorSetLayout &descriptor_set_layout);

    VkWriteDescriptorSet create_buffer_descriptor_write(
        const VkDescriptorType type, const VkDescriptorSet &dst_descriptor_set,
        const VkDescriptorBufferInfo *buffer_info, const size_t binding_num);

    VkWriteDescriptorSet
    create_texture_and_sampler_descriptor_write(const VkDescriptorSet &dst_descriptor_set,
                                                const VkDescriptorImageInfo *image_infos,
                                                const size_t num_image_infos,
                                                const size_t binding_num);

  public:
    SwapDescriptorSetBuilder(size_t capacity);

    SwapDescriptorSetBuilder &
    add_combined_image_sampler(size_t binding,
                               std::vector<vulkan::DescriptorImageInfo> &image_info);

    SwapDescriptorSetBuilder &
    add_storage_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos);

    SwapDescriptorSetBuilder &
    add_storage_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos,
                       SwapDescriptorSetBuilderOptions &&options);

    SwapDescriptorSetBuilder &
    add_uniform_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos);

    SwapDescriptorSetBuilder &
    add_uniform_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos,
                       SwapDescriptorSetBuilderOptions &&options);

    SwapDescriptorSet build(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                            vulkan::DescriptorPool &descriptor_pool);
};
