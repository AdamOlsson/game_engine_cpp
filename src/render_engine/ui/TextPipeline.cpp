#include "render_engine/ui/TextPipeline.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/ShaderModule.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/resources/ResourceManager.h"
#include "vulkan/vulkan_core.h"
#include <cstring>
#include <memory>
#include <vector>

using namespace ui;

TextPipeline::TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                           SwapChainManager &swap_chain_manager,
                           std::vector<UniformBuffer> &uniform_buffers, Sampler &sampler,
                           Texture &texture)
    : m_ctx(ctx), m_character_buffers(SwapStorageBuffer<CharacterInstanceBufferObject>(
                      ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_text_segment_buffers(
          SwapStorageBuffer<TextSegmentBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 16)),
      m_vertex_buffer(
          VertexBuffer(ctx, Geometry::rectangle_vertices, swap_chain_manager)),
      m_index_buffer(IndexBuffer(ctx, Geometry::rectangle_indices, swap_chain_manager)),
      m_descriptor_set_layout(create_descriptor_set_layout()),
      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers)),
      m_descriptor_set(create_descriptor_set(uniform_buffers, sampler, texture)),
      m_pipeline(create_pipeline(m_descriptor_set_layout, swap_chain_manager)) {}

TextPipeline::~TextPipeline() {
    vkDestroyDescriptorSetLayout(m_ctx->device, m_descriptor_set_layout, nullptr);
}

VkDescriptorSetLayout TextPipeline::create_descriptor_set_layout() {
    return DescriptorSetLayoutBuilder()
        .add(StorageBuffer<
             CharacterInstanceBufferObject>::create_descriptor_set_layout_binding(0))
        .add(UniformBuffer::create_descriptor_set_layout_binding(1))
        .add(Sampler::create_descriptor_set_layout_binding(2))
        .add(StorageBuffer<TextSegmentBufferObject>::create_descriptor_set_layout_binding(
            3))
        .build(m_ctx.get());
}

DescriptorSet
TextPipeline::create_descriptor_set(std::vector<UniformBuffer> &uniform_buffers,
                                    Sampler &sampler, Texture &texture) {
    return DescriptorSetBuilder(m_descriptor_set_layout, m_descriptor_pool,
                                MAX_FRAMES_IN_FLIGHT)
        .add_storage_buffers(0, m_character_buffers.get_buffer_references())
        .set_uniform_buffers(1, uniform_buffers)
        .set_texture_and_sampler(2, texture, sampler)
        .add_storage_buffers(3, m_text_segment_buffers.get_buffer_references())
        .build(m_ctx);
}

Pipeline TextPipeline::create_pipeline(VkDescriptorSetLayout &descriptor_set_layout,
                                       SwapChainManager &swap_chain_manager) {
    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code = resoure_manager.get_resource<ShaderResource>("TextVertex");
    auto frag_shader_code = resoure_manager.get_resource<ShaderResource>("TextFragment");

    ShaderModule vertex_shader = ShaderModule(m_ctx, *vert_shader_code);
    ShaderModule fragment_shader = ShaderModule(m_ctx, *frag_shader_code);

    Pipeline pipeline =
        Pipeline(m_ctx, descriptor_set_layout, {}, vertex_shader.shader_module,
                 fragment_shader.shader_module, swap_chain_manager);

    return pipeline;
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
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_pipeline.m_pipeline);

    auto descriptor = m_descriptor_set.get();
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipeline.m_pipeline_layout, 0, 1, &descriptor, 0, nullptr);

    vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer.buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, m_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, m_index_buffer.num_indices, num_instances, 0, 0, 0);

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
