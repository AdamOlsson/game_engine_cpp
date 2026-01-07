#pragma once

#include "CFFIndex.h"
#include "Charset.h"
#include "PrivateData.h"
#include "TopData.h"
#include "Type2Charstring.h"

#include "font/detail/ifstream_util.h"

#include "font/winding.h"
#include "util/assert.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>

using Card8 = uint8_t;
using Card16 = uint16_t;

using OffSize = uint8_t;
using Offset1 = uint8_t;
using Offset2 = uint16_t;
// using Offset3 = uint24_t; // Hope I never get to use this
using Offset4 = uint32_t;

namespace font::detail::otf_font::cff {

enum EncodingID {
    Standard = 0,
    Expert = 1,
};

struct FontTableCFF {
    struct Header {
        Card8 major_version = 0;
        Card8 minor_version = 0;
        Card8 hdr_size = 0;
        OffSize off_size = 0;
    } header;

    std::string name;

    TopData top_data;
    PrivateData private_data;

    std::vector<font::Glyph> glyphs;

    static FontTableCFF read_font_table_cff(std::ifstream &stream) {
        const auto start_cff_data = stream.tellg();
        /*std::cout << "Start CFF data: 0x" << start_cff_data << std::endl;*/
        auto cff = FontTableCFF{
            .header =
                {
                    .major_version = read_uint8(stream),
                    .minor_version = read_uint8(stream),
                    .hdr_size = read_uint8(stream),
                    .off_size = read_uint8(stream),
                },
        };

        const CFFIndex name = CFFIndex::read_index(stream);
        cff.name = std::string(name[0].begin(), name[0].end());

        const auto top_index = CFFIndex::read_index(stream);

        std::cout << "Top dict index count: " << top_index.count << std::endl;

        const auto string_index = CFFIndex::read_index(stream);
        const auto global_subroutines = CFFIndex::read_index(stream);

        cff.top_data = TopData::parse(top_index[0], string_index);

        if (cff.top_data.charstring_type != 2) {
            throw std::runtime_error(
                std::format("Error: charstring type {} is not supported",
                            cff.top_data.charstring_type));
        }

        if (cff.top_data.encoding != EncodingID::Standard) {
            throw std::runtime_error("Error: Only standard encoding implemented.");
        }

        stream.seekg(start_cff_data + std::streamoff(cff.top_data.private_offset));
        DEBUG_ASSERT(stream.good(),
                     "Error: Filestream not okay after seeking to private data.");
        const auto private_bytes = read_n_bytes(stream, cff.top_data.private_size);
        cff.private_data = PrivateData::parse(private_bytes);

        stream.seekg(start_cff_data + std::streamoff(cff.top_data.private_offset +
                                                     cff.private_data.subrs));
        DEBUG_ASSERT(stream.good(),
                     "Error: Filestream not okay after seeking to local subroutines.");
        const auto local_subroutines = CFFIndex::read_index(stream);

        stream.seekg(start_cff_data + std::streamoff(cff.top_data.charstrings));
        DEBUG_ASSERT(stream.good(),
                     "Error: Filestream not okay after seeking to charstring data.");
        const auto charstring_index = CFFIndex::read_index(stream);
        /*std::cout << "Charstring Index: " << charstring_index << std::endl;*/

        stream.seekg(start_cff_data + std::streamoff(cff.top_data.charset));
        DEBUG_ASSERT(stream.good(),
                     "Error: Filestream not okay after seeking to charset data.");
        struct Charset charset =
            Charset::read(stream, charstring_index.count, string_index);

        std::vector<GlyphOutlineCollection> font_outlines = Type2Charstring::parse(
            charstring_index, global_subroutines, local_subroutines);

        // charstring_index[0] = ".notdef"
        cff.glyphs.reserve(charstring_index.count);
        for (size_t i = 0; i < charstring_index.count; i++) {
            std::vector<GlyphOutline> &glyph_outlines = font_outlines[i];

            Glyph glyph{};

            // Form the CFF specification:
            // There is one less element in the glyph name array than nGlyphs because the
            // .notdef glyph name is omitted
            if (i == 0) {
                glyph.name = ".notdef";
            } else {
                glyph.name = std::move(charset.glyph_names[i - 1]);
            }

            glyph.polygons = construct_glyph_polygons(std::move(glyph_outlines));

            cff.glyphs.push_back(std::move(glyph));
        }

        return cff;
    }

