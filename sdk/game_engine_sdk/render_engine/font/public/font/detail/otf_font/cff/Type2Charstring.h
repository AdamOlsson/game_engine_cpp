#pragma once

#include "CFFDict.h"
#include "CFFIndex.h"

#include <ostream>
#include <string>

namespace font::detail::otf_font::cff {

struct Glyph {
    std::string name;
    std::vector<std::pair<int, int>> points;
};

enum Type2Operators {
    CallSubr = 10,
    Return = 11,
    EndChar = 14,
    CallGSubr = 29,
};

enum Type2PathConstructOperators {
    VMoveTo = 4,
    RLineTo = 5,
    HLineTo = 6,
    VLineTo = 7,
    RRCurveTo = 8,
    RMoveTo = 21,
    HMoveTo = 22,
    RCurveLine = 25,
    VVCurveTo = 26,
    HHCurveTo = 27,
    VHCurveTo = 30,
    HVCurveTo = 31,
    // Flex = ... // 12 35
    // HFlex = ... // 12 34
    // HFlex1 = ... // 12 36
    // Flex1 = ... // 12 37
    // TODO: And more...
};

enum Type2HintOperators {
    HStem = 1,
    VStem = 3,
    HStemHM = 18,
    HintMask = 19,
    CntrMask = 20,
    VStemHM = 23,
};

struct Type2Charstring {
    static std::vector<Glyph> parse(const CFFIndex &charstring_index,
                                    const std::vector<std::string> &charset) {

        const auto num_glyphs = charstring_index.count;
        auto char_z = 58;
        /*for (auto i = 0; i < num_glyphs; i++) {*/
        /*for (auto i = 0; i < 59; i++) {*/
        auto i = char_z;
        const auto glyph_name = charset[i];
        const auto encoded_glyph_seq = charstring_index[i];

        /*std::cout << glyph_name << ": ";*/
        /*for (auto i : encoded_glyph_seq) {*/
        /*    std::cout << std::dec << static_cast<int>(i) << " ";*/
        /*}*/
        /*std::cout << "\n";*/

        auto points = decode_glyph(encoded_glyph_seq);

        return {Glyph{.name = std::move(glyph_name), .points = std::move(points)}};
    }

    static std::vector<std::pair<int, int>>
    decode_glyph(const std::span<uint8_t> &encoded_glyph_seq) {

        std::vector<std::stack<int>> value_stacks;
        std::vector<int> operators;
        for (auto iter = encoded_glyph_seq.begin(); iter != encoded_glyph_seq.end();
             iter++) {
            std::stack<int> decoded_values;
            decode_until_next_operator(iter, encoded_glyph_seq.end(), decoded_values);
            const int operator_ = *iter;
            value_stacks.push_back(std::move(decoded_values));
            operators.push_back(operator_);
        }

        std::vector<std::pair<int, int>> points;
        int x = 0;
        int y = 0;
        const auto num_operators = operators.size();
        for (auto i = 0; i < num_operators; i++) {
            const int oper = operators[i];
            auto &values = value_stacks[i];
            switch (oper) {
                // TODO: What do HSteam hint mean?
            case Type2HintOperators::HStem: {
                const auto num_pairs = (values.size() - 2) / 2;
                std::vector<std::pair<int, int>> dys{};
                dys.reserve(num_pairs);
                for (auto i = 0; i < num_pairs; i++) {
                    const int dyb = values.top();
                    values.pop();
                    const int dya = values.top();
                    values.pop();
                    dys.emplace_back(dya, dyb);
                }
                const int dy = values.top();
                values.pop();
                const int y = values.top();
                values.pop();

                // This is all for print
                std::string str;
                for (size_t i = 0; i < dys.size(); ++i) {
                    str += std::format("{{{} {}}},", dys[i].first, dys[i].second);
                }
                std::cout << std::format("{} {} {{{}}} hstem", y, dy, str) << std::endl;
                break;
            }

            case Type2PathConstructOperators::RMoveTo: {
                const int dy1 = values.top();
                values.pop();
                const int dx1 = values.top();
                values.pop();
                x += dx1;
                y += dy1;
                points.emplace_back(x, y);
                std::cout << std::format("{} {} rmoveto", dx1, dy1) << std::endl;
                break;
            }

            case Type2PathConstructOperators::HMoveTo: {
                const int dx1 = values.top();
                values.pop();
                std::cout << std::format("{} hmoveto", dx1) << std::endl;
                break;
            }

            case Type2PathConstructOperators::VMoveTo: {
                const int dy1 = values.top();
                values.pop();
                std::cout << std::format("{} vmoveto", dy1) << std::endl;
                break;
            }

            case Type2PathConstructOperators::RLineTo: {
                const auto num_pairs = values.size() / 2;
                std::vector<std::pair<int, int>> ds{};
                ds.reserve(num_pairs);
                for (auto i = 0; i < num_pairs; i++) {
                    const int dya = values.top();
                    values.pop();
                    const int dxa = values.top();
                    values.pop();
                    ds.emplace_back(dxa, dya);
                    x += dxa;
                    y += dya;
                    points.emplace_back(x, y);
                }
                // This is all for print
                std::string str;
                for (size_t i = 0; i < ds.size(); ++i) {
                    str += std::format("{{{} {}}},", ds[i].first, ds[i].second);
                }
                std::cout << std::format("{{{}}} rlineto", str) << std::endl;
                break;
            }

            case Type2Operators::EndChar: {
                std::cout << "endchar" << std::endl;
                break;
            }

            default: {
                std::cout << std::format("Next operator: {}", static_cast<int>(oper))
                          << std::endl;
                break;
            }
            }
        }

        // Check if the sequence started with a width
        int glyph_width = 0;
        if (value_stacks[0].size() > 0) {
            glyph_width = value_stacks[0].top();
            value_stacks[0].pop();
        }

        DEBUG_ASSERT(value_stacks[0].size() == 0,
                     "Error: glyph sequence still has an unprocessed width value");
        DEBUG_CODE(for (auto i = 1; i < value_stacks.size(); i++) {
            DEBUG_ASSERT(value_stacks[i].size() == 0,
                         "Error: glyph sequence still has an unprocessed value");
        });

        points.shrink_to_fit();
        return points;
    }

