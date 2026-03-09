# Transition to a state machine for game states in caravan_defence

**Session ID:** ses_32ee8eaf8ffeM9Frz8PAZH0Cko
**Created:** 3/9/2026, 6:34:36 AM
**Updated:** 3/9/2026, 7:34:38 AM

---

## User

In the game @examples/caravan_defence/src/main.cpp I have manage logical states through an enum GameState. You can see how it is used throughout the file. I would like to transition into a proper state machine. My requirement is that I should be easily be able to add state, transitions and logic within a state. How would you go about to implement this? 

---

## Assistant (Plan · big-pickle · 5.0s)

_Thinking:_

The user wants me to review their current GameState enum implementation in the caravan_defence example and propose a plan for transitioning to a proper state machine. They want to be able to easily add states, transitions, and logic within states.

Let me first read the main.cpp file to understand how the GameState enum is currently used.

**Tool: read**

**Input:**
```json
{
  "filePath": "/Users/adamolsson/projects/game_engine_cpp/examples/caravan_defence/src/main.cpp"
}
```

**Output:**
```
<file>
00001| #include "Entity.h"
00002| #include "camera/Camera.h"
00003| #include "events/DialogFactory.h"
00004| #include "events/Event.h"
00005| #include "game_engine_sdk/Game.h"
00006| #include "game_engine_sdk/GameEngine.h"
00007| #include "graphics_pipeline/quad/QuadRenderer.h"
00008| #include "graphics_pipeline/text/TextRenderer.h"
00009| #include "interface/NDCPosition.h"
00010| #include "vulkan/CommandBufferManager.h"
00011| #include <random>
00012| 
00013| #define ASSET_FILE(filename) ASSET_DIR "/" filename
00014| constexpr glm::vec2 INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
00015| constexpr float ZOOM_SCALE_FACTOR = 0.1f;
00016| 
00017| enum class GameState {
00018|     Playing,
00019|     Paused,
00020|     Event,
00021| };
00022| 
00023| constexpr vulkan::RenderPassOpts world_pass =
00024|     vulkan::RenderPassOpts{.color = {
00025|                                .load_op = VK_ATTACHMENT_LOAD_OP_CLEAR,
00026|                                .store_op = VK_ATTACHMENT_STORE_OP_STORE,
00027|                                .initial_layout = VK_IMAGE_LAYOUT_UNDEFINED,
00028|                                .final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
00029|                            }};
00030| 
00031| constexpr vulkan::RenderPassOpts ui_pass =
00032|     vulkan::RenderPassOpts{.color = {
00033|                                .load_op = VK_ATTACHMENT_LOAD_OP_LOAD,
00034|                                .store_op = VK_ATTACHMENT_STORE_OP_STORE,
00035|                                .initial_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
00036|                                .final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
00037|                            }};
00038| 
00039| // TODO: Move relevant members into the anonymous game state struct. Then pass
00040| // this game state to event construction.
00041| class CaravanDefence : public Game {
00042|   private:
00043|     std::unique_ptr<vulkan::SwapChain> m_swap_chain;
00044| 
00045|     std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;
00046| 
00047|     camera::Camera2D m_camera;
00048|     struct {
00049|         interface::ViewportPoint cursor_viewport_position =
00050|             interface::ViewportPoint(1e6, 1e6);
00051|         bool is_right_button_pressed = false;
00052|     } m_mouse_state;
00053| 
00054|     // World renderers
00055|     vulkan::RenderPass m_render_pass;
00056|     std::unique_ptr<graphics_pipeline::quad::QuadRenderer> m_quad_renderer;
00057|     std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_geom_renderer;
00058| 
00059|     // UI renderers
00060|     vulkan::RenderPass m_ui_render_pass;
00061|     std::unique_ptr<graphics_pipeline::text::TextRenderer> m_ui_text_renderer;
00062|     std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_ui_geom_renderer;
00063| 
00064|     std::vector<entity::Entity> m_caravan;
00065|     std::vector<entity::Entity> m_caravan_slots;
00066|     std::vector<entity::Entity> m_guards;
00067|     std::vector<entity::Entity> m_attacks;
00068|     std::vector<entity::Entity> m_enemies;
00069| 
00070|     struct {
00071| 
00072|         GameState last_state = GameState::Playing;
00073|         GameState state = GameState::Playing;
00074| 
00075|         std::optional<Event> event;
00076| 
00077|         size_t time_elapsed_ms;
00078|         std::optional<size_t> selected_guard = std::nullopt;
00079| 
00080|         struct {
00081|             std::random_device device;
00082|             std::mt19937 gen;
00083|             void init() { gen = std::mt19937(device()); }
00084|             float uniform(const float from, const float to) {
00085|                 std::uniform_real_distribution<float> dist(from, to);
00086|                 return dist(gen);
00087|             }
00088| 
00089|             int uniform(const int from, const int to) {
00090|                 std::uniform_int_distribution<int> dist(from, to);
00091|                 return dist(gen);
00092|             }
00093|         } rng;
00094|     } m_game_state;
00095| 
00096|   public:
00097|     CaravanDefence() {}
00098| 
00099|     ~CaravanDefence() {};
00100| 
00101|     void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {
00102| 
00103|         auto window_size = ctx->window->get_framebuffer_size<float>();
00104|         m_swap_chain = std::make_unique<vulkan::SwapChain>(ctx);
00105| 
00106|         m_render_pass = m_swap_chain->create_render_pass(world_pass);
00107|         m_ui_render_pass = m_swap_chain->create_render_pass(ui_pass);
00108| 
00109|         m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);
00110| 
00111|         m_camera = camera::Camera2D(window_size.width, window_size.height);
00112|         m_camera.set_zoom(0.1f);
00113| 
00114|         // World renderers
00115|         vulkan::PushConstantRange push_constant_range;
00116|         push_constant_range.offset = 0;
00117|         push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
00118|         push_constant_range.size = camera::Camera2D::matrix_size();
00119| 
00120|         graphics_pipeline::RendererOpts renderer_opts{};
00121|         renderer_opts.push_constant_range = push_constant_range;
00122|         renderer_opts.swap_chain.extent = m_swap_chain->get_extent();
00123|         renderer_opts.swap_chain.render_pass = &m_render_pass;
00124|         renderer_opts.quad.texture = graphics_pipeline::Texture::from_filepath(
00125|             ctx, m_command_buffer_manager.get(), ASSET_FILE("sprite_sheet.png"));
00126|         m_quad_renderer = std::make_unique<graphics_pipeline::quad::QuadRenderer>(
00127|             ctx, m_command_buffer_manager.get(), renderer_opts);
00128| 
00129|         m_geom_renderer = std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
00130|             ctx, m_command_buffer_manager.get(), renderer_opts);
00131| 
00132|         // UI renderers
00133|         graphics_pipeline::RendererOpts ui_renderer_opts{};
00134|         ui_renderer_opts.push_constant_range = push_constant_range;
00135|         ui_renderer_opts.swap_chain.extent = m_swap_chain->get_extent();
00136|         ui_renderer_opts.swap_chain.render_pass = &m_ui_render_pass;
00137| 
00138|         m_ui_text_renderer = std::make_unique<graphics_pipeline::text::TextRenderer>(
00139|             ctx, ui_renderer_opts);
00140|         m_ui_text_renderer->load_font(
00141|             m_command_buffer_manager.get(),
00142|             font::FontLoader(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf")));
00143| 
00144|         m_ui_geom_renderer =
00145|             std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
00146|                 ctx, m_command_buffer_manager.get(), ui_renderer_opts);
00147| 
00148|         const float slot_distance_x = 250.0f;
00149|         // Add entities
00150|         m_caravan.push_back(
00151|             entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 0.0f)));
00152|         m_caravan.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
00153|         m_caravan.back().set_uvwt(0.3f, 0.0f, 0.4f, 0.2f);
00154|         m_caravan_slots.push_back(
00155|             entity::Entity::create_caravan_slot(camera::WorldPoint2D(
00156|                 slot_distance_x, m_caravan.back().get_world_position().y)));
00157|         m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
00158|                                                m_geom_renderer.get());
00159|         m_caravan_slots.push_back(
00160|             entity::Entity::create_caravan_slot(camera::WorldPoint2D(
00161|                 -slot_distance_x, m_caravan.back().get_world_position().y)));
00162|         m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
00163|                                                m_geom_renderer.get());
00164| 
00165|         m_caravan.push_back(
00166|             entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 275.0f)));
00167|         m_caravan.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
00168|         m_caravan.back().set_uvwt(0.2f, 0.0f, 0.3f, 0.2f);
00169|         m_caravan_slots.push_back(
00170|             entity::Entity::create_caravan_slot(camera::WorldPoint2D(
00171|                 slot_distance_x, m_caravan.back().get_world_position().y)));
00172|         m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
00173|                                                m_geom_renderer.get());
00174|         m_caravan_slots.push_back(
00175|             entity::Entity::create_caravan_slot(camera::WorldPoint2D(
00176|                 -slot_distance_x, m_caravan.back().get_world_position().y)));
00177|         m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
00178|                                                m_geom_renderer.get());
00179| 
00180|         m_guards.reserve(16); // 16 is magic
00181|         m_guards.push_back(
00182|             entity::Entity::create_guard(camera::WorldPoint2D(0.0f, 0.0f)));
00183|         m_guards.push_back(
00184|             entity::Entity::create_guard(camera::WorldPoint2D(0.0f, 0.0f)));
00185| 
00186|         set_damage_type(m_guards[0], entity::DamageType::A);
00187|         set_damage_type(m_guards[1], entity::DamageType::B);
00188| 
00189|         m_enemies.reserve(64);
00190|         m_attacks.reserve(8);
00191| 
00192|         for (size_t i = 0; i < m_guards.size(); i++) {
00193|             m_guards[i].set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
00194|             entity::set_caravan_slot(m_guards[i], &m_caravan_slots[i]);
00195|             entity::set_occupying_guard(m_caravan_slots[i], &m_guards[i]);
00196|         }
00197| 
00198|         /*m_quad_renderer->sync_render_slots();*/
00199|         /*m_geom_renderer->sync_render_slots();*/
00200|         /*m_ui_geom_renderer->sync_render_slots();*/
00201| 
00202|         register_mouse_event_handler(ctx.get());
00203|         register_keyboard_event_handler(ctx.get());
00204|     }
00205| 
00206|     void spawn_group_of_enemies() {
00207|         const size_t num_enemies = m_game_state.rng.uniform(1, 3);
00208|         const float group_distance = m_game_state.rng.uniform(800.0f, 1200.0f);
00209|         const float group_angle = m_game_state.rng.uniform(-45.0f, 225.0f);
00210|         const math::Vector2 group_center =
00211|             math::Vector2(group_distance, 0.0f).rotate_z(group_angle);
00212| 
00213|         const int group_radius = 200;
00214|         for (size_t i = 0; i < num_enemies; i++) {
00215|             const float distance_from_group_center =
00216|                 m_game_state.rng.uniform(-group_radius, group_radius);
00217|             const float angle_from_group_center = m_game_state.rng.uniform(0, 360);
00218| 
00219|             const math::Vector2 enemy_position =
00220|                 math::Vector2(distance_from_group_center, 0.0f)
00221|                     .rotate_z(angle_from_group_center);
00222| 
00223|             const uint32_t enemy_type = m_game_state.rng.uniform(0, 1);
00224|             spawn_enemy(group_center + enemy_position,
00225|                         static_cast<entity::EnemyType>(enemy_type));
00226|         }
00227|     }
00228| 
00229|     void spawn_enemy(const math::Vector2 &position, const entity::EnemyType type) {
00230|         m_enemies.push_back(entity::Entity::create_enemy(position));
00231| 
00232|         const size_t caravan_cart_id = m_game_state.rng.uniform(0, 1);
00233|         m_enemies.back().set_move_target(m_caravan[caravan_cart_id].get_world_position());
00234|         entity::set_enemy_type(m_enemies.back(), type);
00235|         m_enemies.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
00236|     }
00237| 
00238|     void create_attack(entity::Entity &guard, entity::Entity &enemy) {
00239|         m_attacks.push_back(entity::attack(guard, enemy));
00240|         m_attacks.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
00241|     }
00242| 
00243|     template <typename T> void update_all(const float dt, std::vector<T> &vec) {
00244|         for (T &t : vec) {
00245|             t.update(dt);
00246|         }
00247|     }
00248| 
00249|     void update(const float dt) override {
00250|         switch (m_game_state.state) {
00251|         case GameState::Playing: {
00252|             update_all(dt, m_caravan);
00253|             update_all(dt, m_caravan_slots);
00254|             update_all(dt, m_attacks);
00255|             update_all(dt, m_enemies);
00256|             update_all(dt, m_guards);
00257| 
00258|             for (int i = m_attacks.size() - 1; i >= 0; i--) {
00259|                 if (!m_attacks[i].is_visible()) {
00260|                     m_attacks[i].clear_render_data();
00261|                     m_attacks.erase(m_attacks.begin() + i);
00262|                 }
00263|             }
00264| 
00265|             const size_t time_elapsed_ms = m_game_state.time_elapsed_ms;
00266|             if (time_elapsed_ms > entity::enemy_t::spawn_rate_ms) {
00267|                 spawn_group_of_enemies();
00268|                 m_game_state.time_elapsed_ms = 0;
00269|             }
00270| 
00271|             for (size_t i = 0; i < m_enemies.size(); i++) {
00272|                 entity::Entity &enemy = m_enemies[i];
00273| 
00274|                 for (entity::Entity &guard : m_guards) {
00275|                     if (enemy.is_alive() && entity::can_attack(guard) &&
00276|                         entity::in_attack_range(guard, enemy)) {
00277|                         create_attack(guard, enemy);
00278|                         break;
00279|                     }
00280|                 }
00281| 
00282|                 // Find the id of the cart the enemy is inside.
00283|                 auto cart_id = find_caravan_cart(enemy.get_world_position());
00284|                 if (enemy.is_alive() && cart_id.has_value()) {
00285|                     m_caravan[cart_id.value()].damage(enemy.get_current_health());
00286|                     enemy.kill();
00287|                 }
00288| 
00289|                 if (enemy.is_dead()) {
00290|                     enemy.clear_render_data();
00291|                     m_enemies.erase(m_enemies.begin() + i);
00292|                 }
00293|             }
00294| 
00295|             for (auto &cart : m_caravan) {
00296|                 if (cart.is_dead()) {
00297|                     logger::info("Cart died, game over!");
00298|                     exit(0);
00299|                 }
00300|             }
00301| 
00302|             m_game_state.time_elapsed_ms += dt * 1000;
00303|             break;
00304|         }
00305| 
00306|         case GameState::Event: {
00307|             const interface::NDCPoint cursor_ndc_point =
00308|                 m_camera.to_ndc_point(m_mouse_state.cursor_viewport_position);
00309|             m_game_state.event->on_hover(cursor_ndc_point);
00310|             break;
00311|         }
00312| 
00313|         case GameState::Paused: {
00314|             break;
00315|         }
00316| 
00317|         default:
00318|             break;
00319|         };
00320| 
00321|         m_game_state.last_state = m_game_state.state;
00322|     };
00323| 
00324|     Event create_event(graphics_pipeline::geometry::GeometryRenderer *geom_renderer,
00325|                        graphics_pipeline::text::TextRenderer *text_renderer,
00326|                        const camera::Camera2D &camera) {
00327| 
00328|         Event event = Event(geom_renderer, text_renderer);
00329| 
00330|         event.create_dialog_bbox(camera);
00331| 
00332|         const interface::NDCPoint box_center = interface::NDCPoint(0, 0);
00333|         const math::Vector2 event_dialog_bbox_content_padding(0.03f, 0.04f);
00334|         const math::Vector2 event_dialog_bbox_size(1.80f, 1.20f);
00335|         const float font_size = camera.to_ndc_width(22);
00336|         const math::Vector2 line_size(event_dialog_bbox_size.x() -
00337|                                           event_dialog_bbox_content_padding.x() * 2,
00338|                                       font_size);
00339| 
00340|         const float content_top_y_ndc = box_center.y() -
00341|                                         event_dialog_bbox_size.y() / 2.0f +
00342|                                         event_dialog_bbox_content_padding.y() * 2.0f;
00343|         const float content_bottom_y_ndc = box_center.y() +
00344|                                            event_dialog_bbox_size.y() / 2.0f +
00345|                                            event_dialog_bbox_content_padding.y();
00346|         const float text_start_x_ndc = box_center.x() -
00347|                                        event_dialog_bbox_size.x() / 2.0f +
00348|                                        event_dialog_bbox_content_padding.x();
00349|         const float text_start_y_ndc = content_top_y_ndc + line_size.y();
00350|         const float text_start_y_options_ndc = content_bottom_y_ndc - line_size.y() * 3;
00351| 
00352|         auto event_desc_opts = graphics_pipeline::text::TextOpts{};
00353|         event_desc_opts.position =
00354|             interface::NDCPoint(text_start_x_ndc, text_start_y_ndc);
00355|         event_desc_opts.font_color = util::colors::WHITE;
00356|         event_desc_opts.font_size = font_size;
00357|         event_desc_opts.line_width = line_size.x();
00358|         event_desc_opts.line_height = line_size.y();
00359| 
00360|         graphics_pipeline::text::TextOpts event_dialog_option_opts{};
00361|         event_dialog_option_opts.position = interface::NDCPoint(
00362|             text_start_x_ndc, text_start_y_options_ndc - line_size.y());
00363|         event_dialog_option_opts.font_color = util::colors::WHITE;
00364|         event_dialog_option_opts.font_size = font_size;
00365|         event_dialog_option_opts.line_width = line_size.x();
00366|         event_dialog_option_opts.line_height = line_size.y();
00367| 
00368|         const auto quit_event = [this]() {
00369|             m_game_state.state = GameState::Playing;
00370|             m_game_state.event = std::nullopt;
00371|         };
00372| 
00373|         // Consistent, same every time for all events
00374|         DialogFactory dialog_factory = DialogFactory(text_renderer);
00375|         dialog_factory.set_event_dialog_text_opts(event_desc_opts);
00376|         dialog_factory.set_event_dialog_option_text_opts(event_dialog_option_opts);
00377| 
00378|         std::string node_a_id = "A";
00379|         dialog_factory.set_id(node_a_id);
00380|         dialog_factory.set_dialog_text(
00381|             "This is a really really really long event description that spans multiple "
00382|             "lines about an interesting event.");
00383|         dialog_factory.add_dialog_option("next0", "Next.", "B", []() {});
00384|         dialog_factory.add_dialog_option("exit0", "Exit the event.", std::nullopt,
00385|                                          quit_event);
00386|         event.add_dialog_node(dialog_factory.build());
00387| 
00388|         dialog_factory.set_id("B");
00389|         dialog_factory.set_dialog_text("...");
00390|         dialog_factory.add_dialog_option("exit1", "Exit again.", std::nullopt,
00391|                                          quit_event);
00392|         event.add_dialog_node(dialog_factory.build());
00393| 
00394|         event.set_active_node(node_a_id);
00395| 
00396|         return event;
00397|     }
00398| 
00399|     std::optional<size_t> find_caravan_cart(const camera::WorldPoint2D &point) {
00400|         for (size_t i = 0; i < m_caravan.size(); i++) {
00401|             if (m_caravan[i].is_point_inside(point)) {
00402|                 return i;
00403|             }
00404|         }
00405|         return std::nullopt;
00406|     }
00407| 
00408|     std::optional<size_t>
00409|     find_selected_caravan_slot(const camera::WorldPoint2D &click_point) {
00410|         for (size_t i = 0; i < m_caravan_slots.size(); i++) {
00411|             if (m_caravan_slots[i].is_point_inside(click_point)) {
00412|                 return i;
00413|             }
00414|         }
00415|         return std::nullopt;
00416|     }
00417| 
00418|     std::optional<size_t> find_selected_guard(const camera::WorldPoint2D &click_point) {
00419|         for (size_t i = 0; i < m_guards.size(); i++) {
00420|             if (m_guards[i].is_point_inside(click_point)) {
00421|                 return i;
00422|             }
00423|         }
00424|         return std::nullopt;
00425|     }
00426| 
00427|     void render() override {
00428| 
00429|         const camera::WorldPoint2D cursor_world_point =
00430|             m_camera.viewport_to_world(m_mouse_state.cursor_viewport_position);
00431| 
00432|         // Only highlight slots when hover if a guard is selected
00433|         if (m_game_state.selected_guard.has_value()) {
00434|             for (size_t i = 0; i < m_caravan_slots.size(); i++) {
00435|                 if (!m_caravan_slots[i].is_visible()) {
00436|                     continue;
00437|                 }
00438|                 m_caravan_slots[i].set_highlighted(
00439|                     m_caravan_slots[i].is_point_inside(cursor_world_point));
00440|             }
00441|         }
00442| 
00443|         const auto &selected_guard = m_game_state.selected_guard;
00444|         // Highlight the guard the cursor is hovering over
00445|         for (size_t i = 0; i < m_guards.size(); i++) {
00446|             entity::Entity &guard = m_guards[i];
00447|             // If a guard is selected, we keep it highlighted
00448|             if (selected_guard.has_value() && selected_guard.value() == i) {
00449|                 continue;
00450|             }
00451|             guard.set_highlighted(guard.is_point_inside(cursor_world_point));
00452|         }
00453| 
00454|         m_quad_renderer->sync_render_slots();
00455|         m_geom_renderer->sync_render_slots();
00456|         m_ui_geom_renderer->sync_render_slots();
00457|         m_ui_text_renderer->sync_render_slots();
00458| 
00459|         math::Matrix push_constant = m_camera.get_view_projection_matrix();
00460|         const size_t num_instances = 256; // Size of instance buffers
00461| 
00462|         auto command_buffer = m_command_buffer_manager->get_command_buffer();
00463| 
00464|         vulkan::Frame frame = m_swap_chain->begin_frame(command_buffer);
00465| 
00466|         frame.begin_render_pass(&m_render_pass);
00467|         m_quad_renderer->render(command_buffer, &push_constant, num_instances);
00468|         m_geom_renderer->render(command_buffer, &push_constant, num_instances);
00469|         frame.end_render_pass();
00470| 
00471|         const math::Matrix ui_push_constant = math::Matrix();
00472|         frame.begin_render_pass(&m_ui_render_pass);
00473| 
00474|         if (m_game_state.state == GameState::Event && m_game_state.event.has_value()) {
00475|             m_ui_geom_renderer->render(command_buffer, &ui_push_constant, 256);
00476|             m_game_state.event->render_text(command_buffer, &ui_push_constant);
00477|         }
00478|         frame.end_render_pass();
00479| 
00480|         frame.submit_present();
00481| 
00482|         m_ui_text_renderer->rotate_descriptors();
00483|     }
00484| 
00485|     void register_mouse_event_handler(vulkan::context::GraphicsContext *ctx) {
00486|         ctx->window->register_mouse_event_callback([this](window::MouseEvent mouse_event,
00487|                                                           interface::ViewportPoint &point)
00488|                                                        -> void {
00489|             switch (mouse_event) {
00490|             case window::MouseEvent::RIGHT_BUTTON_DOWN:
00491|                 m_mouse_state.is_right_button_pressed = true;
00492|                 break;
00493|             case window::MouseEvent::RIGHT_BUTTON_UP: {
00494|                 m_mouse_state.is_right_button_pressed = false;
00495|                 break;
00496|             }
00497|             case window::MouseEvent::CURSOR_MOVED:
00498|                 if (m_game_state.state == GameState::Playing &&
00499|                     m_mouse_state.is_right_button_pressed) {
00500|                     camera::WorldPoint2D world_delta = m_camera.viewport_delta_to_world(
00501|                         point - m_mouse_state.cursor_viewport_position);
00502|                     m_camera.set_relative_position(world_delta * INVERT_AXISES);
00503|                 }
00504|                 m_mouse_state.cursor_viewport_position = point;
00505|                 break;
00506|             case window::MouseEvent::SCROLL:
00507|                 if (m_game_state.state == GameState::Playing) {
00508|                     m_camera.set_relative_zoom(point.y() * ZOOM_SCALE_FACTOR);
00509|                 }
00510|                 break;
00511|             case window::MouseEvent::LEFT_BUTTON_DOWN:
00512|                 break;
00513|             case window::MouseEvent::LEFT_BUTTON_UP: {
00514|                 if (m_game_state.state == GameState::Playing) {
00515|                     const camera::WorldPoint2D world_point =
00516|                         m_camera.viewport_to_world(point);
00517|                     const auto &current_selected_guard = m_game_state.selected_guard;
00518| 
00519|                     // Clear currently selected guard
00520|                     if (current_selected_guard.has_value()) {
00521|                         m_guards[current_selected_guard.value()].set_selected(false);
00522|                         m_guards[current_selected_guard.value()].set_highlighted(false);
00523|                     }
00524| 
00525|                     const auto new_selected_guard = find_selected_guard(world_point);
00526|                     const auto clicked_caravan_slot =
00527|                         find_selected_caravan_slot(world_point);
00528| 
00529|                     if (current_selected_guard.has_value() &&
00530|                         clicked_caravan_slot.has_value()) {
00531|                         entity::Entity &slot =
00532|                             m_caravan_slots[clicked_caravan_slot.value()];
00533|                         entity::Entity &guard = m_guards[current_selected_guard.value()];
00534| 
00535|                         if (entity::is_free(slot)) {
00536|                             entity::Entity *old_slot = entity::get_caravan_slot(guard);
00537|                             entity::set_caravan_slot(guard, &slot);
00538|                             entity::set_occupying_guard(slot, &guard);
00539| 
00540|                             entity::clear_occupying_guard(*old_slot);
00541|                             old_slot->set_highlighted(false);
00542|                         }
00543|                     }
00544| 
00545|                     if (new_selected_guard.has_value()) {
00546|                         m_guards[new_selected_guard.value()].set_selected(true);
00547|                         m_guards[new_selected_guard.value()].set_highlighted(true);
00548|                     }
00549| 
00550|                     m_game_state.selected_guard = new_selected_guard;
00551| 
00552|                 } else if (m_game_state.state == GameState::Event &&
00553|                            m_game_state.event.has_value()) {
00554|                     const interface::NDCPoint cursor_ndc_point =
00555|                         m_camera.to_ndc_point(m_mouse_state.cursor_viewport_position);
00556|                     m_game_state.event->on_click(cursor_ndc_point);
00557|                 }
00558|                 break;
00559|             }
00560|             }
00561|         });
00562|     }
00563| 
00564|     void register_keyboard_event_handler(vulkan::context::GraphicsContext *ctx) {
00565|         ctx->window->register_keyboard_event_callback(
00566|             [this](window::KeyEvent &key, window::KeyState &state) -> void {
00567|                 if (state != window::KeyState::DOWN) {
00568|                     return;
00569|                 }
00570| 
00571|                 switch (key) {
00572|                 case window::KeyEvent::Y: {
00573|                     if (m_game_state.state == GameState::Playing) {
00574|                         m_game_state.event = create_event(
00575|                             m_ui_geom_renderer.get(), m_ui_text_renderer.get(), m_camera);
00576|                         m_game_state.state = GameState::Event;
00577|                     }
00578|                     break;
00579|                 }
00580| 
00581|                 default:
00582|                     break;
00583|                 };
00584|             });
00585|     }
00586| };
00587| 
00588| int main() {
00589| 
00590|     GameEngineConfig config{
00591|         .window_config = window::WindowConfig{.dims = window::WindowDimension(1080, 960),
00592|                                               .title = "Caravan Defence"},
00593|     };
00594| 
00595|     auto game = std::make_unique<CaravanDefence>();
00596|     auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
00597| 
00598|     game_engine->run();
00599| 
00600|     return 0;
00601| }
00602| 

(End of file - total 602 lines)
</file>
```

