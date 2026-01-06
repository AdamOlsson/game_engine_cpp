#pragma once

#include "CFFDict.h"
#include "CFFIndex.h"
#include "font/Glyph.h"

#include <ostream>
#include <stdexcept>
#include <string>

namespace font::detail::otf_font::cff {

struct OffCurvePoint {
    int x;
    int y;

    std::string to_string() const {
        std::ostringstream oss;
        oss << std::format("OffCurvePoint({},{})", x, y);
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const OffCurvePoint &obj) {
        return os << obj.to_string();
    }
};

struct OnCurvePoint {
    int x;
    int y;

    std::string to_string() const {
        std::ostringstream oss;
        oss << std::format("OnCurvePoint({},{})", x, y);
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const OnCurvePoint &obj) {
        return os << obj.to_string();
    }
};

using OutlineControlPoints = std::vector<std::variant<OffCurvePoint, OnCurvePoint>>;

struct DecodeState {
    int x = 0;
    int y = 0;
    int width = 0;
    size_t hint_count = 0;
    std::pair<int, int> contour_start = {0, 0};
    OutlineControlPoints current_outline;
    std::vector<OutlineControlPoints> outlines;
};

enum Type2Operators {
    CallSubr = 10,
    Return = 11,
    EndChar = 14,
    CallGSubr = 29,
};

enum Type2MoveToOperators {
    VMoveTo = 4,
    RMoveTo = 21,
    HMoveTo = 22,
};

enum Type2PathConstructOperators {
    RLineTo = 5,
    HLineTo = 6,
    VLineTo = 7,
    RRCurveTo = 8,
    RLineCurve = 24,
    RCurveLine = 25,
    VVCurveTo = 26,
    HHCurveTo = 27,
    VHCurveTo = 30,
    HVCurveTo = 31,
    HFlex = 0x0C22,  // 12 34 ---
    Flex = 0x0C23,   // 12 35 ---
    HFlex1 = 0x0C24, // 12 36 ---
    Flex1 = 0x0C25,  // 12 37 ---

    And = 0x0C03,    // 12 3
    Or = 0x0C04,     // 12 4
    Not = 0x0C05,    // 12 5
    Abs = 0x0C09,    // 12 9
    Add = 0x0C0A,    // 12 10
    Sub = 0x0C0B,    // 12 11
    Div = 0x0C0C,    // 12 12
    Neg = 0x0C0E,    // 12 14
    Eq = 0x0C0F,     // 12 15
    Drop = 0x0C12,   // 12 18
    Put = 0x0C14,    // 12 20
    Get = 0x0C15,    // 12 21
    IfElse = 0x0C16, // 12 22
    Random = 0x0C17, // 12 23
    Mul = 0x0C18,    // 12 24
    Sqrt = 0x0C1A,   // 12 26
    Dup = 0x0C1B,    // 12 27
    Exch = 0x0C1C,   // 12 28
    Index = 0x0C1D,  // 12 29
    Roll = 0x0C1E,   // 12 30
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
    static std::vector<GlyphOutlineCollection> parse(const CFFIndex &charstring_index,
                                                     const CFFIndex &global_subrs,
                                                     const CFFIndex &local_subrs) {

        std::vector<GlyphVertexCollection> font_outlines;
        font_outlines.reserve(charstring_index.count);
        for (auto i = 0; i < charstring_index.count; i++) {
            const auto encoded_glyph_seq = charstring_index[i];
            // TODO: What to do with width?

            DecodeState state{};
            decode_glyph(encoded_glyph_seq, global_subrs, local_subrs, state);

            GlyphOutlineCollection glyph_outlines;
            glyph_outlines.reserve(state.outlines.size());
            for (auto &outline : state.outlines) {
                glyph_outlines.push_back(parse_outline(std::move(outline)));
            }

            font_outlines.push_back(std::move(glyph_outlines));
        }

        return font_outlines;
    }

