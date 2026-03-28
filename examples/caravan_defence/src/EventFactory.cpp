#include "EventFactory.h"
#include "GameState.h"
#include "events/DialogFactory.h"
#include "events/Event.h"
#include "states/DefendState.h"
#include "states/EventState.h"

Event EventFactory::my_first_event(
    graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer,
    graphics_pipeline::text::TextRenderer *text_renderer, GameState &game_state,
    EventState &event_state) {

    const float dialog_font_size = game_state.camera.to_ndc_width(22);
    const math::Vector2 dialog_box_center = interface::NDCPoint(0, 0);
    const math::Vector2 dialog_box_size(1.80f, 1.20f);
    const math::Vector2 dialog_box_content_padding(0.03f, 0.04f);
    const math::Vector2 dialog_box_content_size =
        dialog_box_size - dialog_box_content_padding * 2.0f;

    const math::Vector2 dialog_line_size(dialog_box_content_size.x(), dialog_font_size);

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

    Event event = Event(geom_renderer, text_renderer);

    event.create_dialog_bbox(game_state.camera);

    // Consistent, same every time for all events
    DialogFactory dialog_factory = DialogFactory(text_renderer);
    dialog_factory.set_event_dialog_text_opts(event_desc_opts);
    dialog_factory.set_event_dialog_option_text_opts(event_dialog_option_opts);

    std::string node_a_id = "A";
    dialog_factory.set_id(node_a_id);
    dialog_factory.set_dialog_text(
        "This is a really really really long event description that spans multiple "
        "lines about an interesting event.");
    dialog_factory.add_dialog_option("next0", "Next.", "B", []() {});
    dialog_factory.add_dialog_option("exit0", "Exit the event.", std::nullopt,
                                     quit_event);
    event.add_dialog_node(dialog_factory.build());

    dialog_factory.set_id("B");
    dialog_factory.set_dialog_text("...");
    dialog_factory.add_dialog_option("exit1", "Exit again.", std::nullopt, quit_event);
    event.add_dialog_node(dialog_factory.build());

    event.set_active_node(node_a_id);

    return event;
}

Event EventFactory::the_scavenger_ambush(
    graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer,
    graphics_pipeline::text::TextRenderer *text_renderer, GameState &game_state,
    EventState &event_state) {

    const float dialog_font_size = game_state.camera.to_ndc_width(22);
    const math::Vector2 dialog_box_center = interface::NDCPoint(0, 0);
    const math::Vector2 dialog_box_size(1.80f, 1.20f);
    const math::Vector2 dialog_box_content_padding(0.03f, 0.04f);
    const math::Vector2 dialog_box_content_size =
        dialog_box_size - dialog_box_content_padding * 2.0f;

    const math::Vector2 dialog_line_size(dialog_box_content_size.x(), dialog_font_size);

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

    Event event = Event(geom_renderer, text_renderer);

    event.create_dialog_bbox(game_state.camera);

    // Consistent, same every time for all events
    DialogFactory dialog_factory = DialogFactory(text_renderer);
    dialog_factory.set_event_dialog_text_opts(event_desc_opts);
    dialog_factory.set_event_dialog_option_text_opts(event_dialog_option_opts);

    std::string node_a_id = "A";
    dialog_factory.set_id(node_a_id);
    dialog_factory.set_dialog_text(
        "On the path ahead you see a wreckage scattered all over the ground. A group "
        "of what seems to be ill-intended humanoid creatures are scavaging the "
        "remains of the once heavily defended caravan. They have yet to notice "
        "you.");
    dialog_factory.add_dialog_option("A0", "1. Make them aware of your presence.", "1",
                                     []() {});
    dialog_factory.add_dialog_option("A1", "2. Attack the creatures.", std::nullopt,
                                     quit_event);
    dialog_factory.add_dialog_option("A2", "3. Find another way around.", std::nullopt,
                                     quit_event);
    event.add_dialog_node(dialog_factory.build());

    // Alternative 1, the creatures attack
    dialog_factory.set_id("1");
    dialog_factory.set_dialog_text(
        "Your engine rumbles, shaking dust from the gravel plating around you. The "
        "scavengers freeze as their primitive scanners detect the proximity alarm "
        "trigger. The creatures look at you with surprised expressions. One of them "
        "starts yelling in a foreign language as the other starts surrounding you. "
        "They attack immediately.");
    dialog_factory.add_dialog_option("10", "Defend the caravan.", std::nullopt,
                                     quit_event);
    event.add_dialog_node(dialog_factory.build());

    // Alternative 2, the creatures negotiate
    dialog_factory.set_id("2");
    dialog_factory.set_dialog_text(
        "One of the creatures approaches you, clutching a smoking pistol like a "
        "sheriff's badge. They speak with the rough cadence of frontier folk in this "
        "wide west type living environment. Looks like you've caught them red "
        "handed. If you are willing to look the other direction, they could "
        "compensate you accordingly.");
    dialog_factory.add_dialog_option("20", "1. Accept the bribe.", std::nullopt,
                                     quit_event);
    dialog_factory.add_dialog_option("21", "2. Demand a piece of the loot.", std::nullopt,
                                     quit_event);
    dialog_factory.add_dialog_option("22", "3. Don't let them get away with this.",
                                     std::nullopt, quit_event);
    event.add_dialog_node(dialog_factory.build());

    // Alternative 3, the creatures flee
    dialog_factory.set_id("3");
    dialog_factory.set_dialog_text(
        "The creatures all look up at you with fear in their eyes. They aren't "
        "afraid of your size, but having sustain major losses during the initial "
        "fight they are now unprepared. Scattering moments later, they disappear "
        "without a trace into the dense undergrowth or space-shackles.");
    dialog_factory.add_dialog_option("30", "1. Search the site..", std::nullopt,
                                     quit_event);
    event.add_dialog_node(dialog_factory.build());

    event.set_active_node(node_a_id);

    return event;
}