---

## Assistant (Plan · big-pickle · 36.6s)

_Thinking:_

Now I understand the current implementation. The user has a simple `enum class GameState` with three states: `Playing`, `Paused`, and `Event`. The state is managed through a simple variable `m_game_state.state`, and transitions happen directly (e.g., `m_game_state.state = GameState::Playing;`).

The current state management has these issues:
1. State logic is embedded in a big `switch` statement in the `update` method
2. State transitions are scattered throughout the code (in mouse/keyboard handlers)
3. No clear separation of concerns - state-specific logic is mixed with game logic
4. Adding new states requires modifying multiple places in the code
5. There's no validation that transitions are valid

The user wants a proper state machine where they can easily:
1. Add new states
2. Define transitions between states
3. Add state-specific logic

Let me think about how to design this state machine for them. I should propose a design that's:
1. Easy to add new states
2. Easy to define valid transitions
3. Easy to add state-specific logic (update, render, input handling)
4. Consistent with the existing C++ code style in the project

I'll propose a plan that uses:
1. A base `State` class/interface with virtual methods for `update`, `render`, `handle_input`, etc.
2. Concrete state classes for each game state
3. A `StateMachine` class that manages the current state and transitions
4. A transition table or similar mechanism to define valid transitions

Let me present this as a plan to the user.

