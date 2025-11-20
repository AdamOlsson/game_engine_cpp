#pragma once

#include "logger/logger.h"
#include "tiling/Position.h"
#include "tiling/TileGrid.h"
#include <cfloat>
#include <set>
#include <vector>

namespace tiling::search {

struct Cell {
    Position parent;
    float g = 0.0f;    // Sum of cost of all steps on route
    float h = 0.0f;    // Estimated cost of moving from selected node to the goal
    float f = FLT_MAX; // g + h
};

constexpr Position NORTH = Position(0, 1);
constexpr Position EAST = Position(1, 0);
constexpr Position SOUTH = Position(0, -1);
constexpr Position WEST = Position(-1, 0);

class AStar {
  private:
    static float heuristic(const Position &node, const Position &end) {
        return node.manhattan_distance(end);
    }

    static std::vector<Position>
    reconstruct_path(const std::unordered_map<Position, Cell, PositionHash> &cell_details,
                     const Position &start, const Position &end) {

        std::stack<Position> reverse_path;
        auto node = cell_details.at(end);
        reverse_path.push(end);

        while (node.parent != start) {
            reverse_path.push(node.parent);
            logger::debug("Adding: ", reverse_path.top());
            node = cell_details.at(node.parent);
        }
        reverse_path.push(start);

        /*logger::debug("======= Reconstructing path =======");*/
        std::vector<Position> path;
        path.reserve(reverse_path.size());
        while (!reverse_path.empty()) {
            path.push_back(std::move(reverse_path.top()));
            /*logger::debug("Adding: ", path.back());*/
            reverse_path.pop();
        }

        /*logger::debug("============== done ===============");*/
        /*logger::debug("Path size:", path.size());*/
        return path;
    }

  public:
    template <typename T>
    static std::vector<Position> search(const tiling::TileGrid<T> &grid,
                                        const Position &start, const Position &end) {

        if (start == end) {
            // Do not return an empty list as we want to distinguish between no step
            // and no solution found
            return {start, end};
        }

        auto cell_details = std::unordered_map<Position, Cell, PositionHash>();
        // Set arranges floats in ascending order, and by extension tuple<float, T> as
        // well
        std::set<std::tuple<float, Position>> open_list;
        open_list.insert(std::make_tuple(0.0f, start));

        while (!open_list.empty()) {
            // Get the node with the smallest 'f' in the open list.
            auto q = *open_list.begin();
            open_list.erase(open_list.begin());

            auto q_f = std::get<0>(q);
            auto q_pos = std::get<1>(q);
            auto &q_details = cell_details[q_pos];

            std::vector<Position> cardinal_successors = {
                q_pos + NORTH,
                q_pos + EAST,
                q_pos + SOUTH,
                q_pos + WEST,
            };
            std::vector<Position> diagonal_successors = {
                q_pos + NORTH + EAST,
                q_pos + SOUTH + EAST,
                q_pos + SOUTH + WEST,
                q_pos + NORTH + WEST,
            };

            for (const auto &suc_pos : cardinal_successors) {
                if (suc_pos == end) {
                    cell_details[suc_pos].parent = q_pos;
                    return reconstruct_path(cell_details, start, end);
                }

                // Compute g, h and f
                const float new_suc_g =
                    q_details.g + grid.get_cell(suc_pos.x, suc_pos.y).weight;
                const float new_suc_h = heuristic(suc_pos, end);
                const float new_suc_f = new_suc_g + new_suc_h;

                // If a node exists in the OPEN/CLOSED list which has a lower f score
                // than successor, skip this successor
                if (cell_details[suc_pos].f <= new_suc_f) {
                    continue;
                }

                open_list.insert(std::make_tuple(new_suc_f, suc_pos));
                cell_details[suc_pos].parent = q_pos;
                cell_details[suc_pos].g = new_suc_g;
                cell_details[suc_pos].h = new_suc_h;
                cell_details[suc_pos].f = new_suc_f;
            }

            for (const auto &suc_pos : diagonal_successors) {
                if (suc_pos == end) {
                    cell_details[suc_pos].parent = q_pos;
                    return reconstruct_path(cell_details, start, end);
                }

                // Compute g, h and f
                const float new_suc_g =
                    q_details.g + grid.get_cell(suc_pos.x, suc_pos.y).weight;
                const float new_suc_h = heuristic(suc_pos, end);
                const float new_suc_f = new_suc_g + new_suc_h;

                // If a node exists in the OPEN/CLOSED list which has a lower f score
                // than successor, skip this successor
                if (cell_details[suc_pos].f <= new_suc_f) {
                    continue;
                }

                open_list.insert(std::make_tuple(new_suc_f, suc_pos));
                cell_details[suc_pos].parent = q_pos;
                cell_details[suc_pos].g = new_suc_g;
                cell_details[suc_pos].h = new_suc_h;
                cell_details[suc_pos].f = new_suc_f;
            }
        }

        return {};
    }

    void step() {}
};

} // namespace tiling::search
