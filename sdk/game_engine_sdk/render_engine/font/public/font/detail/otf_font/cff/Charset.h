#pragma once

#include "font/detail/ifstream_util.h"
#include "font/detail/otf_font/cff/CFFIndex.h"
#include <cstdint>
#include <fstream>

namespace font::detail::otf_font::cff {

struct Range1 {
    uint16_t first;
    uint8_t n_left;

    static Range1 read(std::ifstream &stream) {
        Range1 range{};
        range.first = read_uint16(stream);
        range.n_left = read_uint8(stream);
        return range;
    }
};

struct Range2 {
    uint16_t first;
    uint16_t n_left;

    static Range2 read(std::ifstream &stream) {
        Range2 range{};
        range.first = read_uint16(stream);
        range.n_left = read_uint16(stream);
        return range;
    }
};

struct Charset {

    std::vector<std::string> glyph_names;

    static Charset read(std::ifstream &stream, const uint16_t &num_glyphs,
                        const CFFIndex &string_index) {

        Charset charset{};
        charset.glyph_names.reserve(num_glyphs);
        const int charset_format = read_uint8(stream);
        if (charset_format == 0) {
            for (auto i = 0; i < num_glyphs; i++) {
                int sid = read_uint16(stream);
                charset.glyph_names.emplace_back(
                    CFFIndex::lookup_string(string_index, sid));
            }
        } else if (charset_format == 1) {

            uint16_t count = 0;
            while (count < num_glyphs) {
                const Range1 range = Range1::read(stream);
                count += range.n_left + 1;
                for (size_t sid = range.first; sid < range.first + range.n_left + 1;
                     sid++) {
                    charset.glyph_names.emplace_back(
                        CFFIndex::lookup_string(string_index, sid));
                }
            }
        } else if (charset_format == 2) {

            uint16_t count = 0;
            while (count < num_glyphs) {
                const Range2 range = Range2::read(stream);
                count += range.n_left + 1;
                for (size_t sid = range.first; sid < range.first + range.n_left + 1;
                     sid++) {
                    charset.glyph_names.emplace_back(
                        CFFIndex::lookup_string(string_index, sid));
                }
            }
        } else {
            throw std::runtime_error(
                std::format("Error: unkown charset format {}", charset_format));
        }

        return charset;
    }

    std::string &operator[](size_t i) { return glyph_names[i]; }
};

} // namespace font::detail::otf_font::cff
