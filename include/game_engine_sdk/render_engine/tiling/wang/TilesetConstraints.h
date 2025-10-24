#pragma once

#include "game_engine_sdk/render_engine/tiling/wang/traits.h"
#include <optional>
#include <set>

namespace tiling {

template <WangEnumUint8 T> struct ConstraintHash {
    std::size_t operator()(const std::tuple<T, T, T, T> &t) const {
        return (static_cast<size_t>(std::get<0>(t)) << 24) |
               (static_cast<size_t>(std::get<1>(t)) << 16) |
               (static_cast<size_t>(std::get<2>(t)) << 8) |
               (static_cast<size_t>(std::get<3>(t)));
    }
};

template <WangEnumUint8 T> struct TileConstraint {
    std::set<T> north;
    std::set<T> east;
    std::set<T> south;
    std::set<T> west;
};

using SpriteIndex = std::tuple<unsigned int, unsigned int>;

template <WangEnumUint8 T> class TilesetConstraints {
  private:
    std::unordered_map<std::tuple<T, T, T, T>, SpriteIndex, ConstraintHash<T>>
        m_constraints;

  public:
    TilesetConstraints() = default;
    ~TilesetConstraints() = default;

    TilesetConstraints<T> &add_constraint(const unsigned int x, const unsigned y,
                                          const TileConstraint<T> &&constraint) {
        // CONTINUE: Generate all possible combinations of the constraint and add them
        // to the unordered_map
        return *this;
    }

    std::optional<SpriteIndex> lookup_sprite(const T &north, const T &east,
                                             const T &south, const T &west) {
        return std::nullopt;
    }
};
} // namespace tiling
