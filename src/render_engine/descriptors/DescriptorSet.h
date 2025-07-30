#pragma once

#include "DescriptorPool.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include <cstddef>

class DescriptorSet {
    friend class DescriptorSetBuilder;

  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    size_t m_capacity;
    size_t m_next;
    std::vector<VkDescriptorSet> m_descriptor_sets;
    // std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;

    DescriptorSet(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                  std::vector<VkDescriptorSet> &descriptor_sets);

  public:
    DescriptorSet() = default;

    DescriptorSet(DescriptorSet &&other) noexcept = default;
    DescriptorSet(const DescriptorSet &other) = delete;
    DescriptorSet &operator=(DescriptorSet &&other) noexcept = default;
    DescriptorSet &operator=(const DescriptorSet &other) = delete;

    ~DescriptorSet() = default;

    const VkDescriptorSet get();
};

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

    DescriptorSetBuilder &add_storage_buffer(size_t binding,
                                             std::vector<GpuBufferRef> &&storage_buffers);

    DescriptorSetBuilder &set_texture_and_sampler(size_t binding, Texture &texture,
                                                  Sampler &sampler);

    DescriptorSetBuilder &set_uniform_buffer(size_t binding,
                                             std::vector<GpuBufferRef> &&uniform_buffers);

    DescriptorSet build(std::shared_ptr<graphics_context::GraphicsContext> &ctx);
};
