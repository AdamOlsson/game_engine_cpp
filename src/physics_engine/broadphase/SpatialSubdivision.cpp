#include "physics_engine/broadphase/SpatialSubdivision.h"
#include "equations/equations.h"
#include "glm/fwd.hpp"
#include "io.h"
#include "physics_engine/RigidBody.h"
#include <cstddef>
#include <cstdint>
#include <sys/_types/_u_int8_t.h>
#include <vector>

constexpr ControlBits CONTROL_BIT_BOUNDING_VOLUME_1 = 0b0000'0001;
constexpr ControlBits CONTROL_BIT_BOUNDING_VOLUME_2 = 0b0000'0010;
constexpr ControlBits CONTROL_BIT_BOUNDING_VOLUME_3 = 0b0000'0100;
constexpr ControlBits CONTROL_BIT_BOUNDING_VOLUME_4 = 0b0000'1000;
constexpr ControlBits CONTROL_BIT_HOME_CELL_1 = 0b0000'0000;
constexpr ControlBits CONTROL_BIT_HOME_CELL_2 = 0b0001'0000;
constexpr ControlBits CONTROL_BIT_HOME_CELL_3 = 0b0010'0000;
constexpr ControlBits CONTROL_BIT_HOME_CELL_4 = 0b0011'0000;
constexpr ControlBits BOUNDING_VOLUME_MASK = 0b0000'1111;
constexpr ControlBits HOME_CELL_MASK = 0b1111'0000;

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

enum class CellType { Home, Phantom };

struct CellVolume {
    size_t x;
    size_t y;
    size_t z;
    CellType cell_type;
    size_t volume_id;
};

std::ostream &operator<<(std::ostream &os, const CellVolume &v);
std::ostream &operator<<(std::ostream &os, const BoundingVolumes &bvs);
std::ostream &operator<<(std::ostream &os, const BoundingCircle &bc);
std::ostream &operator<<(std::ostream &os, const std::vector<BoundingCircle> &bcs);
std::ostream &operator<<(std::ostream &os, const ControlBits &value);
std::ostream &operator<<(std::ostream &os, const std::vector<ControlBits> &vs);
std::ostream &operator<<(std::ostream &os, const std::vector<CellVolume> &cvs);

BoundingVolumes create_bounding_volumes(const std::vector<RigidBody> &bodies);
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
inline size_t cell_id_order(const CellVolume &);

std::vector<std::tuple<size_t, size_t>>
count_volumes_per_cell(const std::vector<CellVolume> &volumes);
SpatialSubdivisionResult
create_passes(const std::vector<std::tuple<size_t, size_t>> &cell_volume_count,
              const std::vector<CellVolume> &cell_volumes,
              const std::vector<ControlBits> &control_bits);

inline bool can_we_skip_narrow_collision_check(const uint8_t pass_num,
                                               const ControlBits ctrl_a,
                                               const ControlBits ctrl_b);

/// Runs a broadphase collision detection
///
/// Home cell is the cell which the rigid bodys center point is located. Phantom cells
/// are all cells the rigid bodys bounding circle covers, including the home cell.
SpatialSubdivisionResult
SpatialSubdivision::collision_detection(const std::vector<RigidBody> &bodies) {
    BoundingVolumes bounding_volumes = create_bounding_volumes(bodies);
    float cell_width = bounding_volumes.largest_radius * 2.0;
    auto [control_bits, cell_volumes] =
        create_cell_volumes(bounding_volumes.volumes, cell_width);

    std::sort(cell_volumes.begin(), cell_volumes.end(), [](const auto &a, const auto &b) {
        return cell_id_order(a) < cell_id_order(b);
    });

    auto cell_count = count_volumes_per_cell(cell_volumes);
    return create_passes(cell_count, cell_volumes, control_bits);
}

