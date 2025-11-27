#pragma once

#include "ifstream_util.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

using Card8 = uint8_t;
using Card16 = uint16_t;

using OffSize = uint8_t;
using Offset1 = uint8_t;
using Offset2 = uint16_t;
// using Offset3 = uint24_t; // Hope I never get to use this
using Offset4 = uint32_t;

namespace cff {

/*struct Index {*/
/*    Card16 count = 0;*/
/*    OffSize off_size = 0;*/
/*    off_size = 0;*/
/*};*/

struct FontTableCFF {
    struct {
        Card8 major_version = 0;
        Card8 minor_version = 0;
        Card8 hdr_size = 0;
        OffSize off_size = 0;
    } header;

    static FontTableCFF read_font_table_cff(std::ifstream &stream) {
        auto cff = FontTableCFF{
            .header =
                {
                    .major_version = read_uint8(stream),
                    .minor_version = read_uint8(stream),
                    .hdr_size = read_uint8(stream),
                    .off_size = read_uint8(stream),
                },
        };

        return cff;
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableCFF {\n"
            << "\theader: {" << "\n"
            << "\t\tmajor_version: " << static_cast<int>(header.major_version) << "\n"
            << "\t\tminor_version: " << static_cast<int>(header.minor_version) << "\n"
            << "\t\thdr_size: " << static_cast<int>(header.hdr_size) << "\n"
            << "\t\toff_size: " << static_cast<int>(header.off_size) << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableCFF &obj) {
        return os << obj.to_string();
    }
};

} // namespace cff
