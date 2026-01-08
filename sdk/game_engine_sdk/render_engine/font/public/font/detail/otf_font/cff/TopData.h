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
    UniqueId = 13,
    XUID = 14,
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
    bool is_fixed_pitch = false;
    int32_t italic_angle = 0;
    int32_t underline_position = 0;
    int32_t underline_thickness = 0;
    int32_t paint_type = 0;
    uint8_t charstring_type = 2;
    std::array<float, 6> font_matrix = {0.001f, 0.0f, 0.0f, 0.001f, 0.0f, 0.0f};
    int32_t unique_id = 0;
    std::array<int, 4> font_bbox{0};
    int32_t stroke_width = 0;
    std::array<int, 0> xuid{};
    int32_t charset = 0;
    int32_t encoding = 0;
    int32_t charstrings = 0;
    int32_t private_size = 0;
    int32_t private_offset = 0;
    int32_t synthetic_base = 0;
    std::string post_script = "";
    std::string base_font_name = "";
    std::vector<int> base_font_blend;

    static TopData parse(const std::span<uint8_t> &data, const CFFIndex &string_index) {
        TopData top{};

        /*std::cout << "Encoded Top Data: ";*/
        /*for (int i : data) {*/
        /*    std::cout << i << " ";*/
        /*}*/
        /*std::cout << std::endl;*/

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
                             "Error: expected private to only contain 2 values");
                top.private_size = operands[0];
                top.private_offset = operands[1];
                break;
            }

            case TopDictOperators::Copyright: {
                top.copyright = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::UniqueId: {
                if (operands.size() > 1) {
                    top.unique_id = CFFDict::parse_real_number(operands);
                } else {
                    top.unique_id = operands[0];
                }
                break;
            }

            case TopDictOperators::CharstringType: {
                if (operands.size() != 1) {
                    throw std::runtime_error(
                        std::format("Error: Expected TopData charstring type to only "
                                    "contain 1 value, but found {} values",
                                    operands.size()));
                }

                if (operands[0] != 2) {
                    throw std::runtime_error(std::format(
                        "Error: Charstring type {} not implemented.", operands[0]));
                }
                top.charstring_type = operands[0];
                break;
            }

            case TopDictOperators::IsFixedPitch: {
                DEBUG_ASSERT(
                    operands.size() == 1,
                    std::format(
                        "Error: is_fixed_pitch expected only one value, but found {}",
                        operands.size()));
                top.is_fixed_pitch = operands[0];
                break;
            }

            case TopDictOperators::ItalicAngle: {
                if (operands.size() > 1) {
                    top.italic_angle = CFFDict::parse_real_number(operands);
                } else {
                    top.italic_angle = operands[0];
                }
                break;
            }

            case TopDictOperators::UnderlinePosition: {
                if (operands.size() > 1) {
                    top.underline_position = CFFDict::parse_real_number(operands);
                } else {
                    top.underline_position = operands[0];
                }
                break;
            }

            case TopDictOperators::UnderlineThickness: {
                if (operands.size() > 1) {
                    top.underline_thickness = CFFDict::parse_real_number(operands);
                } else {
                    top.underline_thickness = operands[0];
                }
                break;
            }

            case TopDictOperators::PaintType: {
                if (operands.size() > 1) {
                    top.paint_type = CFFDict::parse_real_number(operands);
                } else {
                    top.paint_type = operands[0];
                }
                break;
            }

            case TopDictOperators::FontMatrix: {
                DEBUG_ASSERT(operands.size() == 4,
                             std::format("Error: Expected font matrix to be an array of "
                                         "4 values, but found {} values",
                                         operands.size()));
                top.font_matrix[0] = operands[0];
                top.font_matrix[1] = operands[1];
                top.font_matrix[2] = operands[2];
                top.font_matrix[3] = operands[3];
                break;
            }

            case TopDictOperators::SyntheticBase: {
                if (operands.size() > 1) {
                    top.synthetic_base = CFFDict::parse_real_number(operands);
                } else {
                    top.synthetic_base = operands[0];
                }
                break;
            }

            case TopDictOperators::PostScript: {
                top.post_script = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::BaseFontName: {
                top.base_font_name = CFFIndex::lookup_string(string_index, operands[0]);
                break;
            }

            case TopDictOperators::BaseFontBlend: {
                top.base_font_blend = CFFDict::parse_delta(operands);
                break;
            }

            case TopDictOperators::UIDBase:
            case TopDictOperators::FontName:
            case TopDictOperators::FDArray:
            case TopDictOperators::FDSelect:
            case TopDictOperators::CIDFontVersion:
            case TopDictOperators::CIDFontRevision:
            case TopDictOperators::CIDFontType:
            case TopDictOperators::CIDCount:
            case TopDictOperators::ROS: {
                throw std::runtime_error("Error: CID fonts are not supported.");
            }

            default:
                throw std::runtime_error(
                    std::format("Top dict operator {} is not yet implemented ",
                                static_cast<int>(operator_)));
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
