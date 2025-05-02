#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(std::shared_ptr<CoreGraphicsContext> &ctx,
                               const int capacity)
    : m_ctx(ctx), m_descriptor_pool(create_descriptor_pool(capacity)) {}

DescriptorPool::~DescriptorPool() {
    if (m_descriptor_pool != nullptr) {
        vkDestroyDescriptorPool(m_ctx->device, m_descriptor_pool, nullptr);
    }
}

VkDescriptorPool DescriptorPool::create_descriptor_pool(const int capacity) {
    std::array<VkDescriptorPoolSize, 3> pool_sizes{};

    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    pool_sizes[0].descriptorCount = static_cast<uint32_t>(capacity);

    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[1].descriptorCount = static_cast<uint32_t>(capacity);

    // TODO: Do we really need this many samplers?
    pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[2].descriptorCount = static_cast<uint32_t>(capacity);

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = static_cast<uint32_t>(capacity);

    VkDescriptorPool descriptor_pool;
    if (vkCreateDescriptorPool(m_ctx->device, &pool_info, nullptr, &descriptor_pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return descriptor_pool;
}
