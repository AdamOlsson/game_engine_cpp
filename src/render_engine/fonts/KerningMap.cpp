#include "KerningMap.h"

const font::KerningMap &font::get_default_kerning_map() {
    static const KerningMap map = {
        .map = {{"IN", 0.75f}, {"SE", 0.65f}, {"ET", 0.65f}, {"TT", 0.65f}, {"TI", 0.75f},
                {"NG", 0.62f}, {"GS", 0.62f}, {"EX", 0.65f}, {"XI", 0.75f}, {"IT", 0.78f},
                {"1.", 0.78f}, {"VE", 0.62f}, {"SI", 0.75f}, {"IO", 0.75f}, {"ON", 0.62f},
                {"RS", 0.62f}, {".2", 0.75f}, {"23", 0.62f}, {"34", 0.62f}, {"ER", 0.62f},
                {"N ", 0.9f},  {" 1", 0.5f},  {"AD", 0.65f}, {"DA", 0.67f}, {"AM", 0.65f},
                {"LI", 0.78f}, {"ND", 0.65f}

        }};
    return map;
}