I can see your current implementation uses a simple `enum class GameState` with three states (Playing, Paused, Event) managed through a `switch` statement in `update()` and scattered transitions in input handlers.

Here's my proposed plan for a proper state machine:

## Design Overview

**1. State Interface (abstract base class)**
- Virtual methods: `enter()`, `exit()`, `update(dt)`, `render()`, `handle_input()`, `handle_mouse()`, `handle_keyboard()`
- Each concrete state implements only what it needs

**2. Concrete State Classes**
- `PlayingState`: Current Playing logic (lines 251-303)
- `PausedState`: Empty or pause menu logic
- `EventState`: Current Event logic (lines 306-311)

**3. StateMachine Class**
- Holds current state + allows state change via `transition_to<StateType>()`
- Validates transitions against a transition table
- Calls `exit()` on old state, `enter()` on new state

**4. Transition Table**
- Simple `std::map<StateType, std::vector<StateType>>` defining valid next states
- Or use a more declarative approach with a DSL-like macro

## Key Benefits
- **Adding a new state**: Create new class + add one line to transition table
- **Adding transitions**: Add entry to transition table
- **State logic**: Isolated in respective state classes

## Questions for you:

1. **Where should this live?** In `sdk/game_engine_sdk/` as a reusable utility module, or specifically in the `examples/caravan_defence/` directory?