    template <std::input_iterator Iter>
    static void decode_until_next_operator(Iter &data, const Iter &end,
                                           std::stack<int> &decoded) {
        while (data != end) {
            const auto b0 = *data;
            if (32 <= b0 && b0 <= 246) {
                decoded.push(CFFDict::decode1(b0));
                data++;
                /*std::cout << std::format("decode1 - b0: {} = {} ",
                 * static_cast<int>(b0),*/
                /*                         decoded.top())*/
                /*          << std::endl;*/
                continue;
            } else if (247 <= b0 && b0 <= 250) {
                data++;
                const auto b1 = *data;
                data++;
                decoded.push(CFFDict::decode2(b0, b1));
                /*std::cout << std::format("decode2 - b0: {}, b1 = {} = {} ",*/
                /*                         static_cast<int>(b0),
                 * static_cast<int>(b1),*/
                /*                         decoded.top())*/
                /*<< std::endl;*/
                continue;
            } else if (251 <= b0 && b0 <= 254) {
                data++;
                const auto b1 = *data;
                data++;
                decoded.push(CFFDict::decode3(b0, b1));
                /*std::cout << std::format("decode3 - b0: {}, b1 = {} = {} ",*/
                /*                         static_cast<int>(b0),
                 * static_cast<int>(b1),*/
                /*                         decoded.top());*/
                continue;
            } else if (b0 == 255) {
                throw std::runtime_error("Operand 255 not yet implemented, see Type "
                                         "2 charstring documtation "
                                         "section 'Charstring Number Encoding'");
            }
            break;
        }
    }
};

/*inline std::string to_string(Type2Operators op) {*/
/*    switch (op) {*/
/*    case HStem:*/
/*        return "HStem";*/
/*    case VStem:*/
/*        return "VStem";*/
/*    case VMoveTo:*/
/*        return "VMoveTo";*/
/*    case RLineTo:*/
/*        return "RLineTo";*/
/*    case HLineTo:*/
/*        return "HLineTo";*/
/*    case VLineTo:*/
/*        return "VLineTo";*/
/*    case RRCurveTo:*/
/*        return "RRCurveTo";*/
/*    case CallSubr:*/
/*        return "CallSubr";*/
/*    case Return:*/
/*        return "Return";*/
/*    case EndChar:*/
/*        return "EndChar";*/
/*    case HStemHM:*/
/*        return "HStemHM";*/
/*    case HintMask:*/
/*        return "HintMask";*/
/*    case CntrMask:*/
/*        return "CntrMask";*/
/*    case RMoveTo:*/
/*        return "RMoveTo";*/
/*    case HMoveTo:*/
/*        return "HMoveTo";*/
/*    case VStemHM:*/
/*        return "VStemHM";*/
/*    case RCurveLine:*/
/*        return "RCurveLine";*/
/*    case VVCurveTo:*/
/*        return "VVCurveTo";*/
/*    case HHCurveTo:*/
/*        return "HHCurveTo";*/
/*    case CallGSubr:*/
/*        return "CallGSubr";*/
/*    case VHCurveTo:*/
/*        return "VHCurveTo";*/
/*    case HVCurveTo:*/
/*        return "HVCurveTo";*/
/*    default:*/
/*        return "Unknown(" + std::to_string(static_cast<int>(op)) + ")";*/
/*    }*/
/*}*/
/**/
/*std::ostream &operator<<(std::ostream &os, Type2Operators op) {*/
/*    return os << to_string(op);*/
/*}*/
}; // namespace font::detail::otf_font::cff
