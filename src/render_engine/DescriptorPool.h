#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
class DescriptorPool {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    VkDescriptorPool create_descriptor_pool(const uint32_t capacity,
                                            const uint32_t num_storage_bufs,
                                            const uint32_t num_uniform_bufs,
                                            const uint32_t num_samplers);

  public:
    VkDescriptorPool m_descriptor_pool;

    DescriptorPool() = default;
    DescriptorPool(std::shared_ptr<CoreGraphicsContext> &ctx, const uint32_t capacity,
                   const uint32_t num_storage_bufs, const uint32_t num_uniform_bufs,
                   const uint32_t num_samplers);

    ~DescriptorPool();
    DescriptorPool(const DescriptorPool &other) = delete;
    DescriptorPool(DescriptorPool &&other) noexcept = default;

    DescriptorPool &operator=(const DescriptorPool &other) = delete;
    DescriptorPool &operator=(DescriptorPool &&other) noexcept = default;
};
