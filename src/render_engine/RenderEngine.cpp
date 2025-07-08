#include "render_engine/RenderEngine.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/Window.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/images/ImageResource.h"
#include "render_engine/ui/TextBox.h"
#include "render_engine/ui/TextPipeline.h"
#include "render_engine/ui/UIPipeline.h"
#include "vulkan/vulkan_core.h"
#include <memory>

UniformBufferCollection create_uniform_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               Window &window);

RenderEngine::RenderEngine(const WindowConfig &window_config, const UseFont use_font)
    : m_window(Window(window_config)),
      m_ctx(std::make_shared<CoreGraphicsContext>(true, m_window)),
      m_window_dimension_buffers(std::move(create_uniform_buffers(m_ctx, m_window))),
      m_swap_chain_manager(SwapChainManager(m_ctx, m_window)), m_sampler(Sampler(m_ctx)) {

    register_all_fonts();
    register_all_images();
    register_all_shaders();

    m_device_queues = m_ctx->get_device_queues();

    auto descriptor_set_layout_builder =
        DescriptorSetLayoutBuilder()
            .add(StorageBuffer<StorageBufferObject>::create_descriptor_set_layout_binding(
                0))
            .add(UniformBuffer::create_descriptor_set_layout_binding(1))
            .add(Sampler::create_descriptor_set_layout_binding(2));

    auto &resource_manager = ResourceManager::get_instance();
    auto dog_image = resource_manager.get_resource<ImageResource>("DogImage");
    m_texture = Texture::unique_from_image_resource(
        m_ctx, m_swap_chain_manager, m_device_queues.graphics_queue, dog_image);

    m_geometry_descriptor_set_layout = descriptor_set_layout_builder.build(m_ctx.get());
    m_geometry_pipeline = std::make_unique<GeometryPipeline>(
        m_window, m_ctx, m_swap_chain_manager, *m_window_dimension_buffers,
        m_geometry_descriptor_set_layout, m_sampler, *m_texture);

    switch (use_font) {
    case UseFont::Default: {
        auto default_font = resource_manager.get_resource<FontResource>("DefaultFont");
        m_font = std::make_unique<Font>(m_ctx, m_swap_chain_manager,
                                        m_device_queues.graphics_queue, default_font);
        break;
    }
    default:
        m_font = nullptr;
        break;
    }

    if (m_font != nullptr) {
        m_text_pipeline = std::make_unique<ui::TextPipeline>(
            m_window, m_ctx, m_swap_chain_manager, *m_window_dimension_buffers, m_sampler,
            *m_font->font_atlas);
    }

    m_ui_pipeline = std::make_unique<ui::UIPipeline>(m_ctx, m_swap_chain_manager,
                                                     *m_window_dimension_buffers);
}

RenderEngine::~RenderEngine() {
    vkDestroyDescriptorSetLayout(m_ctx->device, m_geometry_descriptor_set_layout,
                                 nullptr);
}

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

// TODO: This function is not compatible with render_ui. It is not possible to run them in
// the same loop iteration as they overwrite their buffers
void RenderEngine::render_text(const ui::TextBox &text_box) {

    auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
    auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();
    character_instance_buffer.clear();
    text_segment_buffer.clear();

    const auto font_color = text_box.properties.font.color;
    const auto font_size = text_box.properties.font.size;
    const float font_rotation = text_box.properties.font.rotation;
    const auto center = text_box.properties.container.center;
    const glm::vec3 offset(text_box.properties.font.size / 2.5, 0.0f, 0.0f);

    text_segment_buffer.emplace_back(font_color, font_size, font_rotation);
    text_segment_buffer.transfer();

    const uint32_t text_segment_idx = 0;
    const auto text_start_x =
        center.x - (static_cast<float>((text_box.text.size() - 1)) * offset.x / 2);
    const glm::vec3 loc(text_start_x, center.y, 0.0f);
    float count = 0;
    for (const char &c : text_box.text) {
        character_instance_buffer.emplace_back(loc + offset * count,
                                               m_font->encode_ascii_char(std::toupper(c)),
                                               text_segment_idx);
        count += 1.0f;
    }

    character_instance_buffer.transfer();

    m_text_pipeline->render_text(m_current_render_pass.command_buffer.m_command_buffer);
}

