#pragma once
#include "CFFDict.h"
#include <iostream>
#include <vector>
namespace font::detail::otf_font::cff {

enum PrivateDataOperators {
    BlueValues = 6,
    OtherBlues = 7,
    FamilyBlues = 8,
    FamilyOtherBlues = 9,
    BlueScale = 0x0C09, // 12 9
    BlueShift = 0x0C0A, // 12 10
    BlueFuzz = 0x0C0B,  // 12 11
    StdHW = 10,
    StdVW = 11,
    StemSnapH = 0x0C0C,         // 12 12
    StemSnapV = 0x0C0D,         // 12 13
    ForceBold = 0x0C0E,         // 12 14
    LanguageGroup = 0x0C11,     // 12 17
    ExpansionFactor = 0x0C12,   // 12 18
    InitialRandomSeed = 0x0C13, // 12 19
    Subrs = 19,
    DefaultWidthX = 20,
    NominalWidthX = 21,
};

struct PrivateData {

    std::vector<int> blue_values;
    std::vector<int> other_blues;
    std::vector<int> family_blues;
    std::vector<int> family_other_blues;
    float blue_scale = 0.039625;
    int blue_shift = 7;
    int blue_fuzz = 1;
    int std_hw = 0;
    int std_vw = 0;
    std::vector<int> stem_snap_h;
    std::vector<int> stem_snap_v;
    bool force_bold = false;
    int language_group = 0;
    float expansion_factor = 0.06;
    int initial_random_seed = 0;
    int subrs = -1;
    int default_width_x = 0;
    int nominal_width_x = 0;

    static PrivateData parse(const std::vector<uint8_t> &encoded_bytes) {

        PrivateData private_data{};

        const auto dict = CFFDict::parse(encoded_bytes);

        for (auto i = 0; i < dict.operators.size(); i++) {

            const int operator_ = dict.operators[i];
            const auto operands = dict.operands[i];

            /*std::cout << std::format("{}: ", operator_);*/
            /*for (int o : operands) {*/
            /*    std::cout << o << " ";*/
            /*}*/
            /*std::cout << std::endl;*/

            switch (operator_) {
            case PrivateDataOperators::BlueValues:
                private_data.blue_values = CFFDict::parse_delta(operands);
                break;

            case PrivateDataOperators::OtherBlues:
                private_data.other_blues = CFFDict::parse_delta(operands);
                break;

            case PrivateDataOperators::FamilyBlues:
                private_data.family_blues = CFFDict::parse_delta(operands);
                break;

            case PrivateDataOperators::FamilyOtherBlues:
                private_data.family_other_blues = CFFDict::parse_delta(operands);
                break;

            case PrivateDataOperators::BlueScale:
                private_data.blue_scale = CFFDict::parse_real_number(operands);
                break;

            case PrivateDataOperators::BlueShift:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected blue shift to only contain 1 value");
                private_data.blue_shift = operands[0];
                break;

            case PrivateDataOperators::BlueFuzz:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected blue fuzz to only contain 1 value");
                private_data.blue_fuzz = operands[0];
                break;

            case PrivateDataOperators::StdHW:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected std hw to only contain 1 value");
                private_data.std_hw = operands[0];
                break;

            case PrivateDataOperators::StdVW:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected std vw to only contain 1 value");
                private_data.std_vw = operands[0];
                break;

            case PrivateDataOperators::StemSnapH:
                private_data.stem_snap_h = CFFDict::parse_delta(operands);
                break;

            case PrivateDataOperators::StemSnapV:
                private_data.stem_snap_v = CFFDict::parse_delta(operands);
                break;

            case PrivateDataOperators::ForceBold:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected force bold to only contain 1 value");
                private_data.force_bold = operands[0];
                break;

            case PrivateDataOperators::LanguageGroup:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected language group to only contain 1 value");
                private_data.language_group = operands[0];
                break;

            case PrivateDataOperators::ExpansionFactor:
                private_data.expansion_factor = CFFDict::parse_real_number(operands);
                break;

            case PrivateDataOperators::InitialRandomSeed:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected subroutine offset to only contain 1 value");
                private_data.initial_random_seed = operands[0];
                break;

            case PrivateDataOperators::Subrs:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected subroutine offset to only contain 1 value");
                private_data.subrs = operands[0];
                break;

            case PrivateDataOperators::DefaultWidthX:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected default width x to only contain 1 value");
                private_data.default_width_x = operands[0];
                break;

            case PrivateDataOperators::NominalWidthX:
                DEBUG_ASSERT(operands.size() == 1,
                             "Error: expected nominal width x to only contain 1 value");
                private_data.nominal_width_x = operands[0];
                break;
            }
        }

        return private_data;
    };

    std::string to_string() const {
        std::ostringstream oss;

        auto vec_to_string = [](const std::vector<int> &vec) {
            std::ostringstream s;
            s << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                s << vec[i];
                if (i < vec.size() - 1)
                    s << ", ";
            }
            s << "]";
            return s.str();
        };

        oss << "PrivateData {\n"
            << "\tblue_values: " << vec_to_string(blue_values) << "\n"
            << "\tother_blues: " << vec_to_string(other_blues) << "\n"
            << "\tfamily_blues: " << vec_to_string(family_blues) << "\n"
            << "\tfamily_other_blues: " << vec_to_string(family_other_blues) << "\n"
            << "\tblue_scale: " << blue_scale << "\n"
            << "\tblue_shift: " << blue_shift << "\n"
            << "\tblue_fuzz: " << blue_fuzz << "\n"
            << "\tstd_hw: " << std_hw << "\n"
            << "\tstd_vw: " << std_vw << "\n"
            << "\tstem_snap_h: " << vec_to_string(stem_snap_h) << "\n"
            << "\tstem_snap_v: " << vec_to_string(stem_snap_v) << "\n"
            << "\tforce_bold: " << (force_bold ? "true" : "false") << "\n"
            << "\tlanguage_group: " << language_group << "\n"
            << "\texpansion_factor: " << expansion_factor << "\n"
            << "\tinitial_random_seed: " << initial_random_seed << "\n"
            << "\tsubrs: " << subrs << "\n"
            << "\tdefault_width_x: " << default_width_x << "\n"
            << "\tnominal_width_x: " << nominal_width_x << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const PrivateData &obj) {
        return os << obj.to_string();
    }
};

} // namespace font::detail::otf_font::cff