// Withing one cell, evaluate if we can skip the narrow collision check between all
// pairs. If not, append the pair to the respecive pass vector
SpatialSubdivisionResult SpatialSubdivision::create_passes(
    const std::vector<std::tuple<size_t, size_t>> &cell_volume_count,
    const std::vector<CellVolume> &cell_volumes,
    const std::vector<ControlBits> &control_bits) {

    SpatialSubdivisionResult result{};
    for (auto [start_idx, count] : cell_volume_count) {
        if (count < 2) {
            continue;
        }
        const uint8_t pass_num =
            get_control_bits_for_phantom_cell(cell_volumes[start_idx]);

        std::vector<CollisionCandidatePair> collision_candidate_pairs = {};

        // Evaluate if any rigid bodies' bounding volume covering the cell needs
        // further narrow check collision check
        for (size_t a = start_idx; a < start_idx + count; a++) {
            const ControlBits ctrl_a = control_bits[a];
            for (size_t b = a + 1; b < start_idx + count; b++) {
                const ControlBits ctrl_b = control_bits[b];
                if (can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b)) {
                    continue;
                }
                collision_candidate_pairs.push_back(
                    std::tuple(cell_volumes[a].volume_id, cell_volumes[b].volume_id));
            }
        }

        if (collision_candidate_pairs.size() == 0) {
            continue;
        }

        switch (pass_num) {
        case 1:
            result.pass1.push_back(collision_candidate_pairs);
            break;
        case 2:
            result.pass2.push_back(collision_candidate_pairs);
            break;
        case 3:
            result.pass3.push_back(collision_candidate_pairs);
            break;
        case 4:
            result.pass4.push_back(collision_candidate_pairs);
            break;
        }
    }

    // TODO: debug asserts

    return result;
}

inline bool can_we_skip_narrow_collision_check(const uint8_t pass_num,
                                               const ControlBits ctrl_a,
                                               const ControlBits ctrl_b) {
    const ControlBits home_cell_type_a = (ctrl_a & HOME_CELL_MASK) >> 4;
    const ControlBits home_cell_type_b = (ctrl_b & HOME_CELL_MASK) >> 4;

    // Translate home cell to phantom cell
    const ControlBits home_cell_a_phantom_bit = 1 << home_cell_type_a;
    const ControlBits home_cell_b_phantom_bit = 1 << home_cell_type_b;

    const ControlBits phantom_bits_a = ctrl_a & BOUNDING_VOLUME_MASK;
    const ControlBits phantom_bits_b = ctrl_b & BOUNDING_VOLUME_MASK;
    const ControlBits phantom_bits_common = phantom_bits_a & phantom_bits_b;

    const bool is_home_cell_a_among_common_phantom_cells =
        (home_cell_a_phantom_bit & phantom_bits_common) > 0;
    const bool is_home_cell_b_among_common_phantom_cells =
        (home_cell_b_phantom_bit & phantom_bits_common) > 0;

    const bool is_home_cell_type_a_less_than_pass_num = (home_cell_type_a + 1) < pass_num;
    const bool is_home_cell_type_b_less_than_pass_num = (home_cell_type_b + 1) < pass_num;

    const bool predicate_a = is_home_cell_type_b_less_than_pass_num &&
                             is_home_cell_a_among_common_phantom_cells;

    const bool predicate_b = is_home_cell_type_b_less_than_pass_num &&
                             is_home_cell_b_among_common_phantom_cells;

    return predicate_a || predicate_b;
}

/// Given a vector of cell volumes sorted after which cell the volume occupies, the
/// function counts the number of volumes occupying the cell each cell and stores the
/// index where the group starts and how many volumes there are.
std::vector<std::tuple<size_t, size_t>>
count_volumes_per_cell(const std::vector<CellVolume> &volumes) {
    if (volumes.size() == 0) {
        return {};
    }

    std::vector<std::tuple<size_t, size_t>> index_list = {};
    size_t volume_count = 0;
    size_t current_start_index = 0;
    auto last_cell = cell_id_order(volumes[0]);
    for (size_t i = 0; i < volumes.size(); i++) {
        const auto &vol = volumes[i];
        const auto current_cell = cell_id_order(vol);
        ++volume_count;

        /*std::cout << "Cell hash: " << current_cell << std::endl;*/
        /*std::cout << "i, volumes.size(): " << i << ", " << volumes.size() - 1*/
        /*          << std::endl;*/

        if (last_cell != current_cell) {
            index_list.push_back(std::tuple(current_start_index, volume_count - 1));

            if (i == volumes.size() - 1) {
                index_list.push_back(std::tuple(i, 1));
            }

            current_start_index = i;
            volume_count = 1;
            last_cell = current_cell;
            continue;
        }

        if (i == volumes.size() - 1) {
            index_list.push_back(std::tuple(current_start_index, volume_count));
        }
    }
    return index_list;
}

