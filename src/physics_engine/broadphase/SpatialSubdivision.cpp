#include "physics_engine/broadphase/SpatialSubdivision.h"
#include "equations/equations.h"
#include "glm/fwd.hpp"
#include "physics_engine/RigidBody.h"

#include <sys/_types/_u_int8_t.h>
#include <vector>
struct BoundingCircle {
    glm::vec3 center;
    float radius;
};

struct BoundingVolumes {
    std::vector<BoundingCircle> volumes;
    float largest_radius = 0.0f;
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
};

typedef u_int8_t ControlBits;
const ControlBits CONTROL_BIT_BOUNDING_VOLUME_1 = 0b0000'0001;
const ControlBits CONTROL_BIT_BOUNDING_VOLUME_2 = 0b0000'0010;
const ControlBits CONTROL_BIT_BOUNDING_VOLUME_3 = 0b0000'0100;
const ControlBits CONTROL_BIT_BOUNDING_VOLUME_4 = 0b0000'1000;
const ControlBits CONTROL_BIT_HOME_CELL_1 = 0b0000'0000;
const ControlBits CONTROL_BIT_HOME_CELL_2 = 0b0001'0000;
const ControlBits CONTROL_BIT_HOME_CELL_3 = 0b0010'0000;
const ControlBits CONTROL_BIT_HOME_CELL_4 = 0b0011'0000;
const ControlBits BOUNDING_VOLUME_MASK = 0b0000'1111;
const ControlBits HOME_CELL_MASK = 0b1111'0000;

enum class CellType { Home, Phantom };

struct CellVolume {
    size_t x;
    size_t y;
    size_t z;
    CellType cell_type;
    size_t volume_id;
};

BoundingVolumes create_bounding_volumes(const std::vector<const RigidBody> &bodies);
std::tuple<std::vector<ControlBits>, std::vector<CellVolume>>
create_cell_volumes(const std::vector<BoundingCircle> &bounding_volumes,
                    const float cell_width);
std::tuple<ControlBits, std::vector<CellVolume>>
create_cell_volume(const BoundingCircle &volume, const size_t volume_id,
                   float cell_width);
inline uint8_t get_control_bits_for_home_cell(const CellVolume &home_cell);
inline uint8_t get_control_bits_for_phantom_cell(const CellVolume &phantom_cell);

inline CellVolume create_phantom_cell_volume_for_top_cell(const CellVolume &home_cell);
inline CellVolume
create_phantom_cell_volume_for_top_right_cell(const CellVolume &home_cell);
inline CellVolume
create_phantom_cell_volume_for_top_left_cell(const CellVolume &home_cell);
inline CellVolume create_phantom_cell_volume_for_left_cell(const CellVolume &home_cell);
inline CellVolume create_phantom_cell_volume_for_bottom_cell(const CellVolume &home_cell);
inline CellVolume
create_phantom_cell_volume_for_bottom_left_cell(const CellVolume &home_cell);
inline CellVolume
create_phantom_cell_volume_for_bottom_right_cell(const CellVolume &home_cell);
inline CellVolume create_phantom_cell_volume_for_right_cell(const CellVolume &home_cell);

void SpatialSubdivision::collision_detection(const std::vector<const RigidBody> &bodies) {
    BoundingVolumes bounding_volumes = create_bounding_volumes(bodies);
    float cell_width = bounding_volumes.largest_radius * 2.0 * 1.41;
    auto [control_bits, cell_volumes] =
        create_cell_volumes(bounding_volumes.volumes, cell_width);
    // TODO: Sort the cell_volumes according cell id (x,y,z)
}

BoundingVolumes create_bounding_volumes(const std::vector<const RigidBody> &bodies) {
    float scale_factor = 1.41f;

    BoundingVolumes intermediate_results;
    intermediate_results.volumes.resize(bodies.size());
    std::for_each(bodies.begin(), bodies.end(), [&](const RigidBody &body) {
        float radius = body.bounding_volume_radius() * scale_factor;
        BoundingCircle bounding_circle{.radius = radius, .center = body.position};
        intermediate_results.volumes.push_back(std::move(bounding_circle));
        intermediate_results.largest_radius =
            std::max(intermediate_results.largest_radius, radius);
        intermediate_results.min_x =
            std::min(intermediate_results.min_x, body.position.x);
        intermediate_results.min_y =
            std::min(intermediate_results.min_y, body.position.y);
        intermediate_results.min_z =
            std::min(intermediate_results.min_z, body.position.z);
    });

    // Offset all volumes to be on the positive quadrant
    for (auto &volume : intermediate_results.volumes) {
        volume.center.x -= intermediate_results.min_x;
        volume.center.y -= intermediate_results.min_y;
        volume.center.z -= intermediate_results.min_z;
    }

    return intermediate_results;
}