    static void decode_glyph(const std::span<uint8_t> &encoded_glyph_seq,
                             const CFFIndex &global_subrs, const CFFIndex &local_subrs,
                             DecodeState &state) {

        std::vector<std::stack<int>> operand_stacks;
        std::vector<int> operators;
        for (auto iter = encoded_glyph_seq.begin(); iter != encoded_glyph_seq.end();
             iter++) {

            std::stack<int> decoded_operands;
            decode_until_next_operator(iter, encoded_glyph_seq.end(), decoded_operands);
            const int operator_ = *iter;
            operand_stacks.push_back(std::move(decoded_operands));
            operators.push_back(operator_);

            // The following is the order of the operators:
            // w? {hs* vs* cm* hm* mt subpath}? {mt subpath}* endchar

            // Handle hint operators (hs, vs, cm, hm)
            const int &oper = operators.back();
            auto &operands = operand_stacks.back();
            switch (oper) {

            // Hints: zero or more of each of the following hint
            // operators, in exactly the following order: hstem, hstemhm,
            //  vstem, vstemhm, cntrmask, hintmask.
            case Type2HintOperators::HStem:
                handle_hstem(state, operands);
                break;

            case Type2HintOperators::HStemHM:
                handle_hstemhm(state, operands);
                break;

            case Type2HintOperators::VStem:
                handle_vstem(state, operands);
                break;

            case Type2HintOperators::VStemHM:
                handle_vstemhm(state, operands);
                break;

            case Type2HintOperators::HintMask: {
                if (operands.size() > 0) {
                    // This case is an optimization in OTF files and these values are to
                    // be treated as vstem
                    handle_vstem(state, operands);
                }

                const size_t num_hint_bytes = (state.hint_count + 7) / 8;
                /*std::cout << "hintmask: 0x";*/
                std::vector<uint8_t> hint_mask_bytes;
                for (size_t i = 0; i < num_hint_bytes; i++) {
                    hint_mask_bytes.push_back(*(++iter));
                    /*std::cout << std::hex << static_cast<int>(hint_mask_bytes.back())*/
                    /*          << std::dec;*/
                }
                /*std::cout << std::endl;*/
                break;
            }

            case Type2HintOperators::CntrMask: {
                const size_t num_hint_bytes = (state.hint_count + 7) / 8;
                /*std::cout << "cntrmask: 0x";*/
                std::vector<uint8_t> cntr_mask_bytes;
                for (size_t i = 0; i < num_hint_bytes; i++) {
                    cntr_mask_bytes.push_back(*(++iter));
                    /*std::cout << std::hex << static_cast<int>(cntr_mask_bytes.back())*/
                    /*          << std::dec;*/
                }
                /*std::cout << std::endl;*/
                break;
            }

            case Type2MoveToOperators::RMoveTo:
                handle_rmoveto(state, operands);
                break;

            case Type2MoveToOperators::HMoveTo:
                handle_hmoveto(state, operands);
                break;

            case Type2MoveToOperators::VMoveTo:
                handle_vmoveto(state, operands);
                break;

            case Type2PathConstructOperators::RLineTo:
                handle_rlineto(state, operands);
                break;

            case Type2PathConstructOperators::HLineTo:
                handle_hlineto(state, operands);
                break;

            case Type2PathConstructOperators::VLineTo:
                handle_vlineto(state, operands);
                break;

            case Type2PathConstructOperators::RCurveLine:
                handle_rcurveline(state, operands);
                break;

            case Type2PathConstructOperators::VHCurveTo:
                handle_vhcurveto(state, operands);
                break;

            case Type2PathConstructOperators::HVCurveTo:
                handle_hvcurveto(state, operands);
                break;

            case Type2PathConstructOperators::RRCurveTo:
                handle_rrcurveto(state, operands);
                break;

            case Type2PathConstructOperators::VVCurveTo:
                handle_vvcurveto(state, operands);
                break;

            case Type2PathConstructOperators::HHCurveTo:
                handle_hhcurveto(state, operands);
                break;

            case Type2Operators::EndChar:
                handle_endchar(state, operands);
                break;

            case Type2Operators::CallSubr:
                handle_callsubr(state, operands, global_subrs, local_subrs);
                break;

            case Type2Operators::CallGSubr: {
                handle_callgsubr(state, operands, global_subrs, local_subrs);
                break;
            }

            case Type2Operators::Return: {
                DEBUG_ASSERT(
                    iter + 1 == encoded_glyph_seq.end(),
                    "Error: return operator found when there are operators following.");

                break;
            }

            case Type2PathConstructOperators::RLineCurve: {
                handle_rlinecurve(state, operands);
                break;
            }

            case Type2PathConstructOperators::Flex: {
                handle_flex(state, operands);
                break;
            }

            case Type2PathConstructOperators::HFlex: {
                handle_hflex(state, operands);
                break;
            }
            case Type2PathConstructOperators::HFlex1: {
                handle_hflex1(state, operands);
                break;
            }
            case Type2PathConstructOperators::Flex1: {
                handle_flex1(state, operands);
                break;
            }
            case Type2PathConstructOperators::And:
            case Type2PathConstructOperators::Or:
            case Type2PathConstructOperators::Not:
            case Type2PathConstructOperators::Abs:
            case Type2PathConstructOperators::Add:
            case Type2PathConstructOperators::Sub:
            case Type2PathConstructOperators::Div:
            case Type2PathConstructOperators::Neg:
            case Type2PathConstructOperators::Eq:
            case Type2PathConstructOperators::Drop:
            case Type2PathConstructOperators::Put:
            case Type2PathConstructOperators::Get:
            case Type2PathConstructOperators::IfElse:
            case Type2PathConstructOperators::Random:
            case Type2PathConstructOperators::Mul:
            case Type2PathConstructOperators::Sqrt:
            case Type2PathConstructOperators::Dup:
            case Type2PathConstructOperators::Exch:
            case Type2PathConstructOperators::Index:
            case Type2PathConstructOperators::Roll: {
                DEBUG_ASSERT(
                    false,
                    std::format("Error: Arithmetic operator {} not yet implemented.",
                                oper));
            }

            default: {
                DEBUG_ASSERT(
                    false, std::format("Error: operator {} not yet implemented.", oper));
                break;
            }
            }
        }

        // Check if the sequence started with a width
        if (operand_stacks[0].size() > 0) {
            state.width = operand_stacks[0].top();
            operand_stacks[0].pop();
        }

        DEBUG_ASSERT(operand_stacks[0].size() == 0,
                     "Error: glyph sequence still has an unprocessed width value.");
        DEBUG_CODE(for (auto i = 1; i < operand_stacks.size(); i++) {
            DEBUG_ASSERT(operand_stacks[i].size() == 0,
                         std::format("Error: glyph sequence {} (operand {}) still has an "
                                     "unprocessed operand.",
                                     i, operators[i]));
        });

        return;
    }

