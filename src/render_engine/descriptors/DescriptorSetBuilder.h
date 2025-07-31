#pragma once

#include "DescriptorPool.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/descriptors/DescriptorSet.h"
#include "render_engine/graphics_context/GraphicsContext.h"

class DescriptorSetBuilder {
  private:
    const VkDescriptorSetLayout m_descriptor_set_layout;
    DescriptorPool *m_descriptor_pool;
    size_t m_capacity;

    size_t m_uniform_buffer_binding;
    std::vector<GpuBufferRef> m_uniform_buffers;

    std::vector<size_t> m_storage_buffer_binding;
    std::vector<GpuBufferRef> m_storage_buffers;

    size_t m_texture_binding;
    Texture *m_texture;
    Sampler *m_sampler;

    std::vector<VkDescriptorSet>
    allocate_descriptor_sets(std::shared_ptr<graphics_context::GraphicsContext> &ctx);

    VkWriteDescriptorSet create_buffer_descriptor_write(
        const VkDescriptorType type, const VkDescriptorSet &dst_descriptor_set,
        const VkDescriptorBufferInfo &buffer_info, const size_t binding_num);

    VkWriteDescriptorSet
    create_texture_and_sampler_descriptor_write(const VkDescriptorSet &dst_descriptor_set,
                                                VkDescriptorImageInfo &image_info);

  public:
    DescriptorSetBuilder(const VkDescriptorSetLayout &descriptor_set_layout,
                         DescriptorPool &descriptor_pool, size_t capacity);

    DescriptorSetBuilder &add_gpu_buffer(size_t binding,
                                         std::vector<GpuBufferRef> &&buffers);

    DescriptorSetBuilder &set_texture_and_sampler(size_t binding, Texture &texture,
                                                  Sampler &sampler);

    DescriptorSet build(std::shared_ptr<graphics_context::GraphicsContext> &ctx);
};