    static std::vector<font::Polygon>
    construct_glyph_polygons(std::vector<GlyphOutline> &&glyph_outlines) {
        std::vector<size_t> exterior_outlines;
        exterior_outlines.reserve(glyph_outlines.size());
        std::vector<size_t> interior_outlines;
        interior_outlines.reserve(glyph_outlines.size());
        for (size_t i = 0; i < glyph_outlines.size(); i++) {
            if (glyph_outlines[i].vertices.size() == 0) {
                continue;
            } else if (is_counter_clockwise_winding(glyph_outlines[i].vertices)) {
                exterior_outlines.push_back(i);
            } else {
                interior_outlines.push_back(i);
            }
        }

        std::vector<font::Polygon> polygons;
        polygons.reserve(exterior_outlines.size());
        for (const size_t &exterior_id : exterior_outlines) {
            Polygon polygon;
            for (const size_t &interior_id : interior_outlines) {
                // We test containment with the last vertex because it is always a point
                // on the outline, then we won't have to deal with that a control point
                // can be off the outline
                if (glyph_outlines[interior_id].vertices.size() != 0 &&
                    is_point_inside_outline(glyph_outlines[interior_id].vertices.back(),
                                            glyph_outlines[exterior_id].vertices)) {
                    polygon.interior_outlines.push_back(
                        std::move(glyph_outlines[interior_id].vertices));

                    polygon.curves.insert(polygon.curves.end(),
                                          glyph_outlines[interior_id].curves.begin(),
                                          glyph_outlines[interior_id].curves.end());
                }
            }

            polygon.exterior_outline = std::move(glyph_outlines[exterior_id].vertices);
            polygon.curves.insert(polygon.curves.end(),
                                  glyph_outlines[exterior_id].curves.begin(),
                                  glyph_outlines[exterior_id].curves.end());

            polygons.push_back(std::move(polygon));
        }

        return polygons;
    }

    static bool
    is_point_inside_outline(const std::pair<float, float> &p,
                            const std::vector<std::pair<float, float>> &outline) {
        size_t crossings = 0;
        for (size_t i = 0; i < outline.size(); i++) {
            const std::pair<float, float> &vi = outline[i];
            const std::pair<float, float> &vj = outline[(i + 1) % outline.size()];

            if (((vi.second <= p.second) && (vj.second > p.second)) ||
                ((vi.second > p.second) && (vj.second <= p.second))) {

                // Compute x-coordinate of intersection
                float x_intersect = vi.first + (p.second - vi.second) /
                                                   (vj.second - vi.second) *
                                                   (vj.first - vi.first);

                if (p.first < x_intersect) {
                    crossings++;
                }
            }
        }

        return (crossings % 2) == 1;
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableCFF {\n"
            << "\theader: {" << "\n"
            << "\t\tmajor_version: " << static_cast<int>(header.major_version) << "\n"
            << "\t\tminor_version: " << static_cast<int>(header.minor_version) << "\n"
            << "\t\thdr_size: " << static_cast<int>(header.hdr_size) << "\n"
            << "\t\toff_size: " << static_cast<int>(header.off_size) << "\n"
            << "\t}" << "\n"
            << "\tname: " << name << "\n"
            << "\ttop_data: " << top_data << "\n"
            << "\t}" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableCFF &obj) {
        return os << obj.to_string();
    }
};

} // namespace font::detail::otf_font::cff
