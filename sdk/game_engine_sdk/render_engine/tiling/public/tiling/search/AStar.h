#pragma once

namespace tiling::search {

enum class Heuristic {
    Manhattan,
    Euclidian,
};

namespace detail {
template <tiling::search::Heuristic H> class AStarImpl;
};

// namespace detail
using AStarManhattan =
    tiling::search::detail::AStarImpl<tiling::search::Heuristic::Manhattan>;
using AStarEuclidian =
    tiling::search::detail::AStarImpl<tiling::search::Heuristic::Manhattan>;
using AStar = AStarManhattan;

} // namespace tiling::search

#include "detail/AStarImpl.h"
