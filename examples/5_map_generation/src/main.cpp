#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/Camera.h"
#include "game_engine_sdk/render_engine/TilesetUVWT.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "game_engine_sdk/render_engine/window/WindowConfig.h"
#include <memory>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

enum class CellType : uint8_t {
    None,
    Wall,
    Grass,
};

std::ostream &operator<<(std::ostream &os, CellType type) {
    switch (type) {
    case CellType::None:
        return os << "CellType::None";
    case CellType::Wall:
        return os << "CellType::Wall";
    case CellType::Grass:
        return os << "CellType::Grass";
    default:
        return os << "Unknown";
    }
}

std::ostream &operator<<(std::ostream &os,
                         std::tuple<CellType, CellType, CellType, CellType> type) {
    return os << "(" << std::get<0>(type) << ", " << std::get<1>(type) << ", "
              << std::get<2>(type) << ", " << std::get<3>(type) << ")";
}

struct ConstraintHash {
    std::size_t
    operator()(const std::tuple<CellType, CellType, CellType, CellType> t) const {
        return (static_cast<size_t>(std::get<0>(t)) << 24) |
               (static_cast<size_t>(std::get<1>(t)) << 16) |
               (static_cast<size_t>(std::get<2>(t)) << 8) |
               (static_cast<size_t>(std::get<3>(t)));
    }
};

class MapGeneration : public Game {
  private:
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    vulkan::Sampler m_sampler;
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;

    std::vector<graphics_pipeline::GeometryInstanceBufferObject> m_render_cells;

    Texture m_tileset;
    TilesetUVWT m_tileset_uvwt;
    std::unordered_map<std::tuple<CellType, CellType, CellType, CellType>, glm::vec4,
                       ConstraintHash>
        m_tileset_constraints;

    const int noise_map_width = 4;
    const int noise_map_height = 3;
    const std::vector<float> noise_map = {0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
                                          1.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    bool m_is_right_mouse_pressed = false;
    window::ViewportPoint m_mouse_last_position = window::ViewportPoint();
    Camera2D m_camera;

    std::vector<glm::vec4> cell_sprites;

  public:
    MapGeneration() {}

    void wang_tiling() {
        auto rule = [](float value) -> CellType {
            if (value > 0.5) {
                return CellType::Wall;
            } else {
                return CellType::Grass;
            }
        };

        std::vector<CellType> cell_types;
        cell_types.reserve(noise_map.size());
        for (auto c : noise_map) {
            cell_types.push_back(rule(c));
        }

        const int map_grid_width = noise_map_width;
        const int map_grid_height = noise_map_height;
        cell_sprites.reserve(map_grid_width * map_grid_height);
        for (auto i = 0; i < cell_types.size(); i++) {

            int current = i;
            int left = current - 1;
            int right = current + 1;
            int top = current - map_grid_width;
            int bottom = current + map_grid_width;

            // For each cell, find the constraints based on bordering cells
            CellType left_constraint = CellType::None;
            if (left >= 0) {
                left_constraint = cell_types[left];
            }

            CellType top_constraint = CellType::None;
            if (top >= 0) {
                top_constraint = cell_types[top];
            }

            CellType right_constraint = CellType::None;
            if (right < map_grid_width * map_grid_height) {
                right_constraint = cell_types[right];
            }

            CellType bottom_constraint = CellType::None;
            if (bottom < map_grid_width * map_grid_height) {
                bottom_constraint = cell_types[bottom];
            }

            const std::tuple<CellType, CellType, CellType, CellType> constraints =
                std::tuple(top_constraint, right_constraint, bottom_constraint,
                           left_constraint);

            // TODO: A problem is that the outer most tiles will always have a "None"
            // constraint towards the edge of the map. Either I would need to have a this
            // "None" to a wildcard match or even easier, the outer most tiles always have
            // no texture.
            if (m_tileset_constraints.find(constraints) != m_tileset_constraints.end()) {
                cell_sprites.push_back(m_tileset_constraints[constraints]);

            } else {
                // TODO: Create some wrapper around uvwt coordinates have this be "no
                // uvwt"
                cell_sprites.push_back(glm::vec4(-1.0f));
            }
        }
    }

    ~MapGeneration() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {
        m_swap_chain_manager = std::make_unique<SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);

        m_sampler = vulkan::Sampler(ctx, vulkan::Filter::NEAREST,
                                    vulkan::SamplerAddressMode::CLAMP_TO_BORDER);
        m_tileset = Texture::from_filepath(ctx, m_command_buffer_manager.get(),
                                           ASSET_FILE("forest_tileset_24x24.png"));
        m_tileset_uvwt = TilesetUVWT(m_tileset, TileSize(24, 24));
        m_tileset_constraints = {
            // Walls
            {std::tuple(CellType::Grass, CellType::Grass, CellType::Wall,
                        CellType::Grass),
             m_tileset_uvwt.uvwt_for_tile_at(2, 0)},
            {std::tuple(CellType::Grass, CellType::Wall, CellType::Wall, CellType::Grass),
             m_tileset_uvwt.uvwt_for_tile_at(1, 1)},
            {std::tuple(CellType::Grass, CellType::Wall, CellType::Wall, CellType::Wall),
             m_tileset_uvwt.uvwt_for_tile_at(2, 1)},
            {std::tuple(CellType::Grass, CellType::Grass, CellType::Wall, CellType::Wall),
             m_tileset_uvwt.uvwt_for_tile_at(3, 1)},
            {std::tuple(CellType::Wall, CellType::Wall, CellType::Wall, CellType::Grass),
             m_tileset_uvwt.uvwt_for_tile_at(1, 2)},
            {std::tuple(CellType::Wall, CellType::Grass, CellType::Wall, CellType::Wall),
             m_tileset_uvwt.uvwt_for_tile_at(3, 2)},
            {std::tuple(CellType::Wall, CellType::Wall, CellType::Grass, CellType::Grass),
             m_tileset_uvwt.uvwt_for_tile_at(1, 3)},

            // Same uvwt for multiple scenarios
            {std::tuple(CellType::Wall, CellType::Wall, CellType::Grass, CellType::Wall),
             m_tileset_uvwt.uvwt_for_tile_at(2, 3)},
            {std::tuple(CellType::Grass, CellType::Wall, CellType::Grass,
                        CellType::Grass),
             m_tileset_uvwt.uvwt_for_tile_at(2, 3)},
            {std::tuple(CellType::Grass, CellType::Grass, CellType::Grass,
                        CellType::Wall),
             m_tileset_uvwt.uvwt_for_tile_at(2, 3)},

            {std::tuple(CellType::Wall, CellType::Grass, CellType::Grass, CellType::Wall),
             m_tileset_uvwt.uvwt_for_tile_at(3, 3)},

            // Grass
            /*{std::tuple(CellType::Wall, CellType::Grass, CellType::Wall,
               CellType::Wall),*/
            /* m_tileset_uvwt.uvwt_for_tile_at(0, 3)},*/

        };

        m_geometry_pipeline = std::make_unique<graphics_pipeline::GeometryPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager,
            graphics_pipeline::GeometryPipelineOptions{
                .combined_image_samplers = {
                    vulkan::DescriptorImageInfo(m_tileset.view(), &m_sampler),
                }});

