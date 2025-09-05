#include "Game.h"
#include "GameEngine.h"
#include "logger.h"
#include "render_engine/CommandBufferManager.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/colors.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/graphics_pipeline/TextPipeline.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/ui/UI.h"
#include <random>

constexpr glm::ivec3 UP = glm::ivec3(0, 1, 0);
constexpr glm::ivec3 DOWN = glm::ivec3(0, -1, 0);
constexpr glm::ivec3 LEFT = glm::ivec3(-1, 0, 0);
constexpr glm::ivec3 RIGHT = glm::ivec3(1, 0, 0);

constexpr float TILE_SIDE_PX = 50.0f;
constexpr int NUM_TILES = 13; // let be an odd number

constexpr glm::vec3 GAME_FIELD_CENTER = glm::vec3(0.0f, 0.0f, 0.0f);

constexpr glm::vec4 UV_APPLE = glm::vec4(0.125f * 0.0f, 0.0f, 0.125f * 1.0f, 1.0);
constexpr glm::vec4 UV_SNAKE_HEAD = glm::vec4(0.125f * 1.0f, 0.0f, 0.125f * 2.0f, 1.0);
constexpr glm::vec4 UV_SNAKE_BODY = glm::vec4(0.125f * 2.0f, 0.0f, 0.125f * 3.0f, 1.0);
constexpr glm::vec4 UV_SNAKE_TAIL = glm::vec4(0.125f * 3.0f, 0.0f, 0.125f * 4.0f, 1.0);

constexpr float SPRITE_ROTATION_LEFT = 0.0f;
constexpr float SPRITE_ROTATION_UP = M_PI / 2.0f;
constexpr float SPRITE_ROTATION_RIGHT = 2.0f * M_PI / 2.0f;
constexpr float SPRITE_ROTATION_DOWN = 3.0f * M_PI / 2.0f;

constexpr float BORDER_THICKNESS = 10.0f;
constexpr graphics_pipeline::GeometryInstanceBufferObject FRAME = {
    .center = GAME_FIELD_CENTER,
    .dimension =
        glm::vec2(1.0f) * TILE_SIDE_PX * static_cast<float>(NUM_TILES) + BORDER_THICKNESS,
    .color = colors::TRANSPARENT,
    .border.thickness = BORDER_THICKNESS,
    .border.color = colors::WHITE,
};

constexpr glm::vec4 FRAME_INNER_BOUNDS =
    glm::vec4(GAME_FIELD_CENTER.x + FRAME.dimension.x / 2.0f - FRAME.border.thickness,
              GAME_FIELD_CENTER.y + FRAME.dimension.y / 2.0f - FRAME.border.thickness,
              GAME_FIELD_CENTER.x - FRAME.dimension.x / 2.0f + FRAME.border.thickness,
              GAME_FIELD_CENTER.y - FRAME.dimension.x / 2.0f + FRAME.border.thickness);

void set_hover_color(ui::Button &self) {
    self.properties.container.background_color = colors::WHITE;
    self.properties.font.color = colors::BLACK;
}

void unset_hover_color(ui::Button &self) {
    self.properties.container.background_color = colors::BLACK;
    self.properties.font.color = colors::WHITE;
}

enum class GameState { MainMenu, Started };

class Snake : public Game {
  private:
    // Rendering
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;
    vulkan::Sampler m_sampler;
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;
    std::unique_ptr<graphics_pipeline::TextPipeline> m_text_pipeline;
    std::unique_ptr<Texture> m_sprite_sheet;

    GameState m_game_state = GameState::MainMenu;
    ui::UI m_main_menu;

    // Game tick
    const float m_game_tick_duration_base_s = 1.0f;
    const float m_game_tick_decrease_s = 0.02f;
    float m_current_tick_duration_s = 0.0f;
    float m_game_tick_duration_s = 1.0f;
    uint m_tick_count = 0;

    // Game state
    std::random_device m_rd;
    std::mt19937 m_rnd_gen;
    std::vector<bool> m_tiles_occupied;
    std::vector<glm::ivec3> m_apple_positions;
    const uint m_apple_spawn_tick_rate = 8;

    // Snake params
    float m_pending_sprite_rotation = SPRITE_ROTATION_UP;
    float m_sprite_rotation = SPRITE_ROTATION_UP;
    glm::ivec3 m_pending_head_direction = UP;
    glm::ivec3 m_head_direction = UP;
    std::vector<glm::ivec3> m_body_directions = {};
    std::vector<glm::ivec3> m_body_positions = {};

    graphics_pipeline::GeometryInstanceBufferObject m_snake_head;
    graphics_pipeline::GeometryInstanceBufferObject m_snake_body;
    graphics_pipeline::GeometryInstanceBufferObject m_snake_tail;

