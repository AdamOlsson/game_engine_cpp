#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "math/Matrix.h"
#include "tiling/Position.h"
#include "tiling/TileGrid.h"
#include "tiling/search/AStar.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/SwapChain.h"
#include "window/WindowConfig.h"
#include <memory>

enum SearchState {
    Start,
    Search,
    ShowSearch,
};

enum class CellType : uint8_t {};

constexpr size_t TILE_SIZE = 24; // World space
constexpr auto INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr auto ZOOM_SCALE_FACTOR = 0.1f;
constexpr auto WALL_WEIGHT = 100000.0f;

struct FrameState {
    std::vector<size_t> modified_instances;
};

class ExamplePathing : public Game {
  private:
    std::unique_ptr<vulkan::SwapChain> m_swap_chain;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    vulkan::DescriptorPool m_descriptor_pool;

    bool m_is_right_mouse_pressed = false;
    bool m_is_left_mouse_pressed = false;
    bool m_is_left_shift_pressed = false;
    window::ViewportPoint m_mouse_last_position = window::ViewportPoint();
    camera::Camera2D m_camera;

    size_t m_num_instances;
    std::vector<graphics_pipeline::geometry::GeometrySBOHandle> m_tile_data;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_renderer;

    size_t m_swap_index;
    size_t m_num_tiles_width;
    size_t m_num_tiles_height;

    std::array<FrameState, 2> m_frame_states;

    SearchState m_state;
    camera::WorldPoint2D m_start;
    camera::WorldPoint2D m_end;
    std::vector<tiling::Position> m_path;
    std::set<tiling::Position> m_walls;

    tiling::TileGrid<CellType> m_grid;

  public:
    ExamplePathing()
        : m_swap_index(0), m_mouse_last_position(window::ViewportPoint(-10000, -1000)),
          m_grid(tiling::TileGrid<CellType>(0, 0)) {}

    ~ExamplePathing() {};

    void update(float dt) override {
        if (m_state == SearchState::Search) {
            const auto start_tile_x = (m_start.x / TILE_SIZE) + 0.5f;
            const auto start_tile_y = (m_start.y / TILE_SIZE) + 0.5f;
            const tiling::Position start_tile =
                tiling::Position(start_tile_x, start_tile_y);

            const auto end_tile_x = (m_end.x / TILE_SIZE) + 0.5f;
            const auto end_tile_y = (m_end.y / TILE_SIZE) + 0.5f;
            const tiling::Position end_tile = tiling::Position(end_tile_x, end_tile_y);

            for (const auto &wall : m_walls) {
                if (wall == start_tile || wall == end_tile) {
                    continue;
                }
                auto &cell = m_grid.get_cell(wall.x, wall.y);
                cell.weight = WALL_WEIGHT;
            }

            /*logger::debug("Starting search from ", start_tile, " to ", end_tile);*/
            m_path = tiling::search::AStar::search(m_grid, start_tile, end_tile);
            m_state = SearchState::ShowSearch;
        }
    };

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        auto window_size = ctx->window->get_framebuffer_size<float>();
        const float num_pixels_at_default_zoom = 200.0f;
        m_camera = camera::Camera2D(window_size.width, window_size.height,
                                    num_pixels_at_default_zoom);
        m_camera.configure_max_zoom(5.0f);
        m_camera.configure_min_zoom(0.2f);
        m_camera.set_zoom(0.4f);
        register_mouse_event_handler(ctx.get());
        register_keyboard_event_handler(ctx.get());

