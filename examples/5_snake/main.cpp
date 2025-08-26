#include "Game.h"
#include "GameEngine.h"
#include "io.h"
#include "render_engine/CommandBufferManager.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/resources/ResourceManager.h"

constexpr glm::vec2 UP = glm::vec2(0.0f, -1.0f);
constexpr glm::vec2 DOWN = glm::vec2(0.0f, 1.0f);
constexpr glm::vec2 LEFT = glm::vec2(-1.0f, 0.0f);
constexpr glm::vec2 RIGHT = glm::vec2(1.0f, 0.0f);

class Snake : public Game {
  private:
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    float m_current_tick_duration_s = 0.0f;
    const float m_game_tick_duration_s = 1.0f;

    // Snake params
    glm::vec2 m_direction = glm::vec2(0.0f);
    glm::vec2 m_position = glm::vec2(0.0f);

  public:
    Snake() {};
    ~Snake() {};

    void update(float dt) override {
        m_current_tick_duration_s += dt;
        if (m_current_tick_duration_s < m_game_tick_duration_s) {
            return;
        }
        m_current_tick_duration_s = 0.0f;
        std::cout << m_direction << std::endl;
    };

    void render() override {};

    void handle_keyboard_input(window::KeyEvent &event, window::KeyState &key_state) {
        if (key_state != window::KeyState::DOWN) {
            return;
        }
        switch (event) {
        case window::KeyEvent::W:
            m_direction = UP;
            break;
        case window::KeyEvent::A:
            m_direction = LEFT;
            break;
        case window::KeyEvent::S:
            m_direction = DOWN;
            break;
        case window::KeyEvent::D:
            m_direction = RIGHT;
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