  public:
    Snake() {
        m_rnd_gen = std::mt19937(m_rd());
        m_snake_head = graphics_pipeline::GeometryInstanceBufferObject{
            .dimension = glm::vec2(TILE_SIDE_PX), .uvwt = UV_SNAKE_HEAD};
        m_snake_body = graphics_pipeline::GeometryInstanceBufferObject{
            .dimension = glm::vec2(TILE_SIDE_PX), .uvwt = UV_SNAKE_BODY};
        m_snake_tail = graphics_pipeline::GeometryInstanceBufferObject{
            .dimension = glm::vec2(TILE_SIDE_PX), .uvwt = UV_SNAKE_TAIL};

        m_main_menu = ui::UI(
            ui::Menu()
                .add_button(
                    ui::Button("SNAKE",
                               ui::ElementProperties{
                                   .container.center = glm::vec3(0.0f, 250.0f, 0.0f),
                                   .container.dimension = Dimension(0.0f),
                                   .container.background_color = colors::TRANSPARENT,
                                   .font.color = colors::WHITE,
                                   .font.size = 118,
                               }))
                .add_button(
                    ui::Button("START",
                               ui::ElementProperties{
                                   .container.center = glm::vec3(0.0f, 60.0f, 0.0f),
                                   .container.dimension = Dimension(400.0f, 100.0f),
                                   .container.background_color = colors::BLACK,
                                   .container.border.radius = 10.0f,
                                   .container.border.thickness = 5.0f,
                                   .container.border.color = colors::WHITE,
                                   .font.color = colors::WHITE,
                                   .font.size = 96,
                               })
                        .set_on_enter(set_hover_color)
                        .set_on_leave(unset_hover_color)
                        .set_on_click([this](ui::Button &self) {
                            m_game_state = GameState::Started;
                            unset_hover_color(self);
                        }))
                .add_button(
                    ui::Button("QUIT",
                               ui::ElementProperties{
                                   .container.center =
                                       glm::vec3(0.0f, -60.0f, 0.0),
                                   .container.dimension = Dimension(400.0f, 100.0f),
                                   .container.background_color = colors::BLACK,
                                   .container.border.radius = 10.0f,
                                   .container.border.thickness = 5.0f,
                                   .container.border.color = colors::WHITE,
                                   .font.color = colors::WHITE,
                                   .font.size = 96,
                               })
                        .set_on_enter(set_hover_color)
                        .set_on_leave(unset_hover_color)
                        .set_on_click([this](ui::Button &self) { exit(0); })));
        game_reset();
    }

    ~Snake() {};

    void game_reset() {
        m_body_positions.clear();
        m_body_directions.clear();
        m_tiles_occupied.clear();
        m_apple_positions.clear();

        m_game_tick_duration_s = m_game_tick_duration_base_s;
        m_tick_count = 0;

        m_tiles_occupied.resize(NUM_TILES * NUM_TILES, false);

        const auto start_position = glm::ivec3(0.0);
        m_body_positions.push_back(start_position + DOWN * 0);
        m_tiles_occupied[coordinate_to_index(m_body_positions.back())] = true;
        m_body_positions.push_back(start_position + DOWN * 1);
        m_tiles_occupied[coordinate_to_index(m_body_positions.back())] = true;
        m_body_positions.push_back(start_position + DOWN * 2);
        m_tiles_occupied[coordinate_to_index(m_body_positions.back())] = true;

        m_body_directions.push_back(UP);
        m_body_directions.push_back(UP);
        m_body_directions.push_back(UP);

        m_pending_head_direction = UP;
        m_head_direction = UP;

        m_game_state = GameState::MainMenu;

        logger::info("Game reset");
    }

