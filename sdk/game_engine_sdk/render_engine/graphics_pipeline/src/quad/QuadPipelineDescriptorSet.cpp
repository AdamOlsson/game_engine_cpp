#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipelineDescriptorSet.h"
#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSetBuilder.h"

using namespace graphics_pipeline;

QuadPipelineDescriptorSet::QuadPipelineDescriptorSet(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::DescriptorPool &descriptor_pool, QuadPipelineDescriptorSetOpts &&opts)
    : m_descriptor_set(build_descriptor_set(ctx, descriptor_pool, std::move(opts))) {}

SwapDescriptorSet QuadPipelineDescriptorSet::build_descriptor_set(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::DescriptorPool &descriptor_pool, QuadPipelineDescriptorSetOpts &&opts) {
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

vulkan::DescriptorSetLayout &QuadPipelineDescriptorSet::get_layout() {
    return m_descriptor_set.get_layout();
}

SwapDescriptorSet *QuadPipelineDescriptorSet::handle() { return &m_descriptor_set; }
vulkan::DescriptorSet QuadPipelineDescriptorSet::get_next() {
    return m_descriptor_set.get();
}
