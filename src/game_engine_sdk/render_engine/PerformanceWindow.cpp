#include "game_engine_sdk/render_engine/PerformanceWindow.h"
#include "game_engine_sdk/Dimension.h"
#include "game_engine_sdk/render_engine/ui/Menu.h"
#include "game_engine_sdk/render_engine/ui/UI.h"
#include "glm/glm.hpp"

PerformanceWindow::PerformanceWindow() {
    const glm::vec3 header_dimension = glm::vec3(150.0f, 20.0f, 0.0f);
    const glm::vec3 header_position = glm::vec3(-300.0f, 370.0f, 0.0f);
    const glm::vec3 header_text = glm::vec3(header_position.x, header_position.y, 0.0f);

    const glm::vec3 body_dimension = glm::vec3(header_dimension.x, 45.0f, 0.0f);

    const glm::vec3 header_body_position_offset =
        glm::vec3(0.0f, (header_dimension.y / 2.0f) + (body_dimension.y / 2.0f), 0.0f);

    const glm::vec3 body_position = header_position - header_body_position_offset;

    const auto font_props = ui::ElementProperties::FontProperties{
        .color = colors::WHITE, .size = 24, .weight = 1.0f};

    m_components.push_back(graphics_pipeline::GeometryInstanceBufferObject{
        .center = header_position,
        .dimension = Dimension(header_dimension.x, header_dimension.y),
        .color = colors::BLUE,
    });

    m_components.push_back(graphics_pipeline::GeometryInstanceBufferObject{
        .center = body_position,
        .dimension = Dimension(body_dimension.x, body_dimension.y),
        .color = colors::rgba(0.2f, 0.2f, 0.2f, 0.2f),
    });

    auto m_menu = ui::Menu();
    m_ui = ui::UI(m_menu);
    m_ui.add_text_box(
        "HEADER", ui::TextBox("PERFORMANCE",
                              ui::ElementProperties{
                                  .container = {.center = header_text,
                                                .dimension = glm::vec2(200.0f, 200.0f)},
                                  .font = font_props}));

    const auto base_header_body_text_offset = glm::vec3(0.0f, 5.0f, 0.0f);
    const auto text_offset = glm::vec3(0.0f, 18.0f, 0.0f);
    auto left_alignment = glm::vec3(-5.0f, 0.0, 0.0f);
    m_ui.add_text_box(
        "FPS",
        ui::TextBox("FPS", ui::ElementProperties{
                               .container = {.center = header_text -
                                                       base_header_body_text_offset -
                                                       text_offset + left_alignment,
                                             .dimension = glm::vec2(200.0f, 200.0f)},
                               .font = font_props}));

    auto right_alignment = glm::vec3(36.0f, 0.0, 0.0f);
    m_ui.add_text_box(
        FPS_VALUE_TAG,
        ui::TextBox(
            "60", ui::ElementProperties{
                      .container = {.center = header_text - base_header_body_text_offset -
                                              text_offset + right_alignment,
                                    .dimension = glm::vec2(200.0f, 200.0f)},
                      .font = font_props}));

    left_alignment = glm::vec3(-30.0f, 0.0, 0.0f);
    m_ui.add_text_box(
        "FRAME_TIME",
        ui::TextBox(
            "FRAME TIME",
            ui::ElementProperties{
                .container = {.center = header_text - base_header_body_text_offset +
                                        left_alignment - text_offset * 2.0f,
                              .dimension = glm::vec2(200.0f, 200.0f)},
                .font = font_props}));

    right_alignment = glm::vec3(45.0f, 0.0f, 0.0f);
    m_ui.add_text_box(
        FRAME_TIME_VALUE_TAG,
        ui::TextBox(
            "00.0",
            ui::ElementProperties{
                .container = {.center = header_text - base_header_body_text_offset +
                                        right_alignment - text_offset * 2.0f,
                              .dimension = glm::vec2(200.0f, 200.0f)},
                .font = font_props}));

    m_last_update = Clock::now();
    m_update_interval_s = 0.5f;
    m_frame_count = 0.0f;

    m_last_fps = "60";
    m_last_frame_time = "0.16";
}

void PerformanceWindow::render(graphics_pipeline::GeometryPipeline *geometry_pipeline,
                               graphics_pipeline::TextPipeline *text_pipeline,
                               vulkan::CommandBuffer &command_buffer) {

    TimePoint now = Clock::now();
    Duration elapsed = now - m_last_update;

    m_frame_count += 1.0f;

    if (elapsed.count() > m_update_interval_s) {
        m_last_frame_time = std::format("{:.3f}", elapsed.count() / m_frame_count);
        m_last_fps =
            std::format("{:3}", static_cast<uint32_t>(m_frame_count / elapsed.count()));
        m_frame_count = 0.0f;
        m_last_update = Clock::now();
    }

    m_ui.get_text_box(FRAME_TIME_VALUE_TAG).text = m_last_frame_time;
    m_ui.get_text_box(FPS_VALUE_TAG).text = m_last_fps;

    auto &rectangle_instance_buffer = geometry_pipeline->get_rectangle_instance_buffer();

    for (const auto &comp : m_components) {
        rectangle_instance_buffer.push_back(comp);
    }
    auto &ui_state = m_ui.get_state();
    for (const auto text_box : ui_state.text_boxes) {
        text_pipeline->text_kerning(text_box->text, text_box->properties);
    }
}
