#pragma once

#include "../events/Event.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/text/TextRenderer2.h"
#include "state_machine/StateTransition.h"

struct GameState;

class EventState {
  private:
    graphics_pipeline::text::TextRenderer2 *m_text_renderer2 = nullptr;
    graphics_pipeline::geometry::GeometryRenderer2 *m_geometry_renderer = nullptr;

    std::optional<Event> event = std::nullopt;
    std::optional<util::StateTransition> m_pending_transition = std::nullopt;

  public:
    EventState() = default;

    EventState(graphics_pipeline::text::TextRenderer2 *text_renderer2,
               graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer)
        : m_geometry_renderer(geom_renderer), m_text_renderer2(text_renderer2) {}

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
            std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> data;
            event->get_render_data(data);

            vulkan::DrawIndexedIndirectCommand draw_command =
                m_geometry_renderer->write_to_buffer(data);
            m_geometry_renderer->render_indirect(command_buffer, push_constant,
                                                 {draw_command});

            std::vector<font::TextFormat> formats;
            std::vector<font::Text> texts;
            event->get_text_render_data(formats, texts);

            m_text_renderer2->write_to_format_buffer(formats, 0);

            std::vector<vulkan::DrawIndexedIndirectCommand> draw_commands;
            size_t offset = 0;
            for (size_t i = 0; i < texts.size(); i++) {
                auto text_commands =
                    m_text_renderer2->write_to_glyph_buffer(texts[i], i, offset);
                offset += texts[i].glyphs.size();
                draw_commands.insert(draw_commands.end(), text_commands.begin(),
                                     text_commands.end());
            }

            m_text_renderer2->render_indirect(command_buffer, push_constant,
                                              draw_commands);
        }
    }

    void set_pending_transition(util::StateTransition transition) {
        m_pending_transition = transition;
    }
};
