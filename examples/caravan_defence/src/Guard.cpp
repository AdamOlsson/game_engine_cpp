#include "Guard.h"

Guard::Guard(CaravanSlot *slot) : m_last_attack(Clock::now()) {
    DEBUG_ASSERT(slot != nullptr, "Error: Constructuring guard with nullptr");
    m_caravan_slot = slot;
    m_model_matrix = math::Matrix()
                         .translate(m_caravan_slot->get_world_position())
                         .scale(m_size.x, m_size.y, 1.0f);
    m_caravan_slot->set_occupying_guard(this);
}

bool Guard::is_point_inside(const camera::WorldPoint2D &point) {
    // World position with no regard to the world grid
    const camera::WorldPoint2D position = m_model_matrix.position_2d();
    // TODO: Can we fetch width and height from the model matrix?
    return math::is_point_inside_rectangle(point, position, m_size.x, m_size.y);
}

void Guard::set_render_data(graphics_pipeline::quad::QuadPipelineSBO *render_data) {
    if (render_data == nullptr) {
        return;
    }
    m_render_data = render_data;
    m_render_data->model_matrix = m_model_matrix;
    m_render_data->color = m_color;
}

void Guard::set_selected(const bool is_selected) {
    m_is_selected = is_selected;
    m_render_data->color = m_is_selected ? m_selected_color : m_color;
}

void Guard::toggle_selected() {
    m_is_selected = !m_is_selected;
    m_render_data->color = m_is_selected ? m_selected_color : m_color;
}

void Guard::set_world_position(const camera::WorldPoint2D &position) {
    m_model_matrix = math::Matrix().translate(position).scale(m_size.x, m_size.y, 1.0f);
    m_render_data->model_matrix = m_model_matrix;
}

camera::WorldPoint2D Guard::get_world_position() const {
    return m_model_matrix.position_2d();
}

void Guard::set_caravan_slot(CaravanSlot *slot) {
    m_caravan_slot = slot;
    set_world_position(m_caravan_slot->get_world_position());
}

CaravanSlot *Guard::get_caravan_slot() const { return m_caravan_slot; }

bool Guard::in_attack_range(const camera::WorldPoint2D &point) const {
    const camera::WorldPoint2D &guard_position = get_world_position();
    return math::distance2(guard_position, point) < (m_attack_range * m_attack_range);
}

bool Guard::can_attack() {
    const Duration elapsed = Clock::now() - m_last_attack;
    return elapsed > m_attack_cooldown;
}

RangedAttack Guard::attack(const Enemy &e) {
    m_last_attack = Clock::now();
    return RangedAttack(get_world_position(), e.get_world_position());
}
