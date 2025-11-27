#pragma once

#include "ifstream_util.h"
#include <cstdint>
#include <fstream>
#include <sstream>

// Documentation
// https://learn.microsoft.com/en-us/typography/opentype/spec/hhea

struct FontTableHhea {
    uint16_t major_version = 0;
    uint16_t minor_version = 0;
    int16_t ascender = 0;
    int16_t descender = 0;
    int16_t line_gap = 0;
    uint16_t advance_width_max = 0;
    int16_t min_left_side_bearing = 0;
    int16_t min_right_side_bearing = 0;
    int16_t max_extent = 0;
    int16_t caret_slope_rise = 0;
    int16_t caret_slope_run = 0;
    int16_t caret_offset = 0;
    int16_t reserved1 = 0;
    int16_t reserved2 = 0;
    int16_t reserved3 = 0;
    int16_t reserved4 = 0;
    int16_t metric_data_format = 0;
    uint16_t number_of_h_metrics = 0;

    static FontTableHhea read_font_table_hhea(std::ifstream &stream) {
        return FontTableHhea{
            .major_version = read_uint16(stream),
            .minor_version = read_uint16(stream),
            .ascender = read_int16(stream),
            .descender = read_int16(stream),
            .line_gap = read_int16(stream),
            .advance_width_max = read_uint16(stream),
            .min_left_side_bearing = read_int16(stream),
            .min_right_side_bearing = read_int16(stream),
            .max_extent = read_int16(stream),
            .caret_slope_rise = read_int16(stream),
            .caret_slope_run = read_int16(stream),
            .caret_offset = read_int16(stream),
            .reserved1 = read_int16(stream),
            .reserved2 = read_int16(stream),
            .reserved3 = read_int16(stream),
            .reserved4 = read_int16(stream),
            .metric_data_format = read_int16(stream),
            .number_of_h_metrics = read_uint16(stream),
        };
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableHhea {\n"
            << "\tmajor_version: " << major_version << "\n"
            << "\tminor_version: " << minor_version << "\n"
            << "\tascender: " << ascender << "\n"
            << "\tdescender: " << descender << "\n"
            << "\tline_gap: " << line_gap << "\n"
            << "\tadvance_width_max: " << advance_width_max << "\n"
            << "\tmin_left_side_bearing: " << min_left_side_bearing << "\n"
            << "\tmin_right_side_bearing: " << min_right_side_bearing << "\n"
            << "\tmax_extent: " << max_extent << "\n"
            << "\tcaret_slope_rise: " << caret_slope_rise << "\n"
            << "\tcaret_slope_run: " << caret_slope_run << "\n"
            << "\tcaret_offset: " << caret_offset << "\n"
            << "\treserved1: " << reserved1 << "\n"
            << "\treserved2: " << reserved2 << "\n"
            << "\treserved3: " << reserved3 << "\n"
            << "\treserved4: " << reserved4 << "\n"
            << "\tmetric_data_format: " << metric_data_format << "\n"
            << "\tnumber_of_h_metrics: " << number_of_h_metrics << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableHhea &obj) {
        return os << obj.to_string();
    }
};
