#include "EventState.h"
#include "../EventFactory.h"
#include "../GameState.h"

void EventState::on_enter(GameState &game_state) {
    DEBUG_ASSERT(m_text_renderer != nullptr,
                 "Error: Text renderer is not set in EventState.");
    DEBUG_ASSERT(m_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in EventState.");

    m_pending_transition = std::nullopt;
    event = EventFactory::my_first_event(m_geometry_renderer, m_text_renderer,
                                         m_text_renderer2, game_state, *this);
}

void EventState::on_exit(GameState &game_state) {
    DEBUG_ASSERT(m_text_renderer != nullptr,
                 "Error: Text renderer is not set in EventState.");
    DEBUG_ASSERT(m_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in EventState.");
    event->remove_event();
    event = std::nullopt;
};

util::StateTransition EventState::update(const float dt, GameState &game_state) {

    if (game_state.cursor.click_point.has_value()) {
        const interface::NDCPoint cursor_ndc_point =
            game_state.camera.to_ndc_point(game_state.cursor.click_point.value());
        event->on_click(cursor_ndc_point);
        game_state.cursor.click_point = std::nullopt;
    }

    const interface::NDCPoint cursor_ndc_point =
        game_state.camera.to_ndc_point(game_state.cursor.viewport_position);
    event->on_hover(cursor_ndc_point);

    if (m_pending_transition.has_value()) {
        auto transition = m_pending_transition.value();
        m_pending_transition = std::nullopt;
        return transition;
    }

    return util::StateTransition::none();
}
