#include "IntroState.h"
#include "../GameState.h"
#include "../events/DialogFactory.h"
#include "DefendState.h"

void IntroState::on_enter(GameState &game_state) {
    DEBUG_ASSERT(m_text_renderer2 != nullptr,
                 "Error: Text renderer is not set in IntroState.");
    DEBUG_ASSERT(m_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in IntroState.");

    m_pending_transition = std::nullopt;
}

void IntroState::on_exit(GameState &game_state) {
    DEBUG_ASSERT(m_text_renderer2 != nullptr,
                 "Error: Text renderer is not set in IntroState.");
    DEBUG_ASSERT(m_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in IntroState.");
    event = std::nullopt;
};

util::StateTransition IntroState::update(const float dt, GameState &game_state) {

    if (!event.has_value()) {
        event = create_intro_contract(game_state, *this);
    }

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

Event IntroState::create_intro_contract(GameState &game_state, IntroState &event_state) {

    const float dialog_font_size = game_state.camera.to_ndc_width(22);
    const math::Vector2 dialog_box_center = interface::NDCPoint(0, 0);
    const math::Vector2 dialog_box_size(1.80f, 1.20f);
    const math::Vector2 dialog_box_content_padding(0.03f, 0.04f);
    const math::Vector2 dialog_box_content_size =
        dialog_box_size - dialog_box_content_padding * 2.0f;

    const math::Vector2 dialog_line_size(dialog_box_content_size.x(),
                                         dialog_font_size * 1.0f);

    const float content_top_y_ndc = dialog_box_center.y() - dialog_box_size.y() / 2.0f +
                                    dialog_box_content_padding.y() * 2.0f;
    const float content_bottom_y_ndc = dialog_box_center.y() +
                                       dialog_box_size.y() / 2.0f +
                                       dialog_box_content_padding.y();
    const float dialog_text_start_x_ndc = dialog_box_center.x() -
                                          dialog_box_size.x() / 2.0f +
                                          dialog_box_content_padding.x();
    const float text_start_y_ndc = content_top_y_ndc + dialog_line_size.y();
    const float text_start_y_options_ndc =
        content_bottom_y_ndc - dialog_line_size.y() * 3;

    auto event_desc_opts = font::TextOpts{};
    event_desc_opts.position =
        interface::NDCPoint(dialog_text_start_x_ndc, text_start_y_ndc);
    event_desc_opts.font_color = util::colors::WHITE;
    event_desc_opts.font_size = dialog_font_size;
    event_desc_opts.line_width = dialog_line_size.x();
    event_desc_opts.line_height = dialog_line_size.y();

    font::TextOpts event_dialog_option_opts{};
    event_dialog_option_opts.position = interface::NDCPoint(
        dialog_text_start_x_ndc, text_start_y_options_ndc - dialog_line_size.y());
    event_dialog_option_opts.font_color = util::colors::WHITE;
    event_dialog_option_opts.font_size = dialog_font_size;
    event_dialog_option_opts.line_width = dialog_line_size.x();
    event_dialog_option_opts.line_height = dialog_line_size.y();

    const auto quit_event = [&event_state]() {
        event_state.set_pending_transition(util::StateTransition::to<DefendState>());
    };

    DEBUG_ASSERT(
        m_geometry_renderer != nullptr,
        "Error: Creating intro contract with geometry renderer pointer as nullptr.");
    DEBUG_ASSERT(m_text_renderer2 != nullptr,
                 "Error: Creating intro contract with text renderer pointer as nullptr.");
    Event event;

    event.create_dialog_bbox(game_state.camera);

    // Consistent, same every time for all events
    DialogFactory dialog_factory = DialogFactory(&m_text_renderer2->m_font);
    dialog_factory.set_event_dialog_text_opts(event_desc_opts);
    dialog_factory.set_event_dialog_option_text_opts(event_dialog_option_opts);

    std::string node_a_id = "A";
    dialog_factory.set_id(node_a_id);
    dialog_factory.set_dialog_text(
        "[SYSTEM NOTIFICATION: CONTRACT BRIEFING]\n"
        "MISSION ID: L0N3LY L1ZARD5\n"
        "ISSUER: NEW KESTREL OUTPOST\n"
        "ACCEPT: Lonely Lizards\n"
        "STATUS: Pending Acknowledgement\n"
        "\n"
        "MANDATE: Transport Refined Helium-3 from New Kestrel to The Exclusion Ring "
        "via the Obsidian Flank.\n"
        "THREAT ASSESSMENT: High. Previous shipments lost to Stray Pack interference in "
        "this sector.\n"
        "ENGAGEMENT PROTOCOL: Protect the shipment. Your defense systems are the only "
        "power source sustaining these settlements. Failure results in collapse."
        "\n"
        "\n"
        "[AUTHORIZATION: NEW KESTREL COMMAND]");
    dialog_factory.add_dialog_option("exit0", "Accept the contract.", std::nullopt,
                                     quit_event);
    event.add_dialog_node(dialog_factory.build());

    event.set_active_node(node_a_id);
    return event;
}