std::tuple<std::vector<ControlBits>, std::vector<CellVolume>>
create_cell_volumes(const std::vector<BoundingCircle> &bounding_volumes,
                    const float cell_width) {
    std::vector<ControlBits> control_bits = {};
    std::vector<CellVolume> cell_volumes = {};
    for (size_t i = 0; i < bounding_volumes.size(); i++) {
        auto [control, volumes] = create_cell_volume(bounding_volumes[i], i, cell_width);
        control_bits.push_back(std::move(control));
        cell_volumes.insert(cell_volumes.end(), std::make_move_iterator(volumes.begin()),
                            std::make_move_iterator(volumes.end()));
    }
    return std::tuple(control_bits, cell_volumes);
}

std::tuple<ControlBits, std::vector<CellVolume>>
create_cell_volume(const BoundingCircle &volume, const size_t volume_id,
                   float cell_width) {
    const glm::vec3 grid_normalized_pos = volume.center / cell_width;
    const float grid_normalized_radius = volume.radius / cell_width;

    const CellVolume home_cell = {
        .x = static_cast<size_t>(std::floor(grid_normalized_pos.x)),
        .y = static_cast<size_t>(std::floor(grid_normalized_pos.y)),
        .z = static_cast<size_t>(std::floor(grid_normalized_pos.z)),
        .cell_type = CellType::Home,
        .volume_id = volume_id};
    ControlBits control_bits = get_control_bits_for_home_cell(home_cell);

    const float quad_x = grid_normalized_pos.x - std::floor(grid_normalized_pos.x);
    const float quad_y = grid_normalized_pos.y - std::floor(grid_normalized_pos.y);
    /*const float quad_z = grid_normalized_pos.z - std::floor(grid_normalized_pos.z);*/

    std::vector<CellVolume> cell_volumes = {std::move(home_cell)};
    if (quad_x < 0.5 && quad_y < 0.5) {
        if (quad_x - grid_normalized_radius < 0.0) {
            CellVolume v = create_phantom_cell_volume_for_left_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        if (quad_y - grid_normalized_radius < 0.0) {
            CellVolume v = create_phantom_cell_volume_for_top_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        const glm::vec3 home_cell_top_left_corner = glm::vec3(
            std::floor(grid_normalized_pos.x), std::floor(grid_normalized_pos.y),
            std::floor(grid_normalized_pos.z));
        if (Equations::distance2(home_cell_top_left_corner, grid_normalized_pos) <
            pow(grid_normalized_radius, 2.0f)) {
            CellVolume v = create_phantom_cell_volume_for_top_left_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

    } else if (quad_x >= 0.5 && quad_y < 0.5) {
        if (quad_x + grid_normalized_radius > 1.0) {
            CellVolume v = create_phantom_cell_volume_for_right_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        if (quad_y - grid_normalized_radius < 0.0) {
            CellVolume v = create_phantom_cell_volume_for_top_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        const glm::vec3 home_cell_top_right_corner = glm::vec3(
            std::floor(grid_normalized_pos.x + 1.0f), std::floor(grid_normalized_pos.y),
            std::floor(grid_normalized_pos.z));
        if (Equations::distance2(home_cell_top_right_corner, grid_normalized_pos) <
            pow(grid_normalized_radius, 2.0f)) {
            CellVolume v = create_phantom_cell_volume_for_top_right_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

    } else if (quad_x < 0.5 && quad_y >= 0.5) {
        if (quad_x - grid_normalized_radius < 0.0) {
            CellVolume v = create_phantom_cell_volume_for_left_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        if (quad_y + grid_normalized_radius > 1.0) {
            CellVolume v = create_phantom_cell_volume_for_bottom_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        const glm::vec3 home_cell_bottom_left_corner = glm::vec3(
            std::floor(grid_normalized_pos.x), std::floor(grid_normalized_pos.y + 1.0f),
            std::floor(grid_normalized_pos.z));
        if (Equations::distance2(home_cell_bottom_left_corner, grid_normalized_pos) <
            pow(grid_normalized_radius, 2.0f)) {
            CellVolume v = create_phantom_cell_volume_for_bottom_left_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

    } else {
        if (quad_x + grid_normalized_radius > 1.0) {
            CellVolume v = create_phantom_cell_volume_for_right_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        if (quad_y + grid_normalized_radius > 1.0) {
            CellVolume v = create_phantom_cell_volume_for_bottom_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }

        const glm::vec3 home_cell_bottom_right_corner = glm::vec3(
            std::floor(grid_normalized_pos.x + 1.0f),
            std::floor(grid_normalized_pos.y + 1.0f), std::floor(grid_normalized_pos.z));
        if (Equations::distance2(home_cell_bottom_right_corner, grid_normalized_pos) <
            pow(grid_normalized_radius, 2.0f)) {
            CellVolume v = create_phantom_cell_volume_for_bottom_right_cell(home_cell);
            cell_volumes.push_back(std::move(v));
            control_bits |= get_control_bits_for_phantom_cell(v);
        }
    }

    return std::tuple(control_bits, cell_volumes);
}

inline uint8_t get_control_bits_for_home_cell(const CellVolume &home_cell) {
    uint32_t x_mod = home_cell.x % 2;
    uint32_t y_mod = home_cell.y % 2;
    uint32_t z_mod = home_cell.z % 2;
    constexpr uint8_t lookup_table[2][2] = {
        {CONTROL_BIT_HOME_CELL_1 | CONTROL_BIT_BOUNDING_VOLUME_1,
         CONTROL_BIT_HOME_CELL_3 | CONTROL_BIT_BOUNDING_VOLUME_3},
        {CONTROL_BIT_HOME_CELL_2 | CONTROL_BIT_BOUNDING_VOLUME_2,
         CONTROL_BIT_HOME_CELL_4 | CONTROL_BIT_BOUNDING_VOLUME_4}};
    return lookup_table[x_mod][y_mod];
}

inline uint8_t get_control_bits_for_phantom_cell(const CellVolume &phantom_cell) {
    uint32_t x_mod = phantom_cell.x % 2;
    uint32_t y_mod = phantom_cell.y % 2;
    uint32_t z_mod = phantom_cell.z % 2;
    constexpr uint8_t lookup_table[2][2] = {
        {CONTROL_BIT_BOUNDING_VOLUME_1, CONTROL_BIT_BOUNDING_VOLUME_3},
        {CONTROL_BIT_BOUNDING_VOLUME_2, CONTROL_BIT_BOUNDING_VOLUME_4}};
    return lookup_table[x_mod][y_mod];
}

inline CellVolume
create_phantom_cell_volume_for_top_right_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x + 1,
                      .y = home_cell.y - 1,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}

inline CellVolume
create_phantom_cell_volume_for_top_left_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x - 1,
                      .y = home_cell.y - 1,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}

inline CellVolume
create_phantom_cell_volume_for_bottom_left_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x - 1,
                      .y = home_cell.y + 1,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}

inline CellVolume
create_phantom_cell_volume_for_bottom_right_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x + 1,
                      .y = home_cell.y + 1,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}

inline CellVolume
create_phantom_cell_volume_for_bottom_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x,
                      .y = home_cell.y + 1,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}

inline CellVolume create_phantom_cell_volume_for_top_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x,
                      .y = home_cell.y - 1,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}

inline CellVolume create_phantom_cell_volume_for_right_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x + 1,
                      .y = home_cell.y,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}

inline CellVolume create_phantom_cell_volume_for_left_cell(const CellVolume &home_cell) {
    return CellVolume{.x = home_cell.x - 1,
                      .y = home_cell.y,
                      .z = home_cell.z,
                      .cell_type = CellType::Phantom,
                      .volume_id = home_cell.volume_id};
}
