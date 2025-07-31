#include "TextPipeline.h"
#include "render_engine/Geometry.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/descriptors/DescriptorSet.h"
#include "render_engine/descriptors/DescriptorSetBuilder.h"
#include "render_engine/descriptors/DescriptorSetLayoutBuilder.h"
#include "render_engine/graphics_pipeline/GraphicsPipeline.h"
#include "render_engine/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "render_engine/resources/ResourceManager.h"
#include "vulkan/vulkan_core.h"
#include <cstring>
#include <memory>

using namespace ui;

TextPipeline::TextPipeline(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    SwapChainManager &swap_chain_manager,
    SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers, Sampler &sampler,
    Texture &texture)
    : m_ctx(ctx), m_character_buffers(SwapStorageBuffer<CharacterInstanceBufferObject>(
                      ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_text_segment_buffers(
          SwapStorageBuffer<TextSegmentBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 16)),
      m_vertex_buffer(
          VertexBuffer(ctx, Geometry::rectangle_vertices, swap_chain_manager)),
      m_index_buffer(IndexBuffer(ctx, Geometry::rectangle_indices, swap_chain_manager)),
      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers)),
      m_descriptor_set(create_descriptor_set(uniform_buffers, sampler, texture)),
      m_graphics_pipeline(
          // clang-format off
        graphics_pipeline::GraphicsPipelineBuilder()
            .set_vertex_shader(ResourceManager::get_instance().get_resource<ShaderResource>("TextVertex"))
            .set_fragment_shader(ResourceManager::get_instance().get_resource<ShaderResource>("TextFragment"))
            .set_descriptor_set_layout(&m_descriptor_set.get_layout())
            .build(m_ctx, swap_chain_manager)
          // clang-format on
      ) {}

TextPipeline::~TextPipeline() {}

DescriptorSet TextPipeline::create_descriptor_set(
    SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers, Sampler &sampler,
    Texture &texture) {
    auto layout =
        DescriptorSetLayoutBuilder()
            .add(BufferDescriptor<
                 GpuBufferType::Storage>::create_descriptor_set_layout_binding(0))
            .add(BufferDescriptor<
                 GpuBufferType::Uniform>::create_descriptor_set_layout_binding(1))
            .add(Sampler::create_descriptor_set_layout_binding(2))
            .add(BufferDescriptor<
                 GpuBufferType::Storage>::create_descriptor_set_layout_binding(3))
            .build(m_ctx);

    return DescriptorSetBuilder(std::move(layout), m_descriptor_pool,
                                MAX_FRAMES_IN_FLIGHT)
        .add_gpu_buffer(0, m_character_buffers.get_buffer_references())
        .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
        .set_texture_and_sampler(2, texture, sampler)
        .add_gpu_buffer(3, m_text_segment_buffers.get_buffer_references())
        .build(m_ctx);
}

StorageBuffer<CharacterInstanceBufferObject> &TextPipeline::get_character_buffer() {
    return m_character_buffers.get_buffer();
}

StorageBuffer<TextSegmentBufferObject> &TextPipeline::get_text_segment_buffer() {
    return m_text_segment_buffers.get_buffer();
}

void TextPipeline::render_text(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_character_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances <= 0) {
        return;
    }

    const auto &text_segment_buffer = m_text_segment_buffers.get_buffer();

    const VkDeviceSize vertex_buffers_offset = 0;

    auto descriptor_set = m_descriptor_set.get();
    m_graphics_pipeline.render(command_buffer, m_vertex_buffer, m_index_buffer,
                               descriptor_set, num_instances);
    m_character_buffers.rotate();
    m_text_segment_buffers.rotate();
}

std::string to_string(const TextSegmentBufferObject &obj) {
    return std::format("TextSegmentBufferObject {{\n"
                       "  font_color:     ({:.3f}, {:.3f}, {:.3f})\n"
                       "  font_rotation:  {:.3f}\n"
                       "  font_size: {:d}\n"
                       "  font_weight: {:.3f}\n"
                       "  font_sharpness: {:.3f}\n"
                       "}}",
                       obj.font_color.x, obj.font_color.y, obj.font_color.z,
                       obj.font_rotation, obj.font_size, obj.font_weight,
                       obj.font_sharpness);
}

std::ostream &operator<<(std::ostream &os, const TextSegmentBufferObject &obj) {
    return os << to_string(obj);
}

std::string to_string(const CharacterInstanceBufferObject &obj) {
    return std::format("CharacterInstanceBufferObject {{\n"
                       "  position:            ({:.3f}, {:.3f})\n"
                       "  uvwt:                ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                       "  text_segment_idx:    {:d}\n"
                       "}}",
                       obj.position.x, obj.position.y, obj.uvwt.x, obj.uvwt.y, obj.uvwt.z,
                       obj.uvwt.w, obj.text_segment_idx);
}

std::ostream &operator<<(std::ostream &os, const CharacterInstanceBufferObject &obj) {
    return os << to_string(obj);
}
