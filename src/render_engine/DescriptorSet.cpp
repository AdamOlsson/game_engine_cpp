#include "DescriptorSet.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <memory>

std::vector<VkDescriptorSet> create_descriptor_sets(
    CoreGraphicsContext *ctx, VkDescriptorSetLayout &descriptor_set_layout,
    VkDescriptorPool &descriptor_pool, const int capacity,
    std::vector<StorageBuffer> &storage_buffers,
    std::vector<UniformBuffer> *uniform_buffers, Texture *texture, Sampler *sampler);

DescriptorSet::DescriptorSet() : ctx(nullptr), size(0), next(0), descriptor_sets() {}

// TODO: I would like to make a version of the builder pattern here
DescriptorSet::DescriptorSet(std::shared_ptr<CoreGraphicsContext> ctx,
                             VkDescriptorSetLayout &descriptor_set_layout,
                             VkDescriptorPool &descriptor_pool, const int capacity,
                             std::vector<UniformBuffer> *uniform_buffers,
                             Texture *texture, Sampler *sampler)
    : ctx(ctx), size(capacity), next(0),
      instance_buffers(create_instance_buffers(ctx, capacity)) {
    descriptor_sets = create_descriptor_sets(ctx.get(), descriptor_set_layout,
                                             descriptor_pool, capacity, instance_buffers,
                                             uniform_buffers, texture, sampler);
}

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept
    : ctx(std::move(other.ctx)), size(other.size), next(other.next),
      instance_buffers(std::move(other.instance_buffers)),
      descriptor_sets(std::move(other.descriptor_sets)) {
    other.ctx = nullptr;
    other.size = 0;
    other.next = 0;
    other.descriptor_sets.clear();
    other.instance_buffers.clear();
}

DescriptorSet &DescriptorSet::operator=(DescriptorSet &&other) noexcept {
    if (this != &other) {
        ctx = std::move(other.ctx);
        size = other.size;
        next = other.next;
        descriptor_sets = std::move(other.descriptor_sets);
        instance_buffers = std::move(other.instance_buffers);

        other.ctx = nullptr;
        other.size = 0;
        other.next = 0;
        other.descriptor_sets.clear();
        other.instance_buffers.clear();
    }

    return *this;
}

DescriptorSet::~DescriptorSet() {}

const VkDescriptorSet DescriptorSet::get() {
    auto &desc = descriptor_sets[next];
    next = ++next % size;
    return desc;
}

std::vector<VkDescriptorSet> create_descriptor_sets(
    CoreGraphicsContext *ctx, VkDescriptorSetLayout &descriptor_set_layout,
    VkDescriptorPool &descriptor_pool, const int capacity,
    std::vector<StorageBuffer> &storage_buffers,
    std::vector<UniformBuffer> *uniform_buffers, Texture *texture, Sampler *sampler) {

    std::vector<VkDescriptorSetLayout> layouts(capacity, descriptor_set_layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(capacity);
    allocInfo.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> descriptor_sets;
    descriptor_sets.resize(capacity);
    if (vkAllocateDescriptorSets(ctx->device, &allocInfo, descriptor_sets.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    const VkDeviceSize offset = 0;
    for (size_t i = 0; i < capacity; i++) {
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

        vkUpdateDescriptorSets(ctx->device, descriptor_writes.size(),
                               descriptor_writes.data(), 0, nullptr);
    }
    return descriptor_sets;
}
