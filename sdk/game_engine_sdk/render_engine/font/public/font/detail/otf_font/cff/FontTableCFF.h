#pragma once

#include "CFFDict.h"
#include "CFFIndex.h"
#include "CFFStandardStrings.h"
#include "Type2Charstring.h"
#include "font/detail/ifstream_util.h"
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

enum TopDictValue {
    Version = 0,
    Notice = 1,
    FullName = 2,
    FamilyName = 3,
    Weight = 4,
    FontBBox = 5,
    Charset = 15,
    Encoding = 16,
    CharStrings = 17,
    Private = 18,

    UnderlinePosition = 3075, // 12 3

    // TODO: More here
};

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

    struct Top {
        std::string version = "";
        std::string notice = "";
        std::string copyright = "";
        std::string full_name = "";
        std::string family_name = "";
        std::string weight = "";
        int32_t underline_position = 0;
        int32_t underline_thickness = 0;
        std::array<int, 4> font_bbox{0};
        uint8_t charstring_type = 2;
        int32_t charset = 0;
        int32_t encoding = 0;
        int32_t charstrings = 0;
        int32_t private_size = 0;
        int32_t private_offset = 0;
    } top;

    CFFIndex global_subroutines;

    std::vector<Glyph> glyphs;

    static FontTableCFF read_font_table_cff(std::ifstream &stream) {
        const auto start_cff_data = stream.tellg();
        std::cout << "Start CFF data: 0x" << start_cff_data << std::endl;
        auto cff = FontTableCFF{
            .header =
                {
                    .major_version = read_uint8(stream),
                    .minor_version = read_uint8(stream),
                    .hdr_size = read_uint8(stream),
                    .off_size = read_uint8(stream),
                },
        };

        const auto name = CFFIndex::read_index(stream);
        cff.name = std::string(name[0].begin(), name[0].end());

        const auto top_index = CFFIndex::read_index(stream);
        const auto string_index = CFFIndex::read_index(stream);
        cff.global_subroutines = CFFIndex::read_index(stream);

        cff.top = parse_top_data(top_index, string_index);

        if (cff.top.encoding != EncodingID::Standard) {
            throw std::runtime_error("Error: Only standard encoding implemented");
        }

        stream.seekg(start_cff_data + std::streamoff(cff.top.charstrings));
        DEBUG_ASSERT(stream.good(),
                     "Error: Filestream not okay after seeking to charstring data.");
        const auto charstrings_index = CFFIndex::read_index(stream);
        /*std::cout << "Charstrings Index: " << charstrings_index << std::endl;*/

        stream.seekg(start_cff_data + std::streamoff(cff.top.charset));
        DEBUG_ASSERT(stream.good(),
                     "Error: Filestream not okay after seeking to charset data.");
        const auto charset =
            read_charsets_data(stream, charstrings_index.count, string_index);

        cff.glyphs = Type2Charstring::parse(charstrings_index, charset);

        auto g = cff.glyphs[0];
        std::cout << std::format("Glyph '{}': ", g.name);
        for (auto i : cff.glyphs[0].points) {
            std::cout << std::format("({},{}) ", i.first, i.second);
        }
        std::cout << std::endl;

        return cff;
    }

    static Top parse_top_data(const auto &top_index, const auto &string_index) {
        auto top = Top{};
        const auto data = top_index[0];

        std::stack<int> values;
        for (auto data_iter = data.begin(); data_iter != data.end(); data_iter++) {
            DEBUG_ASSERT(values.size() == 0, "Error: Expected stack to be empty.");

            bool is_operator = false;
            while (!is_operator) {
                values.push(CFFDict::decode_one_value(data_iter));
                const auto next = *(++data_iter);
                is_operator = 0 <= next && next <= 21;
            }

            // Note: "operator" is reserved keyword, therefore the underscore
            const int operator_ = *data_iter;
            if (operator_ == 12) {
                const int operator_2 = *(++data_iter);
                const int combined_operator = operator_ << 8 | operator_2;

                switch (combined_operator) {
                case TopDictValue::UnderlinePosition: {
                    top.underline_position = values.top();
                    values.pop();
                    break;
                }
                default: {
                    throw std::runtime_error(std::format(
                        "Operator {} {} is not yet implemented",
                        static_cast<int>(operator_), static_cast<int>(operator_2)));
                    break;
                }
                };
                continue;
            }

            switch (operator_) {
            case TopDictValue::Version: {
                top.version = lookup_string(string_index, values.top());
                values.pop();
                break;
            }
            case TopDictValue::Notice: {
                top.notice = lookup_string(string_index, values.top());
                values.pop();
                break;
            }
            case TopDictValue::FullName: {
                top.full_name = lookup_string(string_index, values.top());
                values.pop();
                break;
            }
            case TopDictValue::FamilyName: {
                top.family_name = lookup_string(string_index, values.top());
                values.pop();
                break;
            }
            case TopDictValue::Weight: {
                top.weight = lookup_string(string_index, values.top());
                values.pop();
                break;
            }
            case TopDictValue::FontBBox: {
                DEBUG_ASSERT(values.size() == 4,
                             "Error: Font bounding box expects 4 values");
                top.font_bbox[3] = values.top();
                values.pop();
                top.font_bbox[2] = values.top();
                values.pop();
                top.font_bbox[1] = values.top();
                values.pop();
                top.font_bbox[0] = values.top();
                values.pop();
                break;
            }
            case TopDictValue::Charset: {
                top.charset = values.top();
                values.pop();
                break;
            }
            case TopDictValue::Encoding: {
                top.encoding = values.top();
                values.pop();
                break;
            }
            case TopDictValue::CharStrings: {
                top.charstrings = values.top();
                values.pop();
                break;
            }
            case TopDictValue::Private: {
                top.private_offset = values.top();
                values.pop();
                top.private_size = values.top();
                values.pop();
                break;
            }
            default:
                throw std::runtime_error(std::format("Operator {} is not yet implemented",
                                                     static_cast<int>(operator_)));
                values.pop();
                break;
            }
        }

        return top;
    }

    static std::vector<std::string> read_charsets_data(std::ifstream &stream,
                                                       const uint16_t &num_glyphs,
                                                       const CFFIndex &string_index) {

        const int charset_format = read_uint8(stream);
        DEBUG_ASSERT(charset_format == 0,
                     "Error: Only format charset format 0 is implemented");
        std::vector<std::string> glyph_names{};
        glyph_names.reserve(num_glyphs);
        for (auto i = 0; i < num_glyphs; i++) {
            int sid = read_uint16(stream);
            glyph_names.emplace_back(lookup_string(string_index, sid));
        }
        return glyph_names;
    }

    static std::string lookup_string(const CFFIndex &string_index, const int idx) {
        if (idx <= N_STD_STRING) {
            return CFF_STANDARD_STRINGS[idx];
        }
        const std::span<uint8_t> str = string_index[idx - N_STD_STRING];
        return std::string(str.begin(), str.end());
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
            << "\ttop: {" << "\n"
            << "\t\tversion: " << top.version << "\n"
            << "\t\tnotice: " << top.notice << "\n"
            << "\t\tcopyright: " << top.copyright << "\n"
            << "\t\tfull_name: " << top.full_name << "\n"
            << "\t\tfamily_name: " << top.family_name << "\n"
            << "\t\tweight: " << top.weight << "\n"
            << "\t\tunderline_position: " << static_cast<int>(top.underline_position)
            << "\n"
            << "\t\tunderline_thickness: " << static_cast<int>(top.underline_thickness)
            << "\n"
            << "\t\tfont_bbox: " << top.font_bbox[0] << " " << top.font_bbox[1] << " "
            << top.font_bbox[2] << " " << top.font_bbox[3] << "\n"
            << "\t\tcharset: " << top.charset << "\n"
            << "\t\tencoding: " << top.encoding << "\n"
            << "\t\tcharstring_type: " << static_cast<int>(top.charstring_type) << "\n"
            << "\t\tcharstrings: " << top.charstrings << "\n"
            << "\t\tprivate_size: " << top.private_size << "\n"
            << "\t\tprivate_offset: " << top.private_offset << "\n"
            << "\t}" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableCFF &obj) {
        return os << obj.to_string();
    }
};

} // namespace font::detail::otf_font::cff