2. **Transition validation?** Should invalid transitions be compile-time errors, runtime asserts, or silent no-ops?

3. **Shared state access?** How should states access game data (entities, camera, etc.) - passed via constructor, or via a shared `GameContext` reference?

---

## User

1. It should live in @examples/caravan_defence/ 
2. Compile time errors!
3. Lets keep one common game state and not copy it between states.

---

## Assistant (Plan · big-pickle · 23.6s)

_Thinking:_

The user wants me to propose a state machine design that:
1. Lives in examples/caravan_defence/
2. Has compile-time validation for transitions
3. Uses a shared GameContext (not copied between states)

Let me think about how to achieve compile-time transition validation in C++:

1. **Template-based state machine**: Use CRTP or template parameters to validate transitions at compile time
2. **State enum with transition matrix as constexpr**: Use constexpr arrays/maps to define valid transitions
3. **Macro-based approach**: Use macros to declare transitions and generate compile-time checks
4. **Type-based state machine**: Use the type system to enforce valid transitions

For compile-time validation, I think the cleanest approach would be:
- Use a compile-time transition table using `std::array` or similar
- Each state has a `constexpr` list of valid next states
- Use static_assert to validate transitions

Actually, thinking more about this - the user wants compile-time errors for invalid transitions. This is tricky because you want:
1. States to declare what transitions they allow
2. The state machine to validate at compile time that a requested transition is valid