    void update(float dt) override {
        m_current_tick_duration_s += dt;
        if (m_current_tick_duration_s < m_game_tick_duration_s) {
            return;
        }
        m_current_tick_duration_s = 0.0f;

        if (m_game_state == GameState::MainMenu) {
            return;
        }

        // Check for illegal move
        if (m_pending_head_direction != -m_head_direction) {
            m_head_direction = m_pending_head_direction;
            m_sprite_rotation = m_pending_sprite_rotation;
        }

        // CONTINUE: Add score when eating apple
        // TODO: Fix the blurryness of the texture sampler (new sampler?)
        // TODO: Fix the lines around the texture, the snake parts should be seamless
        // TODO: Create a sprite for the corner of a snake when it turns
        // TODO: Wrap the position in a class
        // TODO: Add a nice main menu animation of snakes going accross in the background

        m_body_directions.pop_back();
        m_body_directions.insert(m_body_directions.begin(), m_head_direction);

        // Check for playing field bounds, only required for head
        const auto old_snake_head_pos = m_body_positions[0];
        m_body_positions[0] += m_body_directions[0];
        const auto new_snake_head_pos = m_body_positions[0];
        const bool is_out_of_bounds = !(new_snake_head_pos.x <= NUM_TILES / 2 &&
                                        new_snake_head_pos.x >= -NUM_TILES / 2) ||
                                      !(new_snake_head_pos.y <= NUM_TILES / 2 &&
                                        new_snake_head_pos.y >= -NUM_TILES / 2);
        if (is_out_of_bounds) {
            logger::info("Game over, sneak out of bounds");
            game_reset();
            return;
        }

        m_tiles_occupied[coordinate_to_index(old_snake_head_pos)] = false;
        m_tiles_occupied[coordinate_to_index(new_snake_head_pos)] = true;

        for (auto i = 1; i < m_body_directions.size(); i++) {
            auto old_tile = m_body_positions[i];
            m_body_positions[i] += m_body_directions[i];
            auto new_tile = m_body_positions[i];
            m_tiles_occupied[coordinate_to_index(old_tile)] = false;
            m_tiles_occupied[coordinate_to_index(new_tile)] = true;
        }

        // Check if the snake is on the same tile as an apple
        for (auto i = 0; i < m_apple_positions.size(); i++) {
            if (m_apple_positions[i] == new_snake_head_pos) {
                m_apple_positions.erase(m_apple_positions.begin() + i);

                // Increase length of snake
                auto tail = m_body_positions.back();
                auto new_body = tail - m_body_directions.back();
                m_body_positions.push_back(new_body);
                m_body_directions.push_back(
                    UP); // Gets popped next iteration so values does not matter
                m_tiles_occupied[coordinate_to_index(new_body)] = true;

                // Speed up the game
                m_game_tick_duration_s -= m_game_tick_decrease_s;

                m_tick_count = 0; // reset apple spawn timer
                break;
            }
        }

        // Check if the snake has eaten itself
        for (auto i = 1; i < m_body_positions.size(); i++) {
            if (new_snake_head_pos == m_body_positions[i]) {
                logger::info("Game over, sneak ate itself");
                game_reset();
            }
        }

        // Spawn apple every n tick or if there are no apples
        if (m_apple_positions.size() == 0 ||
            m_tick_count % m_apple_spawn_tick_rate == 0) {
            spawn_apple();
        }

        m_tick_count++;
    };

    uint coordinate_to_index(const glm::ivec3 &coord) {
        DEBUG_ASSERT(abs(coord.x) <= NUM_TILES / 2 && abs(coord.y) <= NUM_TILES / 2,
                     "Received a coordinate ("
                         << coord << ") that is outisde of the playing field");

        // Because playing field is origo centered, coords can be negative
        const glm::ivec3 offset_coord =
            coord + glm::ivec3(NUM_TILES / 2, NUM_TILES / 2, 0);

        DEBUG_ASSERT(offset_coord.x >= 0 && offset_coord.y >= 0,
                     "Expected the offset coordinate (" << coord << " -> " << offset_coord
                                                        << ") to be positive");

        return offset_coord.x + offset_coord.y * NUM_TILES;
    }

    glm::ivec3 index_to_coordinate(const uint index) {
        const auto half_num_tiles = NUM_TILES / 2;
        const int x = (index % NUM_TILES) - half_num_tiles;
        const int y = (index / NUM_TILES) - half_num_tiles;
        return glm::ivec3(x, y, 0);
    }

    void spawn_apple() {
        // Get free tiles
        std::vector<size_t> free_tile_indices = {};
        for (auto i = 0; i < m_tiles_occupied.size(); i++) {
            if (!m_tiles_occupied[i]) {
                free_tile_indices.push_back(i);
            }
        }

        // Randomly select one free tile
        std::uniform_int_distribution<> dist(0, free_tile_indices.size());
        const size_t selected_tile_index = dist(m_rnd_gen);
        const size_t free_tile_index = free_tile_indices[selected_tile_index];
        const auto spawn_location = index_to_coordinate(free_tile_index);

        DEBUG_ASSERT(abs(spawn_location.x) <= NUM_TILES / 2 &&
                         abs(spawn_location.y) <= NUM_TILES / 2,
                     "Attempted to spawn an apple outisde of the playing field at "
                         << spawn_location);

        m_apple_positions.push_back(spawn_location);
        m_tiles_occupied[free_tile_index] = true;
    }