    template <std::input_iterator Iter>
    static void decode_until_next_operator(Iter &data, const Iter &end,
                                           std::stack<int> &decoded) {
        while (data != end) {
            const auto b0 = *data;
            if (32 <= b0 && b0 <= 246) {
                decoded.push(CFFDict::decode1(b0));
                data++;
                // std::cout << std::format("decode1 - b0: {} = {} ",
                // static_cast<int>(b0),
                //                          decoded.top())
                //           << std::endl;
                continue;

            } else if (247 <= b0 && b0 <= 250) {
                data++;
                const auto b1 = *data;
                data++;
                decoded.push(CFFDict::decode2(b0, b1));
                // std::cout << std::format("decode2 - b0: {}, b1 = {} = {} ",
                //                          static_cast<int>(b0), static_cast<int>(b1),
                //                          decoded.top())
                //           << std::endl;
                continue;

            } else if (251 <= b0 && b0 <= 254) {
                data++;
                const auto b1 = *data;
                data++;
                decoded.push(CFFDict::decode3(b0, b1));
                // std::cout << std::format("decode3 - b0: {}, b1 = {} = {} ",
                //                          static_cast<int>(b0), static_cast<int>(b1),
                //                          decoded.top())
                //           << std::endl;
                continue;

            } else if (b0 == 255) {
                throw std::runtime_error("Operand 255 not yet implemented, see Type "
                                         "2 charstring documtation "
                                         "section 'Charstring Number Encoding'");
            }
            break;
        }
    }

    static size_t subroutine_index_correction(const int index,
                                              const int num_subroutines) {
        if (num_subroutines < 1240) {
            return index + 107;
        } else if (num_subroutines < 33900) {
            return index + 1131;
        }
        return index + 32768;
    }

  private:
    // clang-format off
    static void handle_hstem(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_hstemhm(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_vstem(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_vstemhm(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);

    static void handle_hmoveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_rmoveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_vmoveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);

    static void handle_rlineto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_vlineto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_hlineto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);

    static void handle_rrcurveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_vvcurveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_hhcurveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);

    static void handle_rcurveline(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_vhcurveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_hvcurveto(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    
    static void handle_rlinecurve(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);

    static void handle_flex(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_hflex(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_hflex1(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    static void handle_flex1(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);

    static void handle_endchar(font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands);
    // clang-format on

    static void handle_callsubr(font::detail::otf_font::cff::DecodeState &state,
                                std::stack<int> &operands, const CFFIndex &global_subrs,
                                const CFFIndex &local_subrs);
    static void handle_callgsubr(font::detail::otf_font::cff::DecodeState &state,
                                 std::stack<int> &operands, const CFFIndex &global_subrs,
                                 const CFFIndex &local_subrs);

    static void append_bezier(font::detail::otf_font::cff::DecodeState &state,
                              const int x1, const int y1, const int x2, const int y2,
                              const int x3, const int y3);

    static constexpr bool
    is_off_curve_point(const std::variant<OffCurvePoint, OnCurvePoint> &p);
    static font::GlyphOutline parse_outline(const OutlineControlPoints &control_points);
    static constexpr std::pair<int, int>
    decay_to_point(const std::variant<OffCurvePoint, OnCurvePoint> &p);
};

}; // namespace font::detail::otf_font::cff
