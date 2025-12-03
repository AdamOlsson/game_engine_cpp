#pragma once

#include "font/detail/ifstream_util.h"
#include <cstdint>
#include <fstream>
#include <sstream>

namespace font::detail::otf_font {
struct FontTableHead {
    uint16_t major_version = 0;
    uint16_t minor_version = 0;
    uint32_t font_revision = 0;
    uint32_t checksum_adjustment = 0;
    uint32_t magic_number = 0;
    uint16_t flags = 0;
    uint16_t units_per_em = 0;
    int64_t created = 0;
    int64_t modified = 0;
    int16_t x_min = 0;
    int16_t y_min = 0;
    int16_t x_max = 0;
    int16_t y_max = 0;
    uint16_t mac_style = 0;
    uint16_t lowest_rec_ppem = 0;
    int16_t font_direction_hint = 0;
    int16_t index_to_loc_format = 0;
    int16_t glyph_data_format = 0;

    static FontTableHead read_font_table_head(std::ifstream &stream) {
        return FontTableHead{
            .major_version = read_uint16(stream),
            .minor_version = read_uint16(stream),
            .font_revision = read_uint32(stream),
            .checksum_adjustment = read_uint32(stream),
            .magic_number = read_uint32(stream),
            .flags = read_uint16(stream),
            .units_per_em = read_uint16(stream),
            .created = read_int64(stream),
            .modified = read_int64(stream),
            .x_min = read_int16(stream),
            .y_min = read_int16(stream),
            .x_max = read_int16(stream),
            .y_max = read_int16(stream),
            .mac_style = read_uint16(stream),
            .lowest_rec_ppem = read_uint16(stream),
            .font_direction_hint = read_int16(stream),
            .index_to_loc_format = read_int16(stream),
            .glyph_data_format = read_int16(stream),
        };
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableHead {\n"
            << "\tmajor_version: " << major_version << "\n"
            << "\tminor_version: " << minor_version << "\n"
            << "\tfont_revision: 0x" << std::hex << font_revision << std::dec << "\n"
            << "\tchecksum_adjustment: 0x" << std::hex << checksum_adjustment << std::dec
            << "\n"
            << "\tmagic_number: 0x" << std::hex << magic_number << std::dec << "\n"
            << "\tflags: 0x" << std::hex << flags << std::dec << "\n"
            << "\tunits_per_em: " << units_per_em << "\n"
            << "\tcreated: " << created << "\n"
            << "\tmodified: " << modified << "\n"
            << "\tx_min: " << x_min << "\n"
            << "\ty_min: " << y_min << "\n"
            << "\tx_max: " << x_max << "\n"
            << "\ty_max: " << y_max << "\n"
            << "\tmac_style: 0x" << std::hex << mac_style << std::dec << "\n"
            << "\tlowest_rec_ppem: " << lowest_rec_ppem << "\n"
            << "\tfont_direction_hint: " << font_direction_hint << "\n"
            << "\tindex_to_loc_format: " << index_to_loc_format << "\n"
            << "\tglyph_data_format: " << glyph_data_format << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableHead &obj) {
        return os << obj.to_string();
    }
};
} // namespace font::detail::otf_font
