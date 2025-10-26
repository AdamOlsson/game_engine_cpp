#pragma once

#include "tiling/traits.h"
#include "tiling/wang/TilesetIndex.h"
#include <optional>
#include <set>

namespace tiling::wang {

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

template <WangEnumUint8 T> class TilesetConstraints {
  private:
    std::unordered_map<std::tuple<T, T, T, T>, TilesetIndex, ConstraintHash<T>>
        m_constraints;

  public:
    TilesetConstraints() = default;
    ~TilesetConstraints() = default;

    TilesetConstraints<T> &add_constraint(const TilesetIndex &&index,
                                          const TileConstraint<T> &&constraint) {

        for (const auto &n : constraint.north) {
            for (const auto &e : constraint.east) {
                for (const auto &s : constraint.south) {
                    for (const auto &w : constraint.west) {
                        m_constraints[std::make_tuple(n, e, s, w)] = std::move(index);
                    }
                }
            }
        }
        return *this;
    }

    std::optional<TilesetIndex> lookup_sprite(const T north, const T east, const T south,
                                              const T west) {
        return std::nullopt;
    }

    std::optional<TilesetIndex> lookup_sprite(const std::tuple<T, T, T, T> &constraint) {
        return std::nullopt;
    }
};
} // namespace tiling::wang
