#include "vulkan/DescriptorPool.h"

vulkan::DescriptorPool::DescriptorPool(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx, const uint32_t capacity,
    const uint32_t num_storage_bufs, const uint32_t num_uniform_bufs,
    const uint32_t num_samplers)
    : m_ctx(ctx), m_descriptor_pool(create_descriptor_pool(
                      capacity, num_storage_bufs, num_uniform_bufs, num_samplers)) {}

vulkan::DescriptorPool::DescriptorPool(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    const DescriptorPoolOpts &&opts)
    : m_ctx(ctx), m_descriptor_pool(create_descriptor_pool(
                      opts.max_num_descriptor_sets, opts.num_storage_buffers,
                      opts.num_uniform_buffers, opts.num_combined_image_samplers)) {}

vulkan::DescriptorPool::~DescriptorPool() {
    if (m_descriptor_pool == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyDescriptorPool(m_ctx->logical_device, m_descriptor_pool, nullptr);
}

VkDescriptorPool vulkan::DescriptorPool::create_descriptor_pool(
    const uint32_t capacity, const uint32_t num_storage_bufs,
    const uint32_t num_uniform_bufs, const uint32_t num_samplers) {

    std::vector<VkDescriptorPoolSize> pool_sizes{};

    if (num_storage_bufs > 0) {
        pool_sizes.push_back(VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = num_storage_bufs,
        });
    }

    if (num_uniform_bufs > 0) {
        pool_sizes.push_back(VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = num_uniform_bufs,
        });
    }

    if (num_samplers > 0) {
        pool_sizes.push_back(VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = num_samplers,
        });
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = capacity;

    VkDescriptorPool descriptor_pool;
    if (vkCreateDescriptorPool(m_ctx->logical_device, &pool_info, nullptr,
                               &descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return descriptor_pool;
}

vulkan::DescriptorPool::DescriptorPool(vulkan::DescriptorPool &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_descriptor_pool(other.m_descriptor_pool) {
    other.m_descriptor_pool = VK_NULL_HANDLE;
}

vulkan::DescriptorPool &
vulkan::DescriptorPool::operator=(DescriptorPool &&other) noexcept {
    if (this != &other) {
        if (m_descriptor_pool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m_ctx->logical_device, m_descriptor_pool, nullptr);
        }

        m_ctx = std::move(other.m_ctx);
        m_descriptor_pool = other.m_descriptor_pool;

        other.m_descriptor_pool = VK_NULL_HANDLE;
    }
    return *this;
}
