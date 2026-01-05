#pragma once
#include "font/detail/otf_font/cff/CFFDict.h"
#include "font/detail/otf_font/cff/CFFIndex.h"

namespace font::detail::otf_font::cff {

enum TopDictOperators {
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

    Copyright = 0x0C00,          // 12 0
    IsFixedPitch = 0x0C01,       // 12 1
    ItalicAngle = 0x0C02,        // 12 2
    UnderlinePosition = 0x0C03,  // 12 3
    UnderlineThickness = 0x0C04, // 12 4
    PaintType = 0x0C05,          // 12 5
    CharstringType = 0x0C06,     // 12 6
    FontMatrix = 0x0C07,         // 12 7
    SyntheticBase = 0x0C14,      // 12 20
    PostScript = 0x0C15,         // 12 21
    BaseFontName = 0x0C16,       // 12 22
    BaseFontBlend = 0x0C17,      // 12 23

    ROS = 0x0C1E,             // 12 30
    CIDFontVersion = 0x0C1F,  // 12 31
    CIDFontRevision = 0x0C20, // 12 32
    CIDFontType = 0x0C21,     // 12 33
    CIDCount = 0x0C22,        // 12 34
    UIDBase = 0x0C23,         // 12 35
    FDArray = 0x0C24,         // 12 36
    FDSelect = 0x0C25,        // 12 37
    FontName = 0x0C26         // 12 38
};

struct TopData {
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

    static TopData parse(const CFFIndex &top_index, const CFFIndex &string_index) {
        TopData top{};
        const std::span<uint8_t> &data = top_index[0];

        const CFFDict dict = CFFDict::parse(data.begin(), data.end());

        for (size_t i = 0; i < dict.operators.size(); i++) {

            const int operator_ = dict.operators[i];
            const std::vector<int> operands = dict.operands[i];

            switch (operator_) {
            case TopDictOperators::Version: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected version to only contain 1 value");
                top.version = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::Notice: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected notice to only contain 1 value");
                top.notice = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::FullName: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected full name to only contain 1 value");
                top.full_name = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::FamilyName: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected family name to only contain 1 value");
                top.family_name = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::Weight: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected weight to only contain 1 value");
                top.weight = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::FontBBox: {
                DEBUG_ASSERT(operands.size() == 4,
                             "Error: Font bounding box expects 4 values");
                top.font_bbox[0] = operands[0];
                top.font_bbox[1] = operands[1];
                top.font_bbox[2] = operands[2];
                top.font_bbox[3] = operands[3];
                break;
            }

            case TopDictOperators::Charset: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected charset to only contain 1 value");
                top.charset = operands[0];
                break;
            }

            case TopDictOperators::Encoding: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected encoding to only contain 1 value");
                top.encoding = operands[0];
                break;
            }

            case TopDictOperators::CharStrings: {
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected charstring to only contain 1 value");
                top.charstrings = operands[0];
                break;
            }

            case TopDictOperators::Private: {
                DEBUG_ASSERT(operands.size() == 2,
                             "Error: expected private to only contain 1 value");
                top.private_size = operands[0];
                top.private_offset = operands[1];
                break;
            }

            case TopDictOperators::Copyright:
            case TopDictOperators::IsFixedPitch:
            case TopDictOperators::ItalicAngle:
            case TopDictOperators::UnderlinePosition:
            case TopDictOperators::UnderlineThickness:
            case TopDictOperators::PaintType:
            case TopDictOperators::CharstringType:
            case TopDictOperators::FontMatrix:
            case TopDictOperators::SyntheticBase:
            case TopDictOperators::PostScript:
            case TopDictOperators::BaseFontName:
            case TopDictOperators::BaseFontBlend:
            case TopDictOperators::ROS:
            case TopDictOperators::CIDFontVersion:
            case TopDictOperators::CIDFontRevision:
            case TopDictOperators::CIDFontType:
            case TopDictOperators::CIDCount:
            case TopDictOperators::UIDBase:
            case TopDictOperators::FDArray:
            case TopDictOperators::FDSelect:
            case TopDictOperators::FontName: {
                break;
            }

            default:
                throw std::runtime_error(std::format(
                    "Operator {} is not yet implemented ", static_cast<int>(operator_)));
                break;
            }
        }

        return top;
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "TopData {\n"
            << "\tversion: " << version << "\n"
            << "\tnotice: " << notice << "\n"
            << "\tcopyright: " << copyright << "\n"
            << "\tfull_name: " << full_name << "\n"
            << "\tfamily_name: " << family_name << "\n"
            << "\tweight: " << weight << "\n"
            << "\tunderline_position: " << static_cast<int>(underline_position) << "\n"
            << "\t\tunderline_thickness: " << static_cast<int>(underline_thickness)
            << "\n"
            << "\t\tfont_bbox: " << font_bbox[0] << " " << font_bbox[1] << " "
            << font_bbox[2] << " " << font_bbox[3] << "\n"
            << "\t\tcharset: " << charset << "\n"
            << "\t\tencoding: " << encoding << "\n"
            << "\t\tcharstring_type: " << static_cast<int>(charstring_type) << "\n"
            << "\t\tcharstrings: " << charstrings << "\n"
            << "\t\tprivate_size: " << private_size << "\n"
            << "\t\tprivate_offset: " << private_offset << "\n"
            << "\t}" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const TopData &obj) {
        return os << obj.to_string();
    }
};

} // namespace font::detail::otf_font::cff
