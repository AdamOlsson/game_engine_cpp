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

        stream.seekg(start_cff_data + std::streamoff(cff.header.hdr_size));
        const CFFIndex name = CFFIndex::read_index(stream);
        cff.name = std::string(name[0].begin(), name[0].end());

        const CFFIndex top_index = CFFIndex::read_index(stream);
        const CFFIndex string_index = CFFIndex::read_index(stream);
        const CFFIndex global_subroutines = CFFIndex::read_index(stream);
        // Encoded Top Data: 248 30 0 248 27 2 248 28 3 248 29 4 139 12 1 139 12 2 251 0
        // 12 3 192 12 4 254 94 251 128 251 162 250 87 5 29 0 0 2 248 15 29 0 0 0 3 29 0 0
        // 63 117 18 29 0 0 4 163 17
        // Leads:
        // - Why does looking up the full_name in the string index give "Summary
        // Unavailable"?
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
        const std::vector<uint8_t> private_bytes =
            read_n_bytes(stream, cff.top_data.private_size);
        cff.private_data = PrivateData::parse(private_bytes);

        CFFIndex local_subroutines = CFFIndex::read_index(stream);
        if (cff.private_data.subrs > 0) {
            stream.seekg(start_cff_data + std::streamoff(cff.top_data.private_offset +
                                                         cff.private_data.subrs));
            DEBUG_ASSERT(
                stream.good(),
                "Error: Filestream not okay after seeking to local subroutines.");

            local_subroutines = CFFIndex::read_index(stream);
        } else {
            local_subroutines = CFFIndex{};
        }

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

            glyph.polygons = Glyph::construct_polygons(std::move(glyph_outlines));

            cff.glyphs.push_back(std::move(glyph));
        }

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
