#pragma once

#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayoutBuilder.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/vulkan/Sampler.h"

class DescriptorSetBuilder {
  private:
    DescriptorSetLayoutBuilder m_descriptor_set_layout_builder;

    size_t m_capacity;

    std::vector<size_t> m_gpu_buffer_binding;
    std::vector<GpuBufferRef> m_gpu_buffers;

    size_t m_texture_binding;
    Texture *m_texture;
    vulkan::Sampler *m_sampler;

    std::vector<VkDescriptorSet>
    allocate_descriptor_sets(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                             DescriptorPool &descriptor_pool,
                             const VkDescriptorSetLayout &descriptor_set_layout);

    VkWriteDescriptorSet create_buffer_descriptor_write(
        const VkDescriptorType type, const VkDescriptorSet &dst_descriptor_set,
        const VkDescriptorBufferInfo &buffer_info, const size_t binding_num);

    VkWriteDescriptorSet
    create_texture_and_sampler_descriptor_write(const VkDescriptorSet &dst_descriptor_set,
                                                VkDescriptorImageInfo &image_info);

  public:
    DescriptorSetBuilder(size_t capacity);

    DescriptorSetBuilder &add_gpu_buffer(size_t binding,
                                         std::vector<GpuBufferRef> &&buffers);

    DescriptorSetBuilder &set_texture_and_sampler(size_t binding, Texture *texture,
                                                  vulkan::Sampler *sampler);

    DescriptorSet build(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                        DescriptorPool &descriptor_pool);
};
