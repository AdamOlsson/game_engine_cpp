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
                  VkDescriptorSetLayout &descriptor_set_layout,
                  DescriptorPool *descriptor_pool, const int capacity,
                  std::vector<UniformBuffer> *uniform_buffers, Texture *texture,
                  Sampler *sampler);

    std::vector<StorageBuffer> create_instance_buffers();
    std::vector<VkDescriptorSet> create_descriptor_sets(
        VkDescriptorSetLayout &descriptor_set_layout, DescriptorPool *descriptor_pool,
        std::vector<StorageBuffer> &storage_buffers,
        std::vector<UniformBuffer> *uniform_buffers, Texture *texture, Sampler *sampler);

  public:
    // TODO: Make this private, would require some proper handling of updating the
    // instance buffers through API calls. Propably need functions to get the new instance
    // buffer related to the current frame (get()) and current one (current()).
    std::vector<StorageBuffer> instance_buffers;

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
    VkDescriptorSetLayout m_descriptor_set_layout{VK_NULL_HANDLE};
    DescriptorPool *m_descriptor_pool;
    std::vector<UniformBuffer> *m_uniform_buffers;
    Texture *m_texture{nullptr};
    Sampler *m_sampler{nullptr};
    size_t m_capacity{1};

  public:
    DescriptorSetBuilder &
    set_descriptor_set_layout(VkDescriptorSetLayout &descriptor_set_layout);
    DescriptorSetBuilder &set_descriptor_pool(DescriptorPool *descriptor_pool);
    DescriptorSetBuilder &set_capacity(size_t capacity);
    DescriptorSetBuilder &
    set_uniform_buffers(std::vector<UniformBuffer> *uniform_buffers);
    DescriptorSetBuilder &set_texture(Texture *texture);
    DescriptorSetBuilder &set_sampler(Sampler *sampler);

    DescriptorSet build(std::shared_ptr<CoreGraphicsContext> &ctx);
};
