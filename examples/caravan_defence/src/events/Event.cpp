#include "Event.h"
#include "math/Matrix.h"

Event::Event(Event &&other) noexcept
    : m_geometry_renderer(std::move(other.m_geometry_renderer)),
      m_text_renderer(std::move(other.m_text_renderer)),
      m_bbox_render_data(std::move(other.m_bbox_render_data)),
      m_nodes(std::move(other.m_nodes)), m_current_node(std::move(other.m_current_node)) {
    other.m_geometry_renderer = nullptr;
    other.m_text_renderer = nullptr;
}

Event &Event::operator=(Event &&other) noexcept {
    if (this != &other) {
        remove_event();

        m_geometry_renderer = std::move(other.m_geometry_renderer);
        m_text_renderer = std::move(other.m_text_renderer);
        m_bbox_render_data = std::move(other.m_bbox_render_data);
        m_nodes = std::move(other.m_nodes);
        m_current_node = std::move(other.m_current_node);

        other.m_geometry_renderer = nullptr;
        other.m_text_renderer = nullptr;
    }

    return *this;
}

void Event::create_dialog_bbox(const camera::Camera2D &camera) {
    const interface::NDCPoint box_center = interface::NDCPoint(0, 0);
    const float box_width_ndc = 1.80f;
    const float box_height_ndc = 1.20f;

    m_bbox_render_data = m_geometry_renderer->request_render_slot();
    auto &event_box_instance = m_geometry_renderer->get_instance(m_bbox_render_data);
    event_box_instance.model_matrix =
        math::Matrix().translate(box_center).scale(box_width_ndc, box_height_ndc);
    event_box_instance.color = util::colors::rgba(0.0f, 0.0f, 0.0f, 0.95f);
    event_box_instance.flags |=
        static_cast<uint32_t>(graphics_pipeline::geometry::GeometryShape::Rectangle);
    event_box_instance.border.width = camera.to_ndc_width(6.0f);
    event_box_instance.border.radius = camera.to_ndc_width(15.0f);
    event_box_instance.border.color = util::colors::WHITE;
}

void Event::add_dialog_node(DialogNode &&node) {
    m_nodes.insert_or_assign(node.id, std::move(node));
}

void Event::set_active_node(std::string &id) {
    if (m_nodes.find(id) != m_nodes.end()) {
        m_current_node = id;
    }
}

void Event::on_click(const interface::NDCPoint &point) {
    DialogNode &node = m_nodes[m_current_node];

    size_t clicked_option_id = node.get_option(point);
    if (clicked_option_id == std::numeric_limits<size_t>::max()) {
        return;
    }

    DialogOption &clicked_option = node.options[clicked_option_id];
    clicked_option.on_click();

    if (clicked_option.next_dialog_node.has_value()) {
        set_active_node(clicked_option.next_dialog_node.value());
    }
}

void Event::on_hover(const interface::NDCPoint &point) {
    DialogNode &node = m_nodes[m_current_node];
    size_t hover_option_id = node.get_option(point);
    for (size_t i = 0; i < node.options.size(); i++) {
        auto &text_instance =
            m_text_renderer->get_text_format_instance(node.options[i].text_handle);

        if (i == hover_option_id) {
            text_instance.font_color = util::colors::YELLOW;
        } else {
            text_instance.font_color = util::colors::WHITE;
        }
    }
}