One elegant approach:
- Each state class has a `static constexpr` member listing valid transitions
- The `transition_to` function uses `static_assert` with a compile-time check

Let me design this:

```cpp
// State base with compile-time transition validation
template<typename Derived, typename... AllowedTransitions>
class State {
public:
    static constexpr auto valid_transitions = std::array<typename AllowedTransitions::StateType...>;
    
    // compile-time check if transition is valid
    static constexpr bool can_transition_to() { ... }
};

// Or using a more elegant pattern with type list
```

Actually, there's an even simpler approach that still gives compile-time errors:

```cpp
// In each state class, define valid transitions as a type list


// Use a template that only accepts valid transitions
template<typename TargetState, typename CurrentState>
struct TransitionValidator {
    static constexpr bool is_valid = /* check if TargetState is in CurrentState's valid transitions */;
};
```

Or perhaps the most practical: have each state declare `constexpr` valid transitions as a type list, then use a helper that asserts at compile time whether the target state is in that list.

Here's my proposed design for the state machine:

## Design

### 1. `GameStateMachine` Class
Located at `examples/caravan_defence/src/state_machine/GameStateMachine.h`

```cpp
template<typename StateEnum, typename Context>
class GameStateMachine {
    // Current state
    StateEnum m_current_state;
    Context& m_context;
    
    // Transition to new state (compile-time validated)
    template<StateEnum NewState>
    void transition();
};
```

