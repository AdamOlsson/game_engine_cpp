#include "TextPipeline.h"
#include "render_engine/Geometry.h"
#include "render_engine/descriptors/DescriptorSet.h"
#include "render_engine/descriptors/DescriptorSetBuilder.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/graphics_pipeline/GraphicsPipeline.h"
#include "render_engine/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "render_engine/resources/ResourceManager.h"
#include "vulkan/vulkan_core.h"
#include <cstring>
#include <memory>

graphics_pipeline::TextPipeline::TextPipeline(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    CommandBufferManager *command_buffer_manager, SwapChainManager &swap_chain_manager,
    SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers,
    std::unique_ptr<Font> font)
    : m_ctx(ctx), m_font(std::move(font)),
      m_character_buffers(SwapStorageBuffer<CharacterInstanceBufferObject>(
          ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT, 1024)),
      m_text_segment_buffers(SwapStorageBuffer<TextSegmentBufferObject>(
          ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT, 16)),
      m_vertex_buffer(
          VertexBuffer(ctx, Geometry::rectangle_vertices, command_buffer_manager)),
      m_index_buffer(
          IndexBuffer(ctx, Geometry::rectangle_indices, command_buffer_manager)),
      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers)) {

    m_opts.combined_image_sampler =
        vulkan::DescriptorImageInfo(m_font->font_atlas.view(), m_font->sampler);

    m_descriptor_set =
        DescriptorSetBuilder(graphics_pipeline::MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffer(0, vulkan::DescriptorBufferInfo::from_vector(
                                       m_character_buffers.get_buffer_references()))
            .add_uniform_buffer(1, vulkan::DescriptorBufferInfo::from_vector(
                                       uniform_buffers.get_buffer_references()))
            .add_combined_image_sampler(2, m_opts.combined_image_sampler)
            .add_storage_buffer(3, vulkan::DescriptorBufferInfo::from_vector(
                                       m_text_segment_buffers.get_buffer_references()))
            .build(m_ctx, m_descriptor_pool);
    m_graphics_pipeline =
        // clang-format off
        graphics_pipeline::GraphicsPipelineBuilder()
            .set_vertex_shader(ResourceManager::get_instance().get_resource<ShaderResource>("TextVertex"))
            .set_fragment_shader(ResourceManager::get_instance().get_resource<ShaderResource>("TextFragment"))
            .set_descriptor_set_layout(&m_descriptor_set.get_layout())
            .build(m_ctx, swap_chain_manager);
    // clang-format on
}

graphics_pipeline::TextPipeline::~TextPipeline() {}

StorageBuffer<graphics_pipeline::CharacterInstanceBufferObject> &
graphics_pipeline::TextPipeline::get_character_buffer() {
    return m_character_buffers.get_buffer();
}

StorageBuffer<graphics_pipeline::TextSegmentBufferObject> &
graphics_pipeline::TextPipeline::get_text_segment_buffer() {
    return m_text_segment_buffers.get_buffer();
}

void graphics_pipeline::TextPipeline::render_text(const VkCommandBuffer &command_buffer) {
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

void graphics_pipeline::TextPipeline::text_kerning(
    const std::string_view text, const ui::ElementProperties properties) {
    if (text.size() == 0) {
        return;
    }

    const auto kerning_map = m_font->kerning_map;

    const auto font_color = properties.font.color;
    const auto font_size = properties.font.size;
    const float font_rotation = properties.font.rotation;

    auto &glyph_instance_buffer = m_character_buffers.get_buffer();
    auto &text_segment_buffer = m_text_segment_buffers.get_buffer();
    const size_t start_length = glyph_instance_buffer.num_elements();

    const size_t num_char = text.size();
    const auto text_center = properties.container.center;

    text_segment_buffer.emplace_back(font_color, font_size, font_rotation);
    const uint32_t text_segment_idx = text_segment_buffer.num_elements() - 1;

    // First character has no offset
    glyph_instance_buffer.emplace_back(
        text_center, m_font->encode_ascii_char(std::toupper(text[0])), text_segment_idx);

    float combined_offset = 0.0f;
    for (auto i = 1; i < num_char; i++) {
        const auto pair = text.substr(i - 1, 2);
        float offset = font_size;
        if (kerning_map.map.contains(pair)) {
            offset -= font_size * kerning_map.map.find(pair)->second;
        }
        combined_offset += offset;
        const glm::vec2 glyph_position = text_center + glm::vec2(combined_offset, 0.0f);
        glyph_instance_buffer.emplace_back(
            glyph_position, m_font->encode_ascii_char(std::toupper(text[i])),
            text_segment_idx);
    }

    const size_t end_length = glyph_instance_buffer.num_elements();
    const auto center_offset = glm::vec2(combined_offset / 2.0f, 0.0f);
    for (auto i = start_length; i < end_length; i++) {
        auto &glyph = glyph_instance_buffer[i];
        glyph.position -= center_offset;
    }
}

std::string to_string(const graphics_pipeline::TextSegmentBufferObject &obj) {
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

std::ostream &operator<<(std::ostream &os,
                         const graphics_pipeline::TextSegmentBufferObject &obj) {
    return os << to_string(obj);
}

std::string to_string(const graphics_pipeline::CharacterInstanceBufferObject &obj) {
    return std::format("CharacterInstanceBufferObject {{\n"
                       "  position:            ({:.3f}, {:.3f})\n"
                       "  uvwt:                ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                       "  text_segment_idx:    {:d}\n"
                       "}}",
                       obj.position.x, obj.position.y, obj.uvwt.x, obj.uvwt.y, obj.uvwt.z,
                       obj.uvwt.w, obj.text_segment_idx);
}

std::ostream &operator<<(std::ostream &os,
                         const graphics_pipeline::CharacterInstanceBufferObject &obj) {
    return os << to_string(obj);
}
