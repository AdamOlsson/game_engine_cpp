#include "DescriptorSet.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "vulkan/vulkan_core.h"
#include <memory>

DescriptorSet::DescriptorSet(std::shared_ptr<CoreGraphicsContext> ctx,
                             VkDescriptorSetLayout &descriptor_set_layout,
                             DescriptorPool *descriptor_pool, const int capacity,
                             std::vector<UniformBuffer> *uniform_buffers,
                             Texture *texture, Sampler *sampler)
    : m_ctx(ctx), m_capacity(capacity), m_next(0),
      instance_buffers(create_instance_buffers()) {
    m_descriptor_sets =
        create_descriptor_sets(descriptor_set_layout, descriptor_pool, instance_buffers,
                               uniform_buffers, texture, sampler);
}

const VkDescriptorSet DescriptorSet::get() {
    auto &desc = m_descriptor_sets[m_next];
    m_next = ++m_next % m_capacity;
    return desc;
}

std::vector<StorageBuffer> DescriptorSet::create_instance_buffers() {

    size_t num_buffer_slots = 1024;
    std::vector<StorageBuffer> instance_buffers;
    for (auto i = 0; i < m_capacity; i++) {
        instance_buffers.emplace_back(m_ctx, num_buffer_slots);
    }
    return instance_buffers;
}

std::vector<VkDescriptorSet> DescriptorSet::create_descriptor_sets(
    VkDescriptorSetLayout &descriptor_set_layout, DescriptorPool *descriptor_pool,
    std::vector<StorageBuffer> &storage_buffers,
    std::vector<UniformBuffer> *uniform_buffers, Texture *texture, Sampler *sampler) {

    std::vector<VkDescriptorSetLayout> layouts(m_capacity, descriptor_set_layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool->m_descriptor_pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_capacity);
    allocInfo.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> descriptor_sets;
    descriptor_sets.resize(m_capacity);
    if (vkAllocateDescriptorSets(m_ctx->device, &allocInfo, descriptor_sets.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    const VkDeviceSize offset = 0;
    for (size_t i = 0; i < m_capacity; i++) {
        VkDescriptorBufferInfo storage_buffer_info{};
        storage_buffer_info.buffer = storage_buffers[i].buffer;
        storage_buffer_info.offset = offset;
        storage_buffer_info.range = storage_buffers[i].size;

        VkWriteDescriptorSet storage_buffer_descriptor_write{};
        storage_buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        storage_buffer_descriptor_write.dstSet = descriptor_sets[i];
        storage_buffer_descriptor_write.dstBinding = 0;
        storage_buffer_descriptor_write.dstArrayElement = 0;
        storage_buffer_descriptor_write.descriptorType =
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        storage_buffer_descriptor_write.descriptorCount = 1;
        storage_buffer_descriptor_write.pBufferInfo = &storage_buffer_info;
        storage_buffer_descriptor_write.pImageInfo = nullptr;       // Optional
        storage_buffer_descriptor_write.pTexelBufferView = nullptr; // Optional

        VkDescriptorBufferInfo uniform_buffer_info{};
        uniform_buffer_info.buffer = uniform_buffers->at(i).buffer;
        uniform_buffer_info.offset = 0;
        uniform_buffer_info.range = uniform_buffers->at(i).size;

        VkWriteDescriptorSet uniform_buffer_descriptor_write{};
        uniform_buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniform_buffer_descriptor_write.dstSet = descriptor_sets[i];
        uniform_buffer_descriptor_write.dstBinding = 1;
        uniform_buffer_descriptor_write.dstArrayElement = 0;
        uniform_buffer_descriptor_write.descriptorType =
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniform_buffer_descriptor_write.descriptorCount = 1;
        uniform_buffer_descriptor_write.pBufferInfo = &uniform_buffer_info;

        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = texture->view();
        image_info.sampler = sampler->sampler;

        VkWriteDescriptorSet texture_descriptor_write{};
        texture_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        texture_descriptor_write.dstSet = descriptor_sets[i];
        texture_descriptor_write.dstBinding = 2;
        texture_descriptor_write.dstArrayElement = 0;
        texture_descriptor_write.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texture_descriptor_write.descriptorCount = 1;
        texture_descriptor_write.pImageInfo = &image_info;

        std::array<VkWriteDescriptorSet, 3> descriptor_writes = {
            storage_buffer_descriptor_write, uniform_buffer_descriptor_write,
            texture_descriptor_write};

        vkUpdateDescriptorSets(m_ctx->device, descriptor_writes.size(),
                               descriptor_writes.data(), 0, nullptr);
    }
    return descriptor_sets;
}

DescriptorSetBuilder &DescriptorSetBuilder::set_descriptor_set_layout(
    VkDescriptorSetLayout &descriptor_set_layout) {
    m_descriptor_set_layout = descriptor_set_layout;
    return *this;
}

DescriptorSetBuilder &
DescriptorSetBuilder::set_descriptor_pool(DescriptorPool *descriptor_pool) {
    m_descriptor_pool = descriptor_pool;
    return *this;
}

DescriptorSetBuilder &DescriptorSetBuilder::set_capacity(size_t capacity) {
    m_capacity = capacity;
    return *this;
}

DescriptorSetBuilder &
DescriptorSetBuilder::set_uniform_buffers(std::vector<UniformBuffer> *uniform_buffers) {
    m_uniform_buffers = uniform_buffers;
    return *this;
}

DescriptorSetBuilder &DescriptorSetBuilder::set_texture(Texture *texture) {
    m_texture = texture;
    return *this;
}

DescriptorSetBuilder &DescriptorSetBuilder::set_sampler(Sampler *sampler) {
    m_sampler = sampler;
    return *this;
}

DescriptorSet DescriptorSetBuilder::build(std::shared_ptr<CoreGraphicsContext> &ctx) {
    if (m_descriptor_set_layout == VK_NULL_HANDLE) {
        throw std::runtime_error("Descriptor set layout not set");
    }

    if (m_descriptor_pool == nullptr) {
        throw std::runtime_error("Descriptor pool not set");
    }

    return DescriptorSet(ctx, m_descriptor_set_layout, m_descriptor_pool, m_capacity,
                         m_uniform_buffers, m_texture, m_sampler);
}
