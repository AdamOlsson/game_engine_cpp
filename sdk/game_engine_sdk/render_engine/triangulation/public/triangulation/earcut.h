#pragma once

#include <vector>
namespace triangulation {

std::vector<std::array<size_t, 3>>
earclip(const std::vector<std::pair<int, int>> &vertices);

} // namespace triangulation