    void render() override {
        auto &instance_buffer = m_geometry_pipeline->get_rectangle_instance_buffer();
        auto &glyph_instance_buffer = m_text_pipeline->get_character_buffer();
        auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();

        instance_buffer.clear();
        glyph_instance_buffer.clear();
        text_segment_buffer.clear();

        if (m_game_state == GameState::MainMenu) {
            auto ui_state = m_main_menu.get_state();
            for (const auto button : ui_state.buttons) {
                instance_buffer.push_back(graphics_pipeline::GeometryInstanceBufferObject{
                    .center = button->properties.container.center,
                    .dimension = button->properties.container.dimension,
                    .color = button->properties.container.background_color,
                    .border.color = button->properties.container.border.color,
                    .border.thickness = button->properties.container.border.thickness,
                    .border.radius = button->properties.container.border.radius,
                });

                m_text_pipeline->text_kerning(button->text, button->properties);
            }
        } else {
            // render game
            for (auto i = 0; i < m_apple_positions.size(); i++) {
                instance_buffer.push_back(graphics_pipeline::GeometryInstanceBufferObject{
                    .center = static_cast<glm::vec3>(m_apple_positions[i]) * TILE_SIDE_PX,
                    .dimension = glm::vec2(TILE_SIDE_PX),
                    .color = colors::GREEN,
                    .uvwt = UV_APPLE,
                });
            }

            m_snake_head.center =
                static_cast<glm::vec3>(m_body_positions[0]) * TILE_SIDE_PX;
            m_snake_head.rotation = m_sprite_rotation;
            instance_buffer.push_back(m_snake_head);

            for (auto i = 1; i < m_body_positions.size() - 1; i++) {
                m_snake_body.center =
                    static_cast<glm::vec3>(m_body_positions[i]) * TILE_SIDE_PX;
                m_snake_body.rotation = m_sprite_rotation;
                instance_buffer.push_back(m_snake_body);
            }
            m_snake_tail.center =
                static_cast<glm::vec3>(m_body_positions.back()) * TILE_SIDE_PX;
            m_snake_tail.rotation = m_sprite_rotation;
            instance_buffer.push_back(m_snake_tail);

            instance_buffer.push_back(FRAME);
        }

        instance_buffer.transfer();
        glyph_instance_buffer.transfer();
        text_segment_buffer.transfer();

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        auto render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        m_geometry_pipeline->render_rectangles(command_buffer);
        m_text_pipeline->render_text(command_buffer);

        render_pass.end_submit_present();
    };

    void handle_keyboard_input(window::KeyEvent &event, window::KeyState &key_state) {
        if (key_state != window::KeyState::DOWN) {
            return;
        }
        switch (event) {
        case window::KeyEvent::UP:
        case window::KeyEvent::K:
        case window::KeyEvent::W:
            m_pending_head_direction = UP;
            m_pending_sprite_rotation = SPRITE_ROTATION_UP;
            break;
        case window::KeyEvent::LEFT:
        case window::KeyEvent::H:
        case window::KeyEvent::A:
            m_pending_head_direction = LEFT;
            m_pending_sprite_rotation = SPRITE_ROTATION_LEFT;
            break;
        case window::KeyEvent::DOWN:
        case window::KeyEvent::J:
        case window::KeyEvent::S:
            m_pending_head_direction = DOWN;
            m_pending_sprite_rotation = SPRITE_ROTATION_DOWN;
            break;
        case window::KeyEvent::RIGHT:
        case window::KeyEvent::L:
        case window::KeyEvent::D:
            m_pending_head_direction = RIGHT;
            m_pending_sprite_rotation = SPRITE_ROTATION_RIGHT;
            break;
        default:
            break;
        };
    }

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {
        // TODO: Register shaders based on which pipelines are used
        register_all_shaders();
        // TODO: Let the user register the fonts
        register_all_fonts();

        m_swap_chain_manager = std::make_unique<SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);

        m_sampler = vulkan::Sampler(ctx);
        m_sprite_sheet =
            Texture::unique_from_filepath(ctx, m_command_buffer_manager.get(),
                                          "examples/5_snake/assets/Sprite-0001.png");
        m_geometry_pipeline = std::make_unique<graphics_pipeline::GeometryPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager,
            graphics_pipeline::GeometryPipelineOptions{
                .combined_image_sampler =
                    vulkan::DescriptorImageInfo(m_sprite_sheet->view(), &m_sampler)});

        auto font = std::make_unique<Font>(ctx, m_command_buffer_manager.get(),
                                           "DefaultFont", &m_sampler);
        m_text_pipeline = std::make_unique<graphics_pipeline::TextPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager, std::move(font));

        ctx->window->register_keyboard_event_callback(
            [this](window::KeyEvent &event, window::KeyState &key_state) {
                handle_keyboard_input(event, key_state);
            });

        ctx->window->register_mouse_event_callback(
            [this](window::MouseEvent e, window::ViewportPoint &p) {
                if (m_game_state != GameState::MainMenu) {
                    return;
                }
                this->m_main_menu.update_state_from_mouse_event(e, p);
            });
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "Snake"}};

    auto game = std::make_unique<Snake>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
