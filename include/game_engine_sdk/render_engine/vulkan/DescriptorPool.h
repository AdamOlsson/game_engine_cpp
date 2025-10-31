#pragma once

#include "GraphicsContext.h"
#include "vulkan/vulkan_core.h"
namespace vulkan {
struct DescriptorPoolOpts {
    unsigned int max_num_descriptor_sets = 2;
    unsigned int num_storage_buffers = 2;
    unsigned int num_uniform_buffers = 2;
    unsigned int num_combined_image_samplers = 1;
};

class DescriptorPool {
  private:
    std::shared_ptr<GraphicsContext> m_ctx;
    VkDescriptorPool m_descriptor_pool;

    VkDescriptorPool create_descriptor_pool(const uint32_t capacity,
                                            const uint32_t num_storage_bufs,
                                            const uint32_t num_uniform_bufs,
                                            const uint32_t num_samplers);

  public:
    DescriptorPool() = default;

    DescriptorPool(std::shared_ptr<GraphicsContext> &ctx, const uint32_t capacity,
                   const uint32_t num_storage_bufs, const uint32_t num_uniform_bufs,
                   const uint32_t num_samplers);

    DescriptorPool(std::shared_ptr<GraphicsContext> &ctx,
                   const DescriptorPoolOpts &&opts);

    ~DescriptorPool();
    DescriptorPool(const DescriptorPool &other) = delete;
    DescriptorPool(DescriptorPool &&other) noexcept;

    DescriptorPool &operator=(const DescriptorPool &other) = delete;
    DescriptorPool &operator=(DescriptorPool &&other) noexcept;

    operator VkDescriptorPool() const { return m_descriptor_pool; }
};
} // namespace vulkan
