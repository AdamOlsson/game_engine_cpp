#include "render_engine/RenderEngine.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/images/ImageResource.h"
#include "render_engine/ui/TextBox.h"
#include "render_engine/ui/TextPipeline.h"
#include "render_engine/ui/UIPipeline.h"
#include "render_engine/window/Window.h"
#include "vulkan/vulkan_core.h"
#include <memory>

// TODO: How can I create a proper render hierarchy? Preferably I would want my pipelines
// to act on some state object for rendering

RenderEngine::RenderEngine(std::shared_ptr<CoreGraphicsContext> ctx,
                           const UseFont use_font)
    : m_swap_chain_manager(std::make_unique<SwapChainManager>(ctx)),

      m_window_dimension_buffers(SwapUniformBuffer<window::WindowDimension<float>>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1)),
      m_sampler(Sampler(ctx))

{

    m_window_dimension_buffers.write(ctx->window->dimensions<float>());

    auto device_queues = ctx->get_device_queues();

    auto &resource_manager = ResourceManager::get_instance();
    auto dog_image = resource_manager.get_resource<ImageResource>("DogImage");
    m_texture = Texture::unique_from_image_resource(
        ctx, *m_swap_chain_manager, device_queues.graphics_queue, dog_image);

    m_geometry_pipeline = std::make_unique<GeometryPipeline>(
        ctx, *m_swap_chain_manager, m_window_dimension_buffers, m_sampler, *m_texture);

    switch (use_font) {
    case UseFont::Default: {
        auto default_font = resource_manager.get_resource<FontResource>("DefaultFont");
        m_font = std::make_unique<Font>(ctx, *m_swap_chain_manager,
                                        device_queues.graphics_queue, default_font);
        break;
    }
    default:
        m_font = nullptr;
        break;
    }

    if (m_font != nullptr) {
        m_text_pipeline = std::make_unique<ui::TextPipeline>(
            ctx, *m_swap_chain_manager, m_window_dimension_buffers, m_sampler,
            *m_font->font_atlas);
    }

    m_ui_pipeline = std::make_unique<ui::UIPipeline>(ctx, *m_swap_chain_manager,
                                                     m_window_dimension_buffers);
}

RenderEngine::~RenderEngine() {}

void RenderEngine::render(
    const std::vector<std::reference_wrapper<const RenderBody>> &bodies) {
    auto &circle_instance_buffer = m_geometry_pipeline->get_circle_instance_buffer();
    auto &triangle_instance_buffer = m_geometry_pipeline->get_triangle_instance_buffer();
    auto &rectangle_instance_buffer =
        m_geometry_pipeline->get_rectangle_instance_buffer();
    auto &hexagon_instance_buffer = m_geometry_pipeline->get_hexagon_instance_buffer();

    circle_instance_buffer.clear();
    triangle_instance_buffer.clear();
    rectangle_instance_buffer.clear();
    hexagon_instance_buffer.clear();

    for (auto b : bodies) {
        auto deref_b = b.get();
        switch (deref_b.shape.encode_shape_type()) {
        case ShapeTypeEncoding::CircleShape:
            circle_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                deref_b.rotation, deref_b.shape,
                                                deref_b.uvwt);
            break;
        case ShapeTypeEncoding::TriangleShape:
            triangle_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                  deref_b.rotation, deref_b.shape,
                                                  deref_b.uvwt);
            break;
        case ShapeTypeEncoding::RectangleShape:
            rectangle_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                   deref_b.rotation, deref_b.shape,
                                                   deref_b.uvwt);
            break;
        case ShapeTypeEncoding::HexagonShape:
            hexagon_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                 deref_b.rotation, deref_b.shape,
                                                 deref_b.uvwt);
            break;
        };
    }

    circle_instance_buffer.transfer();
    triangle_instance_buffer.transfer();
    rectangle_instance_buffer.transfer();
    hexagon_instance_buffer.transfer();

    auto &command_buffer = m_current_render_pass.command_buffer.m_command_buffer;

    m_geometry_pipeline->render_circles(command_buffer);
    m_geometry_pipeline->render_triangles(command_buffer);
    m_geometry_pipeline->render_rectangles(command_buffer);
    m_geometry_pipeline->render_hexagons(command_buffer);
}

void RenderEngine::text_kerning(const font::KerningMap &kerning_map,
                                const std::string_view text,
                                const ui::ElementProperties properties) {
    if (text.size() == 0) {
        return;
    }

    const auto font_color = properties.font.color;
    const auto font_size = properties.font.size;
    const float font_rotation = properties.font.rotation;

    auto &glyph_instance_buffer = m_text_pipeline->get_character_buffer();
    auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();
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

// TODO: This function is not compatible with render_ui. It is not possible to run them in
// the same loop iteration as they overwrite their buffers
void RenderEngine::render_text(const ui::TextBox &text_box) {
    auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
    auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();
    character_instance_buffer.clear();
    text_segment_buffer.clear();

    text_kerning(font::get_default_kerning_map(), text_box.text, text_box.properties);

    character_instance_buffer.transfer();
    text_segment_buffer.transfer();

    m_text_pipeline->render_text(m_current_render_pass.command_buffer.m_command_buffer);
}

// TODO: Should we instead simply pass the UI class instead of its state?
void RenderEngine::render_ui(const ui::State &state) {
    auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
    auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();
    character_instance_buffer.clear();
    text_segment_buffer.clear();

    const auto kerning_map = m_font->kerning_map;
    for (const auto button : state.buttons) {

        m_ui_pipeline->render(m_current_render_pass.command_buffer.m_command_buffer,
                              button->properties.container);

        text_kerning(font::get_default_kerning_map(), button->text, button->properties);
    }

    for (const auto text_box : state.text_boxes) {

        text_kerning(font::get_default_kerning_map(), text_box->text,
                     text_box->properties);
    }

    character_instance_buffer.transfer();
    text_segment_buffer.transfer();

    m_text_pipeline->render_text(m_current_render_pass.command_buffer.m_command_buffer);
}

bool RenderEngine::begin_render_pass(DeviceQueues &m_device_queues) {
    auto command_buffer_ = m_swap_chain_manager->get_command_buffer();
    if (!command_buffer_.has_value()) {
        return false;
    }
    CommandBuffer command_buffer = std::move(command_buffer_.value());

    command_buffer.begin_render_pass();
    command_buffer.set_viewport(
        Dimension::from_extent2d(m_swap_chain_manager->m_swap_chain.m_extent));
    command_buffer.set_scissor(m_swap_chain_manager->m_swap_chain.m_extent);

    m_current_render_pass.command_buffer = std::move(command_buffer);

    return true;
}

bool RenderEngine::end_render_pass(DeviceQueues &m_device_queues) {
    auto &command_buffer = m_current_render_pass.command_buffer;

    command_buffer.end_render_pass();
    command_buffer.submit_render_pass(m_device_queues.graphics_queue);
    VkResult result = command_buffer.present_render_pass(m_device_queues.present_queue);

    // TODO: framebufferResized needs to be set from the window callback when the
    // window is resized
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        framebuffer_resized) {
        m_swap_chain_manager->recreate_swap_chain();

        return false;
        // It is important to do this after vkQueuePresentKHR to ensure that the
        // semaphores are in a consistent state, otherwise a signaled semaphore
        // may never be properly waited upon.
        framebuffer_resized = false;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    return true;
}
