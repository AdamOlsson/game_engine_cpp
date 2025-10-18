#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipelineDescriptorSet.h"
#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSetBuilder.h"

using namespace graphics_pipeline;

QuadPipelineDescriptorSet::QuadPipelineDescriptorSet(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx,
    vulkan::DescriptorPool &descriptor_pool, QuadPipelineDescriptorSetOpts &&opts)
    : m_descriptor_set(
          SwapDescriptorSetBuilder(opts.num_sets)
              .add_storage_buffer(0, std::move(opts.storage_buffer_refs))
              .add_uniform_buffer(1, std::move(opts.uniform_buffer_refs))
              .add_combined_image_sampler(2, opts.combined_image_sampler_infos)
              .build(ctx, descriptor_pool)) {}

const vulkan::DescriptorSetLayout &QuadPipelineDescriptorSet::get_layout() {
    return m_descriptor_set.get_layout();
}

SwapDescriptorSet *QuadPipelineDescriptorSet::handle() { return &m_descriptor_set; }
vulkan::DescriptorSet QuadPipelineDescriptorSet::get_next() {
    return m_descriptor_set.get();
}
