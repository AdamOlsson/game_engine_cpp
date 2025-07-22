#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include <cstddef>

class DescriptorSet {
    friend class DescriptorSetBuilder;

  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    size_t m_capacity;
    size_t m_next;
    std::vector<VkDescriptorSet> m_descriptor_sets;

    DescriptorSet(std::shared_ptr<CoreGraphicsContext> ctx,
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
    VkDescriptorSetLayout *m_descriptor_set_layout;
    DescriptorPool *m_descriptor_pool;
    size_t m_capacity;

    size_t m_uniform_buffer_binding;
    std::vector<UniformBuffer> *m_uniform_buffers;

    std::vector<size_t> m_instance_buffer_binding;
    std::vector<StorageBufferRef> m_instance_buffers;

    size_t m_texture_binding;
    Texture *m_texture;
    Sampler *m_sampler;

    VkWriteDescriptorSet
    create_instance_buffer_descriptor_write(const VkDescriptorSet &dst_descriptor_set,
                                            const VkDescriptorBufferInfo &buffer_info,
                                            const size_t binding_num);

  public:
    DescriptorSetBuilder(VkDescriptorSetLayout &descriptor_set_layout,
                         DescriptorPool &descriptor_pool, size_t capacity);

    DescriptorSetBuilder &
    add_storage_buffers(size_t binding, std::vector<StorageBufferRef> &&instance_buffers);

    DescriptorSetBuilder &set_texture_and_sampler(size_t binding, Texture &texture,
                                                  Sampler &sampler);

    DescriptorSetBuilder &
    set_uniform_buffers(size_t binding, std::vector<UniformBuffer> &uniform_buffers);

    DescriptorSet build(std::shared_ptr<CoreGraphicsContext> &ctx);

    std::vector<VkDescriptorSet>
    allocate_descriptor_sets(std::shared_ptr<CoreGraphicsContext> &ctx);

    VkWriteDescriptorSet
    create_uniform_buffer_descriptor_write(const VkDescriptorSet &dst_descriptor_set,
                                           const VkDescriptorBufferInfo &buffer_info);

    VkWriteDescriptorSet
    create_texture_and_sampler_descriptor_write(const VkDescriptorSet &dst_descriptor_set,
                                                VkDescriptorImageInfo &image_info);
};
