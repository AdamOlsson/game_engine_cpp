#include "KerningMap.h"

const std::map<std::string, float> &font::get_default_kerning_map() {
    static const std::map<std::string, float> map = {{"EX", 0.0f}};
    return map;
}
