#include "graphics_pipeline/SwapDescriptorSetBuilder.h"
#include "graphics_pipeline/geometry/GeometryPipelineDecriptorSet.h"

graphics_pipeline::geometry::GeometryPipelineDescriptorSet::GeometryPipelineDescriptorSet(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::DescriptorPool &descriptor_pool, GeometryPipelineDescriptorSetOpts &&opts)
    : m_descriptor_set(build_descriptor_set(ctx, descriptor_pool, std::move(opts))) {}

graphics_pipeline::SwapDescriptorSet
graphics_pipeline::geometry::GeometryPipelineDescriptorSet::build_descriptor_set(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::DescriptorPool &descriptor_pool, GeometryPipelineDescriptorSetOpts &&opts) {
    auto builder = SwapDescriptorSetBuilder(opts.num_sets);
    bool no_descriptors = true;
    if (opts.storage_buffer_refs.size() > 0) {
        builder.add_storage_buffer(0, std::move(opts.storage_buffer_refs));
        no_descriptors = false;
    }

    if (opts.uniform_buffer_refs.size() > 0) {
        builder.add_uniform_buffer(1, std::move(opts.uniform_buffer_refs));
        no_descriptors = false;
    }

    if (opts.combined_image_sampler_infos.size() > 0) {
        builder.add_combined_image_sampler(2, opts.combined_image_sampler_infos);
        no_descriptors = false;
    }

    if (no_descriptors) {
        std::runtime_error("If the descriptor set is empty, you should not create it.");
    }

    return builder.build(ctx, descriptor_pool);
}

vulkan::DescriptorSetLayout &
graphics_pipeline::geometry::GeometryPipelineDescriptorSet::get_layout() {
    return m_descriptor_set.get_layout();
}

graphics_pipeline::SwapDescriptorSet *
graphics_pipeline::geometry::GeometryPipelineDescriptorSet::handle() {
    return &m_descriptor_set;
}

vulkan::DescriptorSet
graphics_pipeline::geometry::GeometryPipelineDescriptorSet::get_next() {
    return m_descriptor_set.get_next();
}

vulkan::DescriptorSet
graphics_pipeline::geometry::GeometryPipelineDescriptorSet::get_current() {
    return m_descriptor_set.get_current();
}

void graphics_pipeline::geometry::GeometryPipelineDescriptorSet::rotate() {
    m_descriptor_set.rotate();
}
