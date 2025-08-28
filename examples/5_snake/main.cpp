#include "Game.h"
#include "GameEngine.h"
#include "logger.h"
#include "render_engine/CommandBufferManager.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/colors.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/resources/ResourceManager.h"

constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 DOWN = glm::vec3(0.0f, -1.0f, 0.0f);
constexpr glm::vec3 LEFT = glm::vec3(-1.0f, 0.0f, 0.0f);
constexpr glm::vec3 RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);

constexpr float TILE_SIDE = 50.0f;

constexpr glm::vec3 GAME_FIELD_CENTER = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr graphics_pipeline::GeometryInstanceBufferObject FRAME = {
    .center = GAME_FIELD_CENTER,
    .dimension = glm::vec2(1.0f) * TILE_SIDE * 12.0f - TILE_SIDE / 2.0f,
    .color = colors::TRANSPARENT,
    .border.thickness = 10.0f,
    .border.color = colors::WHITE,
};

constexpr glm::vec4 FRAME_INNER_BOUNDS =
    glm::vec4(GAME_FIELD_CENTER.x + FRAME.dimension.x / 2.0f - FRAME.border.thickness,
              GAME_FIELD_CENTER.y + FRAME.dimension.y / 2.0f - FRAME.border.thickness,
              GAME_FIELD_CENTER.x - FRAME.dimension.x / 2.0f + FRAME.border.thickness,
              GAME_FIELD_CENTER.y - FRAME.dimension.x / 2.0f + FRAME.border.thickness);

class Snake : public Game {
  private:
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    float m_current_tick_duration_s = 0.0f;
    const float m_game_tick_duration_s = 1.0f;

    // Snake params
    glm::vec3 m_pending_head_direction = UP;
    glm::vec3 m_head_direction = UP;
    std::vector<glm::vec3> m_body_directions = {};
    std::vector<glm::vec3> m_body_positions = {};

  public:
    Snake() { game_reset(); }

    ~Snake() {};

    void game_reset() {
        m_body_positions.clear();
        m_body_directions.clear();

        const auto start_position = GAME_FIELD_CENTER;
        m_body_positions.push_back(start_position + DOWN * TILE_SIDE * 0.0f);
        m_body_positions.push_back(start_position + DOWN * TILE_SIDE * 1.0f);
        m_body_positions.push_back(start_position + DOWN * TILE_SIDE * 2.0f);
        m_body_directions.push_back(UP);
        m_body_directions.push_back(UP);
        m_body_directions.push_back(UP);

        m_pending_head_direction = UP;
        m_head_direction = UP;
    }

    void update(float dt) override {
        m_current_tick_duration_s += dt;
        if (m_current_tick_duration_s < m_game_tick_duration_s) {
            return;
        }
        m_current_tick_duration_s = 0.0f;

        // Check for illegal move
        if (m_pending_head_direction != -m_head_direction) {
            m_head_direction = m_pending_head_direction;
        }

        // CONTINUE: Add spawn apple mechanic and increase snake length once eaten
        // CONTINUE: Add game over check when snake eats itself

        m_body_directions.pop_back();
        m_body_directions.insert(m_body_directions.begin(), m_head_direction);

        for (auto i = 0; i < m_body_directions.size(); i++) {
            m_body_positions[i] += m_body_directions[i] * TILE_SIDE;
        }

        // Check for playing field bounds
        const auto snake_head_pos = m_body_positions[0];
        if (!(snake_head_pos.x < FRAME_INNER_BOUNDS.x &&
              snake_head_pos.x > FRAME_INNER_BOUNDS.z) ||
            !(snake_head_pos.y < FRAME_INNER_BOUNDS.y &&
              snake_head_pos.y > FRAME_INNER_BOUNDS.w)) {
            game_reset();
        }
    };

    void render() override {
        auto &instance_buffer = m_geometry_pipeline->get_rectangle_instance_buffer();
        instance_buffer.clear();

        for (auto i = 0; i < m_body_positions.size(); i++) {
            instance_buffer.push_back(graphics_pipeline::GeometryInstanceBufferObject{
                .center = m_body_positions[i],
                .dimension = glm::vec2(TILE_SIDE),
                .color = i == 0 ? colors::RED : colors::WHITE,
                .uvwt = glm::vec4(-1.0f),
            });
        }

        instance_buffer.push_back(FRAME);

        instance_buffer.transfer();

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        auto render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        m_geometry_pipeline->render_rectangles(command_buffer);

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
            break;
        case window::KeyEvent::LEFT:
        case window::KeyEvent::H:
        case window::KeyEvent::A:
            m_pending_head_direction = LEFT;
            break;
        case window::KeyEvent::DOWN:
        case window::KeyEvent::J:
        case window::KeyEvent::S:
            m_pending_head_direction = DOWN;
            break;
        case window::KeyEvent::RIGHT:
        case window::KeyEvent::L:
        case window::KeyEvent::D:
            m_pending_head_direction = RIGHT;
            break;
        default:
            break;
        };
    }

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {
        register_all_shaders();
        m_swap_chain_manager = std::make_unique<SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);

        m_geometry_pipeline = std::make_unique<graphics_pipeline::GeometryPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager,
            graphics_pipeline::GeometryPipelineOptions{});

        ctx->window->register_keyboard_event_callback(
            [this](window::KeyEvent &event, window::KeyState &key_state) {
                handle_keyboard_input(event, key_state);
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