void RenderEngine::render_ui(const ui::State &state) {
    auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
    auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();
    character_instance_buffer.clear();
    text_segment_buffer.clear();

    for (const auto button : state.buttons) {

        m_ui_pipeline->render(m_current_render_pass.command_buffer.m_command_buffer,
                              button->properties.container);

        const auto font_color = button->properties.font.color;
        const auto font_size = button->properties.font.size;
        const auto font_rotation = button->properties.font.rotation;
        const auto font_weight = button->properties.font.weight;
        const auto font_sharpness = button->properties.font.sharpness;
        text_segment_buffer.emplace_back(font_color, font_size, font_rotation,
                                         font_weight, font_sharpness);
        const uint32_t text_segment_idx = text_segment_buffer.num_elements() - 1;

        // TODO: Implement text kerning
        const auto center = button->properties.container.center;
        const auto text = button->text;
        const glm::vec3 offset(font_size / 2.5, 0.0f, 0.0f);
        const auto text_start_x =
            center.x - (static_cast<float>((text.size() - 1)) * offset.x / 2);
        const glm::vec3 loc(text_start_x, center.y, 0.0f);
        float count = 0;
        for (const char &c : text) {
            character_instance_buffer.emplace_back(
                loc + offset * count, m_font->encode_ascii_char(std::toupper(c)),
                text_segment_idx);
            count += 1.0f;
        }
    }

    for (const auto text_box : state.text_boxes) {
        const auto font_color = text_box->properties.font.color;
        const auto font_size = text_box->properties.font.size;
        const float font_rotation = text_box->properties.font.rotation;
        const auto font_weight = text_box->properties.font.weight;
        const auto font_sharpness = text_box->properties.font.sharpness;

        text_segment_buffer.emplace_back(font_color, font_size, font_rotation,
                                         font_weight, font_sharpness);
        const uint32_t text_segment_idx = text_segment_buffer.num_elements() - 1;

        // TODO: Implement text kerning
        const auto center = text_box->properties.container.center;
        const glm::vec3 offset(text_box->properties.font.size / 2.5, 0.0f, 0.0f);
        const auto text_start_x =
            center.x - (static_cast<float>((text_box->text.size() - 1)) * offset.x / 2);
        const glm::vec3 loc(text_start_x, center.y, 0.0f);
        float count = 0;
        for (const char &c : text_box->text) {
            character_instance_buffer.emplace_back(
                loc + offset * count, m_font->encode_ascii_char(std::toupper(c)),
                text_segment_idx);
            count += 1.0f;
        }
    }

    character_instance_buffer.transfer();
    text_segment_buffer.transfer();

    m_text_pipeline->render_text(m_current_render_pass.command_buffer.m_command_buffer);
}

void RenderEngine::wait_idle() { m_ctx->wait_idle(); }

bool RenderEngine::should_window_close() { return m_window.should_window_close(); }

void RenderEngine::process_window_events() { m_window.process_window_events(); }

void RenderEngine::register_mouse_event_callback(MouseEventCallbackFn cb) {
    m_window.register_mouse_event_callback(cb);
}

void RenderEngine::register_keyboard_event_callback(KeyboardEventCallbackFn cb) {
    m_window.register_keyboard_event_callback(cb);
}

UniformBufferCollection create_uniform_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               Window &window) {
    auto [width, height] = window.dimensions();
    UniformBufferObject ubo{
        .dimensions = glm::vec2(static_cast<float>(width), static_cast<float>(height))};
    auto uniform_buffers = std::make_unique<std::vector<UniformBuffer>>();
    uniform_buffers->reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniform_buffers->emplace_back(ctx, sizeof(UniformBufferObject));
        uniform_buffers->at(i).update_uniform_buffer(ubo);
    }
    return std::move(uniform_buffers);
}

bool RenderEngine::begin_render_pass() {
    auto command_buffer_ = m_swap_chain_manager.get_command_buffer();
    if (!command_buffer_.has_value()) {
        return false;
    }
    CommandBuffer command_buffer = std::move(command_buffer_.value());

    command_buffer.begin_render_pass();
    command_buffer.set_viewport(
        Dimension::from_extent2d(m_swap_chain_manager.m_swap_chain.m_extent));
    command_buffer.set_scissor(m_swap_chain_manager.m_swap_chain.m_extent);

    m_current_render_pass.command_buffer = std::move(command_buffer);

    return true;
}

bool RenderEngine::end_render_pass() {
    auto &command_buffer = m_current_render_pass.command_buffer;

    command_buffer.end_render_pass();
    command_buffer.submit_render_pass(m_device_queues.graphics_queue);
    VkResult result = command_buffer.present_render_pass(m_device_queues.present_queue);

    // TODO: framebufferResized needs to be set from the window callback when the
    // window is resized
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        framebuffer_resized) {
        m_swap_chain_manager.recreate_swap_chain();

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