### 2. Compile-Time Transition Validation

Each state declares its valid transitions as a type list:

```cpp
struct PlayingState {
    using ValidTransitions = TypeList<PausedState, EventState>;
    // ...
};

struct PausedState {
    using ValidTransitions = TypeList<PlayingState>;
    // ...
};

struct EventState {
    using ValidTransitions = TypeList<PlayingState>;
    // ...
};
```

Transition validation via `static_assert`:
```cpp
template<StateEnum NewState>
void transition() {
    static_assert(can_transition_v<NewState, CurrentState>, 
                  "Invalid state transition!");
    // ... perform transition
}
```

### 3. Shared GameContext
```cpp
struct GameContext {
    // References to all game data (entities, camera, etc.)
    // No ownership - just references
};
```

### 4. State Base Class
```cpp
template<typename Derived, typename Context>
class State {
public:
    virtual ~State() = default;
    virtual void enter(Context&) {}
    virtual void update(float dt, Context&) {}
    virtual void render(Context&) {}
    // ... input handlers
};
```

### File Structure
```
examples/caravan_defence/src/state_machine/
  - GameStateMachine.h    (state machine implementation)
  - State.h              (base state class)
  - PlayingState.h
  - PausedState.h
  - EventState.h
```

---

**One clarification**: For compile-time validation, I need to know how you'll handle the initial state - should it be a template parameter to `GameStateMachine`?

---

## User

Sure, it can be a template parameter.

I am not a fan of the virtual pointer as it creates a vtable which is diffuctult for the compiler to optimise. What other approaches are there?

---

## Assistant (Plan · big-pickle · 23.5s)

_Thinking:_

The user wants to avoid virtual functions (vtable) for performance reasons. Let me think about alternatives for implementing a state machine without virtual dispatch:

1. **Tag dispatch / Static dispatch** - Use templates to select behavior at compile time
2. **std::variant + std::visit** - Type-safe union with visitor pattern (no vtable)
3. **State pattern with function pointers** - Manual vtable implementation
4. **CRTP (Curiously Recurring Template Pattern)** - Compile-time polymorphism

Let me think about each:

### 1. Tag Dispatch
States are just tags/types, and behavior is selected via template specialization. This is very efficient but can get complex.

### 2. std::variant + std::visit
```cpp
using GameState = std::variant<PlayingState, PausedState, EventState>;
std::visit([](auto& state) { state.update(dt, context); }, current_state);
```
This uses std::visit which can be optimized by the compiler. No vtable needed. This is probably the cleanest modern C++ approach.

### 3. Function pointers
Manual vtable:
```cpp
struct StateVTable {
    void (*update)(void* state, float dt, Context& ctx);
    void (*render)(void* state, Context& ctx);
    // ...
};
```
But this is error-prone and hard to maintain.

### 4. CRTP
```cpp
template<typename Derived, typename Context>
class State {
public:
    void update(float dt, Context& ctx) {
        static_cast<Derived*>(this)->update(dt, ctx);
    }
};
```
This is compile-time polymorphism - no vtable. Each derived class gets its own copy of the functions.

I think the best options for the user are:
1. **std::variant + std::visit** - Clean, modern, no vtable, compile-time type safe


2. **CRTP** - Pure compile-time polymorphism, good if they want a more OOP-like interface without virtuals