        wang_tiling();

        auto window_size = ctx->window->get_framebuffer_size<float>();
        m_camera = Camera2D(window_size.width, window_size.height);
        register_mouse_event_handler(ctx.get());
    }

    void register_mouse_event_handler(graphics_context::GraphicsContext *ctx) {
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
                    if (m_is_right_mouse_pressed) {
                        logger::debug("Mouse position: ", point);
                        auto position_change = point - m_mouse_last_position;
                        m_camera.set_relative_position(position_change);
                    }
                    m_mouse_last_position = point;
                    break;
                case window::MouseEvent::LEFT_BUTTON_DOWN:
                case window::MouseEvent::LEFT_BUTTON_UP:
                    break;
                }
            });
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        RenderPass render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        auto &rectangle_instance_buffer =
            m_geometry_pipeline->get_rectangle_instance_buffer();

        rectangle_instance_buffer.clear();

        const size_t num_geometries = m_render_cells.size();
        for (auto i = 0; i < num_geometries; i++) {
            rectangle_instance_buffer.push_back(m_render_cells[i]);
        }

        const int cell_width = 50;
        const int cell_height = 50;
        const WorldPoint center_offset =
            WorldPoint((noise_map_width / 2.0f) * cell_width,
                       (noise_map_height / 2.0f) * cell_height);

        m_render_cells.reserve(cell_sprites.size());
        for (auto i = 0; i < cell_sprites.size(); i++) {
            const int x = (i % noise_map_width) * cell_width;
            const int y = (i / noise_map_width) * cell_height;
            m_render_cells.push_back(graphics_pipeline::GeometryInstanceBufferObject{
                .center = WorldPoint(x, y) - center_offset,
                .dimension = Dimension(cell_width, cell_height),
                .uvwt = cell_sprites[i],
            });
        }

        rectangle_instance_buffer.transfer();

        auto camera_transform_projection = m_camera.get_transform_projection_matrix();
        m_geometry_pipeline->render_rectangles(command_buffer,
                                               camera_transform_projection);

        render_pass.end_submit_present();
    };
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "5_map_generation"},
    };

    auto game = std::make_unique<MapGeneration>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
