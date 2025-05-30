#include "DescriptorSet.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "vulkan/vulkan_core.h"
#include <memory>

DescriptorSet::DescriptorSet(std::shared_ptr<CoreGraphicsContext> ctx,
                             std::vector<VkDescriptorSet> &descriptor_sets)
    : m_ctx(ctx), m_capacity(descriptor_sets.size()), m_next(0),
      m_descriptor_sets(std::move(descriptor_sets)) {}

const VkDescriptorSet DescriptorSet::get() {
    auto &desc = m_descriptor_sets[m_next];
    m_next = ++m_next % m_capacity;
    return desc;
}

DescriptorSetBuilder::DescriptorSetBuilder(VkDescriptorSetLayout &descriptor_set_layout,
                                           DescriptorPool &descriptor_pool,
                                           size_t capacity)
    : m_descriptor_set_layout(&descriptor_set_layout),
      m_descriptor_pool(&descriptor_pool), m_capacity(capacity) {}

DescriptorSetBuilder &
DescriptorSetBuilder::set_uniform_buffers(std::vector<UniformBuffer> &uniform_buffers) {
    m_uniform_buffers = &uniform_buffers;
    return *this;
}

DescriptorSetBuilder &
DescriptorSetBuilder::set_instance_buffers(std::vector<StorageBuffer> &instance_buffers) {
    m_instance_buffers = &instance_buffers;
    return *this;
}

DescriptorSetBuilder &DescriptorSetBuilder::set_texture_and_sampler(Texture &texture,
                                                                    Sampler &sampler) {
    m_texture = &texture;
    m_sampler = &sampler;
    return *this;
}

std::vector<VkDescriptorSet> DescriptorSetBuilder::allocate_descriptor_sets(
    std::shared_ptr<CoreGraphicsContext> &ctx) {
    std::vector<VkDescriptorSetLayout> layouts(m_capacity, *m_descriptor_set_layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = m_descriptor_pool->m_descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(m_capacity);
    alloc_info.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> descriptor_sets;
    descriptor_sets.resize(m_capacity);
    if (vkAllocateDescriptorSets(ctx->device, &alloc_info, descriptor_sets.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    return descriptor_sets;
}

VkWriteDescriptorSet DescriptorSetBuilder::create_texture_and_sampler_descriptor_write(
    const VkDescriptorSet &dst_descriptor_set, VkDescriptorImageInfo &image_info) {
    VkWriteDescriptorSet texture_descriptor_write{};
    texture_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    texture_descriptor_write.dstSet = dst_descriptor_set;
    texture_descriptor_write.dstBinding = 2;
    texture_descriptor_write.dstArrayElement = 0;
    texture_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    texture_descriptor_write.descriptorCount = 1;
    texture_descriptor_write.pImageInfo = &image_info;
    return texture_descriptor_write;
}

VkWriteDescriptorSet DescriptorSetBuilder::create_instance_buffer_descriptor_write(
    const VkDescriptorSet &dst_descriptor_set,
    const VkDescriptorBufferInfo &buffer_info) {
    VkWriteDescriptorSet instance_buffer_descriptor_write{};
    instance_buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    instance_buffer_descriptor_write.dstSet = dst_descriptor_set;
    instance_buffer_descriptor_write.dstBinding = 0;
    instance_buffer_descriptor_write.dstArrayElement = 0;
    instance_buffer_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    instance_buffer_descriptor_write.descriptorCount = 1;
    instance_buffer_descriptor_write.pBufferInfo = &buffer_info;
    instance_buffer_descriptor_write.pImageInfo = nullptr;       // Optional
    instance_buffer_descriptor_write.pTexelBufferView = nullptr; // Optional
    return instance_buffer_descriptor_write;
}

VkWriteDescriptorSet DescriptorSetBuilder::create_uniform_buffer_descriptor_write(
    const VkDescriptorSet &dst_descriptor_set,
    const VkDescriptorBufferInfo &buffer_info) {
    VkWriteDescriptorSet uniform_buffer_descriptor_write{};
    uniform_buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uniform_buffer_descriptor_write.dstSet = dst_descriptor_set;
    uniform_buffer_descriptor_write.dstBinding = 1;
    uniform_buffer_descriptor_write.dstArrayElement = 0;
    uniform_buffer_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniform_buffer_descriptor_write.descriptorCount = 1;
    uniform_buffer_descriptor_write.pBufferInfo = &buffer_info;
    return uniform_buffer_descriptor_write;
}

DescriptorSet DescriptorSetBuilder::build(std::shared_ptr<CoreGraphicsContext> &ctx) {
    if (m_descriptor_set_layout == nullptr) {
        throw std::runtime_error("Descriptor set layout not set");
    }

    if (m_descriptor_pool == nullptr) {
        throw std::runtime_error("Descriptor pool not set");
    }

    std::vector<VkDescriptorSet> descriptor_sets = allocate_descriptor_sets(ctx);

    for (auto i = 0; i < m_capacity; i++) {
        std::vector<VkWriteDescriptorSet> descriptor_writes = {};

        VkDescriptorBufferInfo instance_buffer_info{};
        if (m_instance_buffers->size() > 0) {
            instance_buffer_info.buffer = m_instance_buffers->at(i).buffer;
            instance_buffer_info.offset = 0;
            instance_buffer_info.range = m_instance_buffers->at(i).size;

            descriptor_writes.push_back(create_instance_buffer_descriptor_write(
                descriptor_sets[i], instance_buffer_info));
        }

        VkDescriptorBufferInfo uniform_buffer_info{};
        if (m_uniform_buffers != nullptr) {
            uniform_buffer_info.buffer = m_uniform_buffers->at(i).buffer;
            uniform_buffer_info.offset = 0;
            uniform_buffer_info.range = m_uniform_buffers->at(i).size;

            descriptor_writes.push_back(create_uniform_buffer_descriptor_write(
                descriptor_sets[i], uniform_buffer_info));
        }

        VkDescriptorImageInfo image_info{};
        if (m_texture != nullptr && m_sampler != nullptr) {
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = m_texture->view();
            image_info.sampler = m_sampler->sampler;

            descriptor_writes.push_back(create_texture_and_sampler_descriptor_write(
                descriptor_sets[i], image_info));
        }

        vkUpdateDescriptorSets(ctx->device, descriptor_writes.size(),
                               descriptor_writes.data(), 0, nullptr);
    }

    return DescriptorSet(ctx, descriptor_sets);
}