I'll present both options to the user.

Good call on avoiding vtables. Here are the alternatives:

## Option 1: `std::variant` + `std::visit` (Recommended)

```cpp
using GameState = std::variant<PlayingState, PausedState, EventState>;

GameState current_state = PlayingState{context};

void update(float dt) {
    std::visit([dt](auto& state) {
        state.update(dt, context);
    }, current_state);
}
```

- **No vtable** - `std::visit` uses static dispatch when possible
- **Type-safe** - compiler catches invalid operations
- **Clean** - each state is just a struct with its data

## Option 2: CRTP (Curiously Recurring Template Pattern)

```cpp
template<typename Derived, typename Context>
class State {
public:
    void update(float dt, Context& ctx) {
        static_cast<Derived*>(this)->update(dt, ctx);
    }
};

class PlayingState : public State<PlayingState, GameContext> {
public:
    void update(float dt, GameContext& ctx);  // no override keyword needed
};
```

- **Pure compile-time polymorphism** - no vtable
- **OOP-like** - familiar interface
- **Slightly more complex** - requires understanding CRTP

## Option 3: Tag Dispatch

States are empty tags, all logic lives in free functions:
```cpp
struct Playing {};
struct Paused {};

void update(Playing,& ctx) { float dt, Context /* logic */ }
void update(Paused, float dt, Context& ctx) { /* logic */ }

std::variant<Playing, Paused> current_state;
std::visit([dt, &ctx](auto state) { update(state, dt, ctx); }, current_state);
```

---

**My recommendation**: `std::variant` (Option 1) - it's the most idiomatic modern C++20, has zero runtime overhead compared to virtual, and keeps state logic isolated in each struct.

Which approach do you prefer?

---

## User

Option 1 seems good enough

---

## Assistant (Plan · big-pickle · 5.9s)

_Thinking:_

The user has confirmed they want Option 1 (std::variant). Now I need to create a detailed plan for implementing this state machine in the caravan_defence example.

Let me think through what needs to be created:

1. **GameContext struct** - Holds references to all shared game data
2. **State types** - PlayingState, PausedState, EventState (as structs)
3. **GameStateMachine class** - Manages transitions using std::variant
4. **Update main.cpp** - Integrate the new state machine

Let me look at the current game state structure in main.cpp to understand what needs to go into GameContext:
- m_swap_chain
- m_command_buffer_manager
- m_camera
- m_mouse_state
- m_render_pass
- m_quad_renderer
- m_geom_renderer
- m_ui_render_pass
- m_ui_text_renderer
- m_ui_geom_renderer
- m_caravan
- m_caravan_slots
- m_guards
- m_attacks
- m_enemies
- m_game_state (last_state, state, event, time_elapsed_ms, selected_guard, rng)

I'll need to plan the file structure and the implementation details.

I'll create a comprehensive plan for implementing the state machine. Let me first look at what data needs to be shared in the GameContext.

**Tool: glob**

**Input:**
```json
{
  "pattern": "examples/caravan_defence/src/**/*.h"
}
```

**Output:**
```
/Users/adamolsson/projects/game_engine_cpp/examples/caravan_defence/src/events/Event.h
/Users/adamolsson/projects/game_engine_cpp/examples/caravan_defence/src/events/DialogFactory.h
/Users/adamolsson/projects/game_engine_cpp/examples/caravan_defence/src/events/Dialog.h
/Users/adamolsson/projects/game_engine_cpp/examples/caravan_defence/src/Entity.h
```

---

## Assistant (Plan · big-pickle · 21.3s)

_Thinking:_

Let me now create a detailed plan for implementing this. I'll structure the files as follows:

1. `examples/caravan_defence/src/state_machine/GameContext.h` - Shared context
2. `examples/caravan_defence/src/state_machine/PlayingState.h` - Playing state
3. `examples/caravan_defence/src/state_machine/PausedState.h` - Paused state
4. `examples/caravan_defence/src/state_machine/EventState.h` - Event state
5. `examples/caravan_defence/src/state_machine/GameStateMachine.h` - State machine
6. `examples/caravan_defence/src/state_machine/StateTypes.h` - State enum and variant

Then update main.cpp to use this new system.

Let me write out the plan.

Now I'll create the implementation plan:

## Implementation Plan

### File Structure
```
examples/caravan_defence/src/state_machine/
  - StateTypes.h        # State enum + std::variant
  - GameContext.h       # Shared game data references
  - PlayingState.h      # Playing state struct
  - PausedState.h       # Paused state struct
  - EventState.h        # Event state struct
  - GameStateMachine.h  # State machine implementation
```

### Step-by-Step

**Step 1: `StateTypes.h`**
- Define `enum class State` with Playing, Paused, Event
- Define `using GameState = std::variant<PlayingState, PausedState, EventState>`

**Step 2: `GameContext.h`**
- Create struct holding **references** to all game data (not owned)
- Move current game state members (`time_elapsed_ms`, `selected_guard`, `rng`, etc.) here
- Include pointers/references to entities, camera, renderers

**Step 3: `PlayingState.h`**
- Struct with `update(dt, context)` method
- Contains current Playing logic (lines 251-303 from main.cpp)
- No vtable - pure compile-time dispatch

**Step 4: `PausedState.h`**
- Struct with `update(dt, context)` method (currently empty)

**Step 5: `EventState.h`**
- Struct with `update(dt, context)` method
- Contains current Event logic (lines 306-311)

**Step 6: `GameStateMachine.h`**
- Template class holding `std::variant<GameState>` and current state index
- `transition<NewState>()` with compile-time validation using `static_assert`
- `update(dt)`, `render()`, `handle_*()` methods using `std::visit`

**Step 7: Update `main.cpp`**
- Include new headers
- Replace `m_game_state.state` enum with `GameStateMachine`
- Move entity/renderer pointers to `GameContext`
- Hook up input handlers to state machine

### Compile-Time Validation
```cpp
template<State NewState>
void transition() {
    static_assert(is_valid_transition<CurrentState(), NewState>::value,
                  "Invalid state transition");
    // ... perform transition
}
```

Where each state declares its valid transitions via a type trait.

---

Want me to proceed with this implementation?

---

## User

