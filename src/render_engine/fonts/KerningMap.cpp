#include "KerningMap.h"

const font::KerningMap &font::get_default_kerning_map() {
    static const KerningMap map = {.base_offset = 12.0f,
                                   .map = {
                                       {"IE", 0.0f},
                                       {"IN", 6.0f},
                                   }};
    return map;
}