/// For each of the bodies, create a bounding circle that encapsulates each body
BoundingVolumes create_bounding_volumes(const std::vector<RigidBody> &bodies) {
    float scale_factor = 1.41f;

    BoundingVolumes intermediate_results;
    intermediate_results.volumes.reserve(bodies.size());
    std::for_each(bodies.begin(), bodies.end(), [&](const RigidBody &body) {
        float radius = body.bounding_volume_radius() * scale_factor;
        BoundingCircle bounding_circle{.center = body.position, .radius = radius};
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

    // Offset all volumes to be on the positive quadrant, we also move by the largest
    // radius because if we didn't we would move the top left most object to origo. We
    // want no object to span over any other quadrant. Moving by largest radius is an
    // overcorrection, which won't have any computational effects. The exakt correction
    // would be to offset all objects by the min_x/y/z + the radius of the most top left
    // object. However, computing the radius of the top left most object is unnecessary as
    // we can simply use the largest radius.
    for (auto &volume : intermediate_results.volumes) {
        volume.center.x = volume.center.x - intermediate_results.min_x +
                          intermediate_results.largest_radius;
        volume.center.y = volume.center.y - intermediate_results.min_y +
                          intermediate_results.largest_radius;
        volume.center.z = volume.center.z - intermediate_results.min_z +
                          intermediate_results.largest_radius;
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
    /*const float quad_z = grid_normalized_pos.z -
     * std::floor(grid_normalized_pos.z);*/

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

inline size_t cell_id_order(const CellVolume &v) {
    return v.x + v.y * 1000 + v.z * 1000000;
}

std::ostream &operator<<(std::ostream &os, const BoundingVolumes &bvs) {
    return os << "BoundingVolumes(largest_radius: " << bvs.largest_radius << ", min: ("
              << bvs.min_x << ", " << bvs.min_y << ", " << bvs.min_z
              << "), volumes: " << bvs.volumes << ")";
}

std::ostream &operator<<(std::ostream &os, const BoundingCircle &bc) {
    return os << "BoundingCircle(center: " << bc.center << ", radius: " << bc.radius
              << ")";
}

std::ostream &operator<<(std::ostream &os, const std::vector<BoundingCircle> &bcs) {
    os << "[\n";
    for (auto bv : bcs) {
        os << "  " << bv << ",\n";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const CellType &v) {
    switch (v) {
    case CellType::Home:
        return os << "CellType::Home";
    case CellType::Phantom:
        return os << "CellType::Phantom";
    }
}

std::ostream &operator<<(std::ostream &os, const CellVolume &v) {
    return os << "CellVolume( cell: (" << v.x << ", " << v.y << ", " << v.z
              << "), cell_type: " << v.cell_type << ", volume_id: " << v.volume_id << ")";
}

std::ostream &operator<<(std::ostream &os, const std::vector<CellVolume> &cvs) {
    os << "[\n";
    for (auto cv : cvs) {
        os << "  " << cv << ",\n";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const CollisionCandidatePair &ccp) {
    return os << "(" << std::get<0>(ccp) << ", " << std::get<1>(ccp) << ")";
}

std::ostream &operator<<(std::ostream &os, const CollisionCandidates &ccs) {
    os << "[";
    for (auto ccp : ccs) {
        os << ccp << ", ";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const SpatialSubdivisionResult &res) {
    os << "SpatialSubdivisionResult" << std::endl;
    os << "  Pass 1" << std::endl;
    for (auto cc : res.pass1) {
        os << "    " << cc << std::endl;
    }
    os << "  Pass 2" << std::endl;
    for (auto cc : res.pass2) {
        os << "    " << cc << std::endl;
    }
    os << "  Pass 3" << std::endl;
    for (auto cc : res.pass3) {
        os << "    " << cc << std::endl;
    }
    os << "  Pass 3" << std::endl;
    for (auto cc : res.pass3) {
        os << "    " << cc << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const ControlBits &value) {
    for (int i = 7; i >= 0; i--) {
        if (i == 3) {
            os << '\'';
        }
        if (((value >> i) & 1) == 1) {
            os << '1';
        } else {
            os << '0';
        }
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<ControlBits> &vs) {
    os << "[";
    for (auto v : vs) {
        os << v << ", ";
    }
    os << "]";
    return os;
}