Give me an example how I would instantiate a new state and add it to the state machine

---

## Assistant (Plan · big-pickle · 14.1s)

_Thinking:_

The user wants to see an example of how they'd instantiate a new state and add it to the state machine. I need to provide a clear code example showing the API.

Here's how adding a new state would look with this design:

## Adding a New State

**1. Declare the state struct (in `StateTypes.h`):**
```cpp
struct GameOverState {
    int final_score;
};
```

**2. Add to variant (in `StateTypes.h`):**
```cpp
using GameState = std::variant<PlayingState, PausedState, EventState, GameOverState>;
```

**3. Define valid transitions in a traits class:**
```cpp
template<typename From, typename To>
struct is_valid_transition : std::false_type {};

template<> struct is_valid_transition<PlayingState, PausedState> : std::true_type {};
template<> struct is_valid_transition<PlayingState, EventState> : std::true_type {};
template<> struct is_valid_transition<PlayingState, GameOverState> : std::true_type {};
template<> struct is_valid_transition<PausedState, PlayingState> : std::true_type {};
template<> struct is_valid_transition<EventState, PlayingState> : std::true_type {};
template<> struct is_valid_transition<GameOverState, PlayingState> : std::true_type {};
```

**4. Implement the state's logic (new file `GameOverState.h`):**
```cpp
struct GameOverState {
    void update(float dt, GameContext& ctx) {
        // Game over logic
    }
};
```

## Using the State Machine

```cpp
int main() {
    GameContext ctx{/* references to game data */};
    
    // Initial state is Playing
    GameStateMachine<State, GameContext, PlayingState> state_machine{ctx};
    
    // Transition to Event (compile-time validated)
    state_machine.transition<EventState>();
    
    // Or transition with data
    state_machine.transition<GameOverState>(42);  // final_score = 42
    
    // In game loop
    void update(float dt) {
        state_machine.update(dt);
    }
}
```

---

**The key benefit**: If you try `state_machine.transition<EventState>()` from `GameOverState`, the `static_assert` fires at compile time with a clear error message.

Does this match what you're looking for?

---

## User

Will this also be reusable for other state machines?

---

## Assistant (Plan · big-pickle · 29.7s)

_Thinking:_

The user is asking if this design can be reused for other state machines in the future. They want it to be generic/reusable. This is a great question - I should explain how to make it generic.

Yes, absolutely. Here's how to make it fully generic:

## Reusable Generic Design

```cpp
// Fully generic - works for ANY state machine
template<typename StateEnum, typename Context, StateEnum InitialState, typename... States>
class StateMachine {
    std::variant<States...> m_current_state;
    Context& m_context;
};
```

## Usage for CaravanDefence

```cpp
// Specific to this game
using CaravanStateMachine = StateMachine<
    CaravanState,           // enum: Playing, Paused, Event, GameOver
    CaravanGameContext,     // your game context
    CaravanState::Playing,  // initial state
    PlayingState,
    PausedState,
    EventState,
    GameOverState
>;
```

## Usage for Another Game

```cpp
// Completely different game
using PlatformerStateMachine = StateMachine<
    PlatformerState,        // enum: Idle, Running, Jumping, Dead
    PlatformerContext,      // different context type
    PlatformerState::Idle,  // different initial
    IdleState,
    RunningState,
    JumpingState,
    DeadState
>;
```

## What's Generic vs What's Specific

| Generic (reusable) | Specific (your game) |
|---|---|
| `StateMachine` class | `State` enum values |
| `transition<NewState>()` with static_assert | `CaravanGameContext` struct |
| `update(dt)`, `render()`, input handlers | `PlayingState`, `EventState` structs |

## File Organization

```
examples/caravan_defence/src/state_machine/
  - StateMachine.h         # Generic implementation (reusable)
  - CaravanStateTypes.h    # Caravan-specific: enum + variant
  - CaravanContext.h       # Caravan-specific: game data
  - CaravanStates.h       # PlayingState, EventState, etc.
```

The `StateMachine.h` becomes a reusable utility you can copy to other projects.

---

**One question**: Should I make the generic `StateMachine.h` part of the SDK (`sdk/game_engine_sdk/util/`) so it's available for all projects, or keep it in `examples/caravan_defence/` since that's where you're using it?

---

## User

Lets store it in @examples/caravan_defence/ for now

---

## Assistant (Plan · big-pickle · 16.1s)

_Thinking:_

The user wants to store the state machine in `examples/caravan_defence/` for now. I'm in plan mode so I can't implement anything yet - I need to finalize the plan.

Let me summarize what we've discussed and present a clear implementation plan for the user to approve before I start implementing.

Based on our conversation, here's the plan:

1. Create a generic `StateMachine.h` in `examples/caravan_defence/src/state_machine/`
2. Create Caravan-specific state types, context, and states
3. Update main.cpp to use the new state machine

I should present this as a clear plan that the user can approve or provide feedback on.

## Implementation Plan Summary

### Files to Create

```
examples/caravan_defence/src/state_machine/
  - StateMachine.h         # Generic reusable state machine
  - CaravanStateTypes.h   # State enum + std::variant
  - CaravanGameContext.h  # Shared game data (references)
  - CaravanStates.h      # PlayingState, PausedState, EventState
```

### Implementation Order

1. **`StateMachine.h`** - Generic template class with:
   - `std::variant<States...>` for current state
   - `transition<NewState>()` with `static_assert` validation
   - `update()`, `render()`, input handlers using `std::visit`

2. **`CaravanStateTypes.h`** - Game-specific:
   - `enum class CaravanState { Playing, Paused, Event }`
   - Transition validation via trait structs
   - `std::variant<PlayingState, PausedState, EventState>`

3. **`CaravanGameContext.h`** - Shared data:
   - References to entities, camera, renderers
   - Game state data (`time_elapsed_ms`, `selected_guard`, `rng`, `event`)

4. **`CaravanStates.h`** - State implementations:
   - Each state struct with `update(dt, context)` method
   - Input handlers as needed

5. **`main.cpp`** - Integration:
   - Replace `GameState` enum with `GameStateMachine`
   - Move members to `CaravanGameContext`
   - Hook up handlers to state machine

---

Ready to implement?

---

