#pragma once

#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayoutBuilder.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/vulkan/DescriptorBufferInfo.h"
#include "render_engine/vulkan/DescriptorImageInfo.h"
#include "vulkan/vulkan_core.h"

struct DescriptorSetBuilderOptions {
    VkShaderStageFlags stage_flags = VK_SHADER_STAGE_VERTEX_BIT;
};

class DescriptorSetBuilder {
  private:
    DescriptorSetLayoutBuilder m_descriptor_set_layout_builder;

    size_t m_capacity;

    std::vector<size_t> m_storage_buffer_bindings;
    std::vector<vulkan::DescriptorBufferInfo> m_storage_buffers_infos;

    std::vector<size_t> m_uniform_buffer_bindings;
    std::vector<vulkan::DescriptorBufferInfo> m_uniform_buffers_infos;

    std::optional<vulkan::DescriptorImageInfo> m_combined_image_sampler_info;
    std::optional<size_t> m_combined_image_sampler_binding;

    std::vector<VkDescriptorSet>
    allocate_descriptor_sets(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                             DescriptorPool &descriptor_pool,
                             const VkDescriptorSetLayout &descriptor_set_layout);

    VkWriteDescriptorSet create_buffer_descriptor_write(
        const VkDescriptorType type, const VkDescriptorSet &dst_descriptor_set,
        const VkDescriptorBufferInfo *buffer_info, const size_t binding_num);

    VkWriteDescriptorSet
    create_texture_and_sampler_descriptor_write(const VkDescriptorSet &dst_descriptor_set,
                                                const VkDescriptorImageInfo *image_info,
                                                const size_t binding_num);

  public:
    DescriptorSetBuilder(size_t capacity);

    DescriptorSetBuilder &
    add_combined_image_sampler(size_t binding, vulkan::DescriptorImageInfo &image_info);

    DescriptorSetBuilder &
    add_storage_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos);

    DescriptorSetBuilder &
    add_storage_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos,
                       DescriptorSetBuilderOptions &&options);

    DescriptorSetBuilder &
    add_uniform_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos);

    DescriptorSetBuilder &
    add_uniform_buffer(size_t binding,
                       std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos,
                       DescriptorSetBuilderOptions &&options);

    DescriptorSet build(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                        DescriptorPool &descriptor_pool);
};
