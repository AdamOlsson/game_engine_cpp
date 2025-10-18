#include "game_engine_sdk/render_engine/descriptors/DescriptorSetBuilder.h"
#include "vulkan/vulkan_core.h"

SwapDescriptorSetBuilder::SwapDescriptorSetBuilder(size_t capacity)
    : m_capacity(capacity) {}

SwapDescriptorSetBuilder &SwapDescriptorSetBuilder::add_storage_buffer(
    size_t binding, std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos,
    SwapDescriptorSetBuilderOptions &&options) {
    if (buffer_infos.size() != m_capacity) {
        throw std::runtime_error(
            "size of storage buffer reference needs to be equal to capacity");
    }

    m_storage_buffer_bindings.insert(m_storage_buffer_bindings.end(), buffer_infos.size(),
                                     binding);
    m_storage_buffers_infos.insert(m_storage_buffers_infos.end(),
                                   std::make_move_iterator(buffer_infos.begin()),
                                   std::make_move_iterator(buffer_infos.end()));

    m_descriptor_set_layout_builder.add_storage_buffer_binding(binding,
                                                               options.stage_flags);
    return *this;
}

SwapDescriptorSetBuilder &SwapDescriptorSetBuilder::add_storage_buffer(
    size_t binding, std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos) {
    return add_storage_buffer(binding, std::move(buffer_infos),
                              SwapDescriptorSetBuilderOptions{});
}

SwapDescriptorSetBuilder &SwapDescriptorSetBuilder::add_uniform_buffer(
    size_t binding, std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos,
    SwapDescriptorSetBuilderOptions &&options) {
    if (buffer_infos.size() != m_capacity) {
        throw std::runtime_error(
            "size of uniform buffer reference needs to be equal to capacity");
    }

    m_uniform_buffer_bindings.insert(m_uniform_buffer_bindings.end(), buffer_infos.size(),
                                     binding);
    m_uniform_buffers_infos.insert(m_uniform_buffers_infos.end(),
                                   std::make_move_iterator(buffer_infos.begin()),
                                   std::make_move_iterator(buffer_infos.end()));

    m_descriptor_set_layout_builder.add_uniform_buffer_binding(binding,
                                                               options.stage_flags);
    return *this;
}

SwapDescriptorSetBuilder &SwapDescriptorSetBuilder::add_uniform_buffer(
    size_t binding, std::vector<vulkan::DescriptorBufferInfo> &&buffer_infos) {
    return add_uniform_buffer(binding, std::move(buffer_infos),
                              SwapDescriptorSetBuilderOptions{});
}

SwapDescriptorSetBuilder &SwapDescriptorSetBuilder::add_combined_image_sampler(
    size_t binding, std::vector<vulkan::DescriptorImageInfo> &image_info) {
    m_combined_image_sampler_infos.assign(image_info.begin(), image_info.end());
    m_combined_image_sampler_binding = binding;
    m_descriptor_set_layout_builder.add_combined_image_sampler_binding(
        binding, m_combined_image_sampler_infos.size());
    /*logger::debug("Num image sampler descriptors: ",*/
    /*              m_combined_image_sampler_infos.size());*/
    return *this;
}

std::vector<VkDescriptorSet> SwapDescriptorSetBuilder::allocate_descriptor_sets(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx,
    DescriptorPool &descriptor_pool, const VkDescriptorSetLayout &descriptor_set_layout) {

    std::vector<VkDescriptorSetLayout> layouts(m_capacity, descriptor_set_layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(m_capacity);
    alloc_info.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> descriptor_sets;
    descriptor_sets.resize(m_capacity);
    if (vkAllocateDescriptorSets(ctx->logical_device, &alloc_info,
                                 descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    return descriptor_sets;
}

VkWriteDescriptorSet SwapDescriptorSetBuilder::create_buffer_descriptor_write(
    const VkDescriptorType type, const VkDescriptorSet &dst_descriptor_set,
    const VkDescriptorBufferInfo *buffer_info, const size_t binding_num) {
    VkWriteDescriptorSet buffer_descriptor_write{};
    buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    buffer_descriptor_write.dstSet = dst_descriptor_set;
    buffer_descriptor_write.dstBinding = binding_num;
    buffer_descriptor_write.dstArrayElement = 0;
    buffer_descriptor_write.descriptorType = type;
    buffer_descriptor_write.descriptorCount = 1;
    buffer_descriptor_write.pBufferInfo = buffer_info;
    return buffer_descriptor_write;
}

VkWriteDescriptorSet
SwapDescriptorSetBuilder::create_texture_and_sampler_descriptor_write(
    const VkDescriptorSet &dst_descriptor_set, const VkDescriptorImageInfo *image_infos,
    const size_t num_image_infos, const size_t binding_num) {
    VkWriteDescriptorSet texture_descriptor_write{};
    texture_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    texture_descriptor_write.dstSet = dst_descriptor_set;
    texture_descriptor_write.dstBinding = binding_num;
    texture_descriptor_write.dstArrayElement = 0;
    texture_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    texture_descriptor_write.descriptorCount = num_image_infos;
    texture_descriptor_write.pImageInfo = image_infos;
    return texture_descriptor_write;
}

SwapDescriptorSet
SwapDescriptorSetBuilder::build(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                                DescriptorPool &descriptor_pool) {

    if (m_storage_buffers_infos.size() % m_capacity != 0) {
        throw std::runtime_error(
            "Number of storage buffers needs to be a multiple of capacity");
    }

    if (m_uniform_buffers_infos.size() % m_capacity != 0) {
        throw std::runtime_error(
            "Number of uniform buffers needs to be a multiple of capacity");
    }

    vulkan::DescriptorSetLayout descriptor_set_layout =
        m_descriptor_set_layout_builder.build(ctx);

    std::vector<VkDescriptorSet> descriptor_sets =
        allocate_descriptor_sets(ctx, descriptor_pool, descriptor_set_layout);

    const size_t num_storage_buffers_per_set =
        m_storage_buffers_infos.size() / m_capacity;
    const size_t num_uniform_buffers_per_set =
        m_uniform_buffers_infos.size() / m_capacity;

    for (auto i = 0; i < m_capacity; i++) {
        std::vector<VkWriteDescriptorSet> descriptor_writes = {};

        for (auto j = 0; j < num_storage_buffers_per_set; j++) {
            const auto idx = j * m_capacity + i;

            descriptor_writes.push_back(create_buffer_descriptor_write(
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptor_sets[i],
                m_storage_buffers_infos[idx].get(), m_storage_buffer_bindings[idx]));
        }

        for (auto j = 0; j < num_uniform_buffers_per_set; j++) {
            const auto idx = j * m_capacity + i;
            descriptor_writes.push_back(create_buffer_descriptor_write(
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptor_sets[i],
                m_uniform_buffers_infos[idx].get(), m_uniform_buffer_bindings[idx]));
        }

        if (m_combined_image_sampler_infos.size() > 0) {
            descriptor_writes.push_back(create_texture_and_sampler_descriptor_write(
                descriptor_sets[i], m_combined_image_sampler_infos.data(),
                m_combined_image_sampler_infos.size(), m_combined_image_sampler_binding));
        }

        vkUpdateDescriptorSets(ctx->logical_device, descriptor_writes.size(),
                               descriptor_writes.data(), 0, nullptr);
    }

    return SwapDescriptorSet(ctx, descriptor_sets, std::move(descriptor_set_layout));
}
