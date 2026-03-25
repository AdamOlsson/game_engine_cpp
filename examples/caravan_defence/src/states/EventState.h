#pragma once

#include "../events/Event.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "state_machine/StateTransition.h"

struct GameState;

class EventState {
  private:
    graphics_pipeline::text::TextRenderer *m_text_renderer = nullptr;
    graphics_pipeline::geometry::GeometryRenderer2 *m_geometry_renderer = nullptr;

    std::optional<Event> event = std::nullopt;
    std::optional<util::StateTransition> m_pending_transition = std::nullopt;

  public:
    EventState() = default;

    EventState(graphics_pipeline::text::TextRenderer *text_renderer,
               graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer)
        : m_text_renderer(text_renderer), m_geometry_renderer(geom_renderer) {}

    EventState(const EventState &) = delete;
    EventState(EventState &&) = default;
    EventState &operator=(const EventState &) = delete;
    EventState &operator=(EventState &&) = default;

    void on_enter(GameState &game_state);
    void on_exit(GameState &game_state);
    util::StateTransition update(const float dt, GameState &game_state);

    template <typename PushConstantType>
    void render_ui(const vulkan::CommandBuffer &command_buffer,
                   PushConstantType *push_constant) {
        if (event.has_value()) {
            event->render_geometry(command_buffer, push_constant);
            event->render_text(command_buffer, push_constant);
        }
    }

    void set_pending_transition(util::StateTransition transition) {
        m_pending_transition = transition;
    }
};
