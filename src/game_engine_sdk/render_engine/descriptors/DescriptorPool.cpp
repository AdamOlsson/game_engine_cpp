#include "game_engine_sdk/render_engine/descriptors/DescriptorPool.h"

DescriptorPool::DescriptorPool(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                               const uint32_t capacity, const uint32_t num_storage_bufs,
                               const uint32_t num_uniform_bufs,
                               const uint32_t num_samplers)
    : m_ctx(ctx), m_descriptor_pool(create_descriptor_pool(
                      capacity, num_storage_bufs, num_uniform_bufs, num_samplers)) {}

DescriptorPool::DescriptorPool(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                               const DescriptorPoolOpts &&opts)
    : m_ctx(ctx), m_descriptor_pool(create_descriptor_pool(
                      opts.max_num_descriptor_sets, opts.num_storage_buffers,
                      opts.num_uniform_buffers, opts.num_combined_image_sampelrs)) {}

DescriptorPool::~DescriptorPool() {
    if (m_descriptor_pool != nullptr) {
        vkDestroyDescriptorPool(m_ctx->logical_device, m_descriptor_pool, nullptr);
    }
}

VkDescriptorPool DescriptorPool::create_descriptor_pool(const uint32_t capacity,
                                                        const uint32_t num_storage_bufs,
                                                        const uint32_t num_uniform_bufs,
                                                        const uint32_t num_samplers) {
    std::array<VkDescriptorPoolSize, 3> pool_sizes{};

    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    pool_sizes[0].descriptorCount = num_storage_bufs;

    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[1].descriptorCount = num_uniform_bufs;

    pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[2].descriptorCount = num_samplers;

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
