#pragma once

#include <map>
namespace font {
struct KerningMap {
    float base_offset;
    std::map<std::string, float, std::less<>> map;
};

const KerningMap &get_default_kerning_map();

}; // namespace font