        m_swap_chain = std::make_unique<vulkan::SwapChain>(ctx);
        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_descriptor_pool = vulkan::DescriptorPool(
            ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,
                                            .num_storage_buffers = 1,
                                            .num_uniform_buffers = 0,
                                            .num_combined_image_samplers = 0});

        const auto window_dims = ctx->window->dimensions<size_t>();
        // multiply by 2 is simply to cover the entire screen and more
        m_num_tiles_width = (window_dims.width / TILE_SIZE) * 2;
        m_num_tiles_height = (window_dims.width / TILE_SIZE) * 2;
        auto quad_push_constant_range =
            vulkan::PushConstantRange{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                      .offset = 0,
                                      .size = camera::Camera2D::matrix_size()};

        graphics_pipeline::RendererOpts renderer_opts{};
        renderer_opts.push_constant_range = quad_push_constant_range;
        renderer_opts.swap_chain.extent = m_swap_chain->get_extent();
        renderer_opts.swap_chain.render_pass = m_swap_chain->get_render_pass_handle();
        renderer_opts.geometry.instance_buffer_opts.size =
            m_num_tiles_width * m_num_tiles_height;
        m_renderer = std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
            ctx, m_command_buffer_manager.get(), renderer_opts);

        m_tile_data.reserve(m_num_tiles_width * m_num_tiles_height);
        for (auto i = 0; i < m_num_tiles_width * m_num_tiles_height; i++) {
            graphics_pipeline::geometry::GeometrySBOHandle handle =
                m_renderer->request_render_slot();
            graphics_pipeline::geometry::GeometryPipelineSBO &instance =
                m_renderer->get_instance(handle);
            m_tile_data.push_back(std::move(handle));

            const int x = i % m_num_tiles_width;
            const int y = i / m_num_tiles_height;
            instance.model_matrix =
                math::Matrix().scale(TILE_SIZE, TILE_SIZE, 1.0f).translate(x, y, 0);
            instance.border = {
                .color = util::colors::rgba(1.0f, 1.0f, 1.0f, 0.1f),
                .width = 1,
            };
            m_num_instances++;
        }

        m_renderer->sync_render_slots();

        m_grid = tiling::TileGrid<CellType>(m_num_tiles_width, m_num_tiles_height);
        m_camera.set_position(camera::WorldPoint2D(
            m_num_tiles_width / 2.0f * TILE_SIZE, m_num_tiles_height / 2.0f * TILE_SIZE));
        m_start = m_camera.get_position();
    }

    void register_mouse_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_mouse_event_callback(
            [this](window::MouseEvent mouse_event, window::ViewportPoint &point) -> void {
                switch (mouse_event) {
                case window::MouseEvent::RIGHT_BUTTON_DOWN:
                    m_is_right_mouse_pressed = true;
                    break;
                case window::MouseEvent::RIGHT_BUTTON_UP:
                    m_is_right_mouse_pressed = false;
                    break;
                case window::MouseEvent::CURSOR_MOVED:
                    if (m_is_left_shift_pressed && m_is_left_mouse_pressed) {
                        // Recaalculate the cursor position to a tile
                        const auto world_point = m_camera.viewport_to_world(point);
                        const auto tile_x = (world_point.x / TILE_SIZE) + 0.5f;
                        const auto tile_y = (world_point.y / TILE_SIZE) + 0.5f;
                        m_walls.insert(tiling::Position(tile_x, tile_y));
                    } else if (!m_is_left_shift_pressed && m_is_right_mouse_pressed) {
                        auto world_delta = m_camera.viewport_delta_to_world(
                            point - m_mouse_last_position);
                        m_camera.set_relative_position(world_delta * INVERT_AXISES);
                    }
                    m_mouse_last_position = point;
                    break;
                case window::MouseEvent::SCROLL:
                    m_camera.set_relative_zoom(point.y * ZOOM_SCALE_FACTOR);
                    break;
                case window::MouseEvent::LEFT_BUTTON_DOWN:
                    m_is_left_mouse_pressed = true;
                    break;
                case window::MouseEvent::LEFT_BUTTON_UP: {
                    if (m_is_left_shift_pressed) {
                        break;
                    }

                    if (m_state == SearchState::Start) {
                        m_end = m_camera.viewport_to_world(point);
                        m_state = SearchState::Search;
                    } else if (m_state == ShowSearch) {
                        m_start = m_end;
                        m_path.clear();
                        m_state = SearchState::Start;
                    }
                    m_is_left_mouse_pressed = false;
                    break;
                }
                }
            });
    }

    void register_keyboard_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_keyboard_event_callback(
            [this](window::KeyEvent &key, window::KeyState &state) {
                switch (key) {
                case window::KeyEvent::LEFT_SHIFT:
                    m_is_left_shift_pressed = state == window::KeyState::DOWN;
                    break;
                default:
                    break;
                }
            });
    }

    void render() override {

        // Reset highlight from previous frame
        const auto modified_instances = m_frame_states[m_swap_index].modified_instances;
        for (const auto &idx : modified_instances) {
            auto &instance = m_renderer->get_instance(m_tile_data[idx]);
            instance.color = util::colors::TRANSPARENT;
            /*m_tile_instances->at(idx).color = util::colors::TRANSPARENT;*/
        }

        m_frame_states[m_swap_index].modified_instances.clear();

        // Render walls
        for (const auto &wall : m_walls) {
            const auto wall_tile_index = static_cast<size_t>(wall.y) * m_num_tiles_width +
                                         static_cast<size_t>(wall.x);
            auto &instance = m_renderer->get_instance(m_tile_data[wall_tile_index]);
            instance.color = util::colors::rgba(0.8f, 0.8f, 0.8f, 1.0f);
            /*m_tile_instances->at(wall_tile_index).color =*/
            /*    util::colors::rgba(0.8f, 0.8f, 0.8f, 1.0f);*/
            m_frame_states[m_swap_index].modified_instances.push_back(wall_tile_index);
        }

        // Render path
        for (const auto &pos : m_path) {
            const auto tile_index = pos.y * m_num_tiles_width + pos.x;
            if (0 <= tile_index && tile_index < m_num_tiles_width * m_num_tiles_height) {
                auto &instance = m_renderer->get_instance(m_tile_data[tile_index]);
                instance.color = util::colors::rgba(0.0f, 1.0f, 0.0f, 0.4f);
                /*m_tile_instances->at(tile_index).color =*/
                /*    util::colors::rgba(0.0f, 1.0f, 0.0f, 0.4f);*/
                m_frame_states[m_swap_index].modified_instances.push_back(tile_index);
            }
        }

        // Render start tile
        const auto start_tile_x = (m_start.x / TILE_SIZE) + 0.5f;
        const auto start_tile_y = (m_start.y / TILE_SIZE) + 0.5f;
        const auto start_tile_index =
            static_cast<size_t>(start_tile_y) * m_num_tiles_width +
            static_cast<size_t>(start_tile_x);
        if (0 <= start_tile_index &&
            start_tile_index < m_num_tiles_width * m_num_tiles_height) {
            auto &instance = m_renderer->get_instance(m_tile_data[start_tile_index]);
            instance.color = util::colors::rgba(0.0f, 0.0f, 1.0f, 0.4f);
            /*m_tile_instances->at(start_tile_index).color =*/
            /*    util::colors::rgba(0.0f, 0.0f, 1.0f, 0.4f);*/
            m_frame_states[m_swap_index].modified_instances.push_back(start_tile_index);
        }

        // Render stop tile
        if (m_state == SearchState::ShowSearch) {
            const auto end_tile_x = (m_end.x / TILE_SIZE) + 0.5f;
            const auto end_tile_y = (m_end.y / TILE_SIZE) + 0.5f;
            const auto end_tile_index =
                static_cast<size_t>(end_tile_y) * m_num_tiles_width +
                static_cast<size_t>(end_tile_x);
            if (0 <= end_tile_index &&
                end_tile_index < m_num_tiles_width * m_num_tiles_height) {
                auto &instance = m_renderer->get_instance(m_tile_data[end_tile_index]);
                instance.color = util::colors::rgba(1.0f, 0.0f, 0.0f, 0.4f);
                /*m_tile_instances->at(end_tile_index).color =*/
                /*    util::colors::rgba(1.0f, 0.0f, 0.0f, 0.4f);*/
                m_frame_states[m_swap_index].modified_instances.push_back(end_tile_index);
            }
        }

        const auto cursor_world_point = m_camera.viewport_to_world(m_mouse_last_position);
        // Render tile highlight under cursor
        // Offset by half a tile because tile center is middle of tile, not bottom right
        const auto cursor_tile_x = (cursor_world_point.x / TILE_SIZE) + 0.5f;
        const auto cursor_tile_y = (cursor_world_point.y / TILE_SIZE) + 0.5f;
        const auto cursor_tile_index =
            static_cast<size_t>(cursor_tile_y) * m_num_tiles_width +
            static_cast<size_t>(cursor_tile_x);
        if (0 <= cursor_tile_index &&
            cursor_tile_index < m_num_tiles_width * m_num_tiles_height) {
            auto &current_color =
                m_renderer->get_instance(m_tile_data[cursor_tile_index]).color;
            /*auto &current_color = m_tile_instances->at(cursor_tile_index).color;*/
            if (current_color == util::colors::TRANSPARENT) {
                current_color = util::colors::rgba(0.2f, 0.2f, 0.2f, 1.0f);
            } else {
                current_color.a = 0.2f;
            }
            m_frame_states[m_swap_index].modified_instances.push_back(cursor_tile_index);
        }

        m_renderer->sync_render_slots();

        glm::mat4 push_constant = m_camera.get_view_projection_matrix();

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        vulkan::Frame frame = m_swap_chain->begin_frame(command_buffer);

        frame.begin_render_pass();
        m_renderer->render(command_buffer, &push_constant, m_num_instances);
        frame.end_render_pass();

        frame.submit_present();

        m_swap_index = 0;
        /*++m_swap_index % 2;*/
    }
};

int main() {

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                        🎯 CONTROLS                         ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "  ① Left Click once to search from blue to clicked tile\n";
    std::cout << "\n";
    std::cout << "  ② Left Click again to remove\n";
    std::cout << "\n";
    std::cout << "  ③ Hold left Shift and click & drag to create obstacles over tiles\n";
    std::cout << "\n";

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "6_pathing"},
    };

    auto game = std::make_unique<ExamplePathing>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