Event EventFactory::defectors_den(
    graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer,
    graphics_pipeline::text::TextRenderer *text_renderer, GameState &game_state,
    EventState &event_state) {

    const float dialog_font_size = game_state.camera.to_ndc_width(22);
    const math::Vector2 dialog_box_center = interface::NDCPoint(0, 0);
    const math::Vector2 dialog_box_size(1.80f, 1.20f);
    const math::Vector2 dialog_box_content_padding(0.03f, 0.04f);
    const math::Vector2 dialog_box_content_size =
        dialog_box_size - dialog_box_content_padding * 2.0f;

    const math::Vector2 dialog_line_size(dialog_box_content_size.x(), dialog_font_size);

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

    Event event = Event(geom_renderer, text_renderer);

    event.create_dialog_bbox(game_state.camera);

    // Consistent, same every time for all events
    DialogFactory dialog_factory = DialogFactory(text_renderer);
    dialog_factory.set_event_dialog_text_opts(event_desc_opts);
    dialog_factory.set_event_dialog_option_text_opts(event_dialog_option_opts);

    std::string node_a_id = "A";
    dialog_factory.set_id(node_a_id);
    dialog_factory.set_dialog_text(
        "The camp is quiet, save for the hum of dormant plasma generators scavenged from "
        "collapsed colony ships. Bandits are resting or repairing weapons around a "
        "central fire, but their tension is high. A wounded figure lies slumped behind a "
        "crate; they wear your logistics company's jacket but lack the uniform badge, "
        "indicating they were discharged or defected. The camp represents a resource "
        "opportunity: their scavenged equipment is worth a lot, but eliminating "
        "them all would trigger a heavy combat response.");
    dialog_factory.add_dialog_option("A0", "1. Negotiate for the capture.", "1", []() {});
    dialog_factory.add_dialog_option("A1", "2. Attack the camp.", std::nullopt,
                                     quit_event);
    dialog_factory.add_dialog_option("A2", "3. Leave.", std::nullopt, quit_event);
    event.add_dialog_node(dialog_factory.build());

    // Alternative 1, the creatures attack
    dialog_factory.set_id("1");
    dialog_factory.set_dialog_text(
        "You approach the bandits querying about the worth of the capture. They are "
        "willing to let him go for 10,000 credits.");
    dialog_factory.add_dialog_option("10", "1. Accept.", std::nullopt, quit_event);
    dialog_factory.add_dialog_option("11", "2. Deny.", std::nullopt, quit_event);
    event.add_dialog_node(dialog_factory.build());

    event.set_active_node(node_a_id);

    return event;
}
