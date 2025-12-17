#pragma once

#include "CFFDict.h"
#include "CFFIndex.h"

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

using Outline = std::vector<std::pair<int, int>>;
using GlyphOutlines = std::vector<Outline>;
using OutlineControlPoints = std::vector<std::variant<OffCurvePoint, OnCurvePoint>>;

struct DecodeState {
    bool path_open = false;
    int x = 0;
    int y = 0;
    int width = 0;
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
    static std::vector<Outline> parse(const CFFIndex &charstring_index,
                                      const CFFIndex &global_subrs,
                                      const CFFIndex &local_subrs) {

        std::vector<GlyphOutlines> font_outlines;
        font_outlines.reserve(charstring_index.count);
        for (auto i = 0; i < charstring_index.count; i++) {
            std::cout << "Decoding glyph ID: " << i << std::endl;
            const auto encoded_glyph_seq = charstring_index[i];
            // TODO: What to do with width?
            DecodeState state{};
            decode_glyph(encoded_glyph_seq, global_subrs, local_subrs, state);

            GlyphOutlines glyph_outlines;
            glyph_outlines.reserve(state.outlines.size());
            for (auto &bezier_curve : state.outlines) {
                glyph_outlines.push_back(parse_bezier_curves(std::move(bezier_curve)));
            }
            font_outlines.push_back(std::move(glyph_outlines));
        }

        // 48 = O
        // 54 = U
        // 74 = i
        // 77 = l
        return font_outlines[54];
    }

    static Outline parse_bezier_curves(const OutlineControlPoints bezier_curve) {
        Outline outline;
        outline.reserve(bezier_curve.size());
        for (const auto &control_point : bezier_curve) {
            outline.emplace_back(std::visit(
                // TODO: Handle bezier curves
                [](const auto &p) -> std::pair<int, int> {
                    using T = std::decay_t<decltype(p)>;
                    if constexpr (std::is_same_v<T, OnCurvePoint>) {
                        return std::make_pair(p.x, p.y);
                    } else if constexpr (std::is_same_v<T, OffCurvePoint>) {
                        return std::make_pair(p.x, p.y);
                    }
                    throw std::runtime_error("Error: Unkown curve point type");
                },
                control_point));
        }
        return outline;
    }

    static void decode_glyph(const std::span<uint8_t> &encoded_glyph_seq,
                             const CFFIndex &global_subrs, const CFFIndex &local_subrs,
                             DecodeState &state) {

        std::vector<std::stack<int>> value_stacks;
        std::vector<int> operators;
        for (auto iter = encoded_glyph_seq.begin(); iter != encoded_glyph_seq.end();
             iter++) {

            auto iter_copy = iter;

            std::stack<int> decoded_values;
            decode_until_next_operator(iter, encoded_glyph_seq.end(), decoded_values);
            const int operator_ = *iter;
            value_stacks.push_back(std::move(decoded_values));
            operators.push_back(operator_);

            std::cout << "Encoded sequence: ";
            while (iter_copy != iter) {
                std::cout << static_cast<int>(*iter_copy) << " ";
                iter_copy++;
            }
            std::cout << operator_ << std::endl;
        }

        // The following is the order of the operators:
        // w? {hs* vs* cm* hm* mt subpath}? {mt subpath}* endchar

        // Handle hint operators (hs, vs, cm, hm)
        for (auto i = 0; i < operators.size(); i++) {
            const int oper = operators[i];
            auto &values = value_stacks[i];
            switch (oper) {

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
                    str += std::format("({},{}) ", dys[i].first, dys[i].second);
                }
                std::cout << std::format("hstem: {} {} [{}]", y, dy, str) << std::endl;
                break;
            }

            case Type2HintOperators::HStemHM: {
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
                    str += std::format("({},{}) ", dys[i].first, dys[i].second);
                }
                std::cout << std::format("hstemhm: {} {} [{}]", y, dy, str) << std::endl;
                break;
            }

            case Type2HintOperators::VStem: {
                const auto num_pairs = (values.size() - 2) / 2;
                std::vector<std::pair<int, int>> dxs{};
                dxs.reserve(num_pairs);
                for (auto i = 0; i < num_pairs; i++) {
                    const int dxb = values.top();
                    values.pop();
                    const int dxa = values.top();
                    values.pop();
                    dxs.emplace_back(dxa, dxb);
                }
                const int dx = values.top();
                values.pop();
                const int x = values.top();
                values.pop();

                // This is all for print
                std::string str;
                for (size_t i = 0; i < dxs.size(); ++i) {
                    str += std::format("{{{} {}}},", dxs[i].first, dxs[i].second);
                }
                std::cout << std::format("vstem {} {} [{}] ", x, dx, str) << std::endl;
                break;
            }

            case Type2HintOperators::VStemHM: {
                const auto num_pairs = (values.size() - 2) / 2;
                std::vector<std::pair<int, int>> dxs{};
                dxs.reserve(num_pairs);
                for (auto i = 0; i < num_pairs; i++) {
                    const int dxb = values.top();
                    values.pop();
                    const int dxa = values.top();
                    values.pop();
                    dxs.emplace_back(dxa, dxb);
                }
                const int dx = values.top();
                values.pop();
                const int x = values.top();
                values.pop();

                // This is all for print
                std::string str;
                for (size_t i = 0; i < dxs.size(); ++i) {
                    str += std::format("{{{} {}}},", dxs[i].first, dxs[i].second);
                }
                std::cout << std::format("vstemhm {} {} [{}] ", x, dx, str) << std::endl;
                break;
            }

            case Type2HintOperators::HintMask:
            case Type2HintOperators::CntrMask:
                DEBUG_ASSERT(
                    false,
                    std::format("Error: Hint operator {} not yet implemented", oper));
            }
        }

        /*bool state.path_open = false;*/
        /*OutlineControlPoints points;*/
        /*int x = 0;*/
        /*int y = 0;*/
        std::pair<int, int> contour_start(0, 0);
        for (auto i = 0; i < operators.size(); i++) {
            const int oper = operators[i];
            auto &values = value_stacks[i];
            switch (oper) {

            case Type2MoveToOperators::RMoveTo: {
                const int dy1 = values.top();
                values.pop();
                const int dx1 = values.top();
                values.pop();
                state.x += dx1;
                state.y += dy1;
                if (state.path_open) {
                    state.path_open = false;
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       contour_start.first,
                                                       contour_start.second);
                    state.outlines.push_back(std::move(state.current_outline));
                    state.current_outline = {};
                    std::cout << std::format("rmoveto (closed path): ({},{})", state.x,
                                             state.y)
                              << std::endl;
                } else {
                    state.path_open = true;

                    std::cout << std::format("rmoveto (opened path): ({},{})", state.x,
                                             state.y)
                              << std::endl;
                }
                contour_start = std::make_pair(state.x, state.y);
                break;
            }

            case Type2MoveToOperators::HMoveTo: {
                const int dx1 = values.top();
                values.pop();
                state.x += dx1;
                if (state.path_open) {
                    state.path_open = false;
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       contour_start.first,
                                                       contour_start.second);
                    state.outlines.push_back(std::move(state.current_outline));
                    state.current_outline = {};
                    std::cout << std::format("hmoveto (closed path): ({},{})", state.x,
                                             state.y)
                              << std::endl;
                } else {
                    state.path_open = true;

                    std::cout << std::format("hmoveto (opened path): ({},{})", state.x,
                                             state.y)
                              << std::endl;
                }
                contour_start = std::make_pair(state.x, state.y);
                break;
            }

            case Type2MoveToOperators::VMoveTo: {
                const int dy1 = values.top();
                values.pop();
                state.y += dy1;
                if (state.path_open) {
                    state.path_open = false;
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       contour_start.first,
                                                       contour_start.second);
                    state.outlines.push_back(std::move(state.current_outline));
                    state.current_outline = {};
                    std::cout << std::format("vmoveto (closed path): ({},{})", state.x,
                                             state.y)
                              << std::endl;

                    contour_start = std::make_pair(state.x, state.y);
                } else {
                    contour_start = std::make_pair(state.x, state.y);
                    state.path_open = true;

                    std::cout << std::format("vmoveto (opened path): ({},{})", state.x,
                                             state.y)
                              << std::endl;
                }
                contour_start = std::make_pair(state.x, state.y);
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
                }

                for (int i = ds.size() - 1; i >= 0; i--) {
                    state.x += ds[i].first;
                    state.y += ds[i].second;
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       state.x, state.y);
                    std::cout << std::format("rlineto: ({},{}) ", state.x, state.y)
                              << std::endl;
                }

                /*DEBUG_ASSERT(state.path_open,*/
                /*             "Error: expected path to be open before path operator");*/
                break;
            }

            case Type2PathConstructOperators::HLineTo: {
                const auto num_operands = values.size();
                std::vector<int> operands;
                operands.resize(num_operands);
                for (int i = operands.size() - 1; i >= 0; i--) {
                    operands[i] = values.top();
                    values.pop();
                }

                for (auto i = 0; i < operands.size(); i++) {
                    if (i % 2 == 0) {
                        state.x += operands[i];
                    } else {
                        state.y += operands[i];
                    }
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       state.x, state.y);
                    std::cout << std::format("hlineto ({},{})", state.x, state.y)
                              << std::endl;
                }
                break;
            }

            case Type2PathConstructOperators::VLineTo: {
                const auto num_operands = values.size();
                std::vector<int> operands;
                operands.resize(num_operands);
                for (int i = operands.size() - 1; i >= 0; i--) {
                    operands[i] = values.top();
                    values.pop();
                }

                for (auto i = 0; i < operands.size(); i++) {
                    if (i % 2 == 0) {
                        state.y += operands[i];
                    } else {
                        state.x += operands[i];
                    }
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       state.x, state.y);
                    std::cout << std::format("vlineto ({},{})", state.x, state.y)
                              << std::endl;
                }
                break;
            }

            case Type2PathConstructOperators::RRCurveTo: {
                const auto num_operands = values.size();
                std::vector<int> operands;
                operands.resize(num_operands);
                for (int i = operands.size() - 1; i >= 0; i--) {
                    operands[i] = values.top();
                    values.pop();
                }

                for (auto i = 0; i < num_operands / 6; i++) {
                    const int dxa = operands[i * 6 + 0];
                    const int dya = operands[i * 6 + 1];
                    const int dxb = operands[i * 6 + 2];
                    const int dyb = operands[i * 6 + 3];
                    const int dxc = operands[i * 6 + 4];
                    const int dyc = operands[i * 6 + 5];

                    //  Control point A is the last value of the points vector
                    //  Control point B
                    state.x += dxa;
                    state.y += dya;
                    std::cout << std::format("rrcurveto: ({},{}) ", state.x, state.y);
                    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>,
                                                       state.x, state.y);

                    // Control point C
                    state.x += dxb;
                    state.y += dyb;
                    std::cout << std::format("({},{}) ", state.x, state.y);
                    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>,
                                                       state.x, state.y);

                    // Control point D
                    state.x += dxc;
                    state.y += dyc;
                    std::cout << std::format("({},{})", state.x, state.y) << std::endl;
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       state.x, state.y);
                }

                /*DEBUG_ASSERT(state.path_open,*/
                /*             "Error: expected path to be open before path operator");*/
                break;
            }

            case Type2PathConstructOperators::RCurveLine: {
                DEBUG_ASSERT(false, "Error: RCurveLine operator is not yet implemented");
                break;
            }

            case Type2PathConstructOperators::VVCurveTo: {
                // |- dx1? {dya dxb dyb dyc}+ vvcurveto (26) |-
                // We do some odd things with multiples of 4 to only read exactly what we
                // expect. The assert at the end of this function will catch any errors.
                const size_t num_operations = values.size() / 4;
                std::vector<int> operands;
                operands.resize(num_operations * 4);
                auto count = operands.size() - 1;
                while (values.size() >= 4) {
                    operands[count--] = values.top();
                    values.pop();
                    operands[count--] = values.top();
                    values.pop();
                    operands[count--] = values.top();
                    values.pop();
                    operands[count--] = values.top();
                    values.pop();
                }
                if (values.size() > 0) {
                    const int dx1 = values.top();
                    state.x += dx1;
                    values.pop();
                }
                for (auto i = 0; i < num_operations; i++) {
                    const int dya = operands[i * 4 + 0];
                    const int dxb = operands[i * 4 + 1];
                    const int dyb = operands[i * 4 + 2];
                    const int dyc = operands[i * 4 + 3];

                    state.y += dya;
                    std::cout << std::format("vvcurveto: ({},{}) ", state.x, state.y);
                    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>,
                                                       state.x, state.y);

                    // Second control point
                    state.x += dxb;
                    state.y += dyb;
                    std::cout << std::format("({},{}) ", state.x, state.y);
                    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>,
                                                       state.x, state.y);

                    state.y += dyc;
                    std::cout << std::format("({},{})", state.x, state.y) << std::endl;
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       state.x, state.y);
                }

                /*DEBUG_ASSERT(state.path_open,*/
                /*             "Error: expected path to be open before path operator");*/
                break;
            }

            case Type2PathConstructOperators::HHCurveTo: {
                // |- dy1? {dxa dxb dyb dxc}+ hhcurveto (27) |-
                // We do some odd things with multiples of 4 to only read exactly what we
                // expect. The assert at the end of this function will catch any errors.
                const size_t num_operations = values.size() / 4;
                std::vector<int> operands;
                operands.resize(num_operations * 4);
                auto count = operands.size() - 1;
                while (values.size() >= 4) {
                    operands[count--] = values.top();
                    values.pop();

                    operands[count--] = values.top();
                    values.pop();

                    operands[count--] = values.top();
                    values.pop();

                    operands[count--] = values.top();
                    values.pop();
                }

                if (values.size() > 0) {
                    const int dy1 = values.top();
                    state.y += dy1;
                    values.pop();
                }

                for (auto i = 0; i < num_operations; i++) {
                    const int dxa = operands[i * 4 + 0];
                    const int dxb = operands[i * 4 + 1];
                    const int dyb = operands[i * 4 + 2];
                    const int dxc = operands[i * 4 + 3];

                    state.x += dxa;
                    std::cout << std::format("hhcurveto: ({},{}) ", state.x, state.y);
                    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>,
                                                       state.x, state.y);

                    state.x += dxb;
                    state.y += dyb;
                    std::cout << std::format("({},{}) ", state.x, state.y);
                    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>,
                                                       state.x, state.y);

                    state.x += dxc;
                    std::cout << std::format("({},{})", state.x, state.y) << std::endl;
                    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                       state.x, state.y);
                }

                break;
            }

            // Hintmasks operators are allowed in the subpath
            case Type2HintOperators::HintMask:
                DEBUG_ASSERT(
                    false, "Error: HintMask operators in subpath is not yet implemented");
                break;

            case Type2Operators::EndChar: {
                std::cout << "endchar: "
                          << std::format("({},{})", contour_start.first,
                                         contour_start.second)
                          << std::endl;

                state.path_open = false;
                state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                                   contour_start.first,
                                                   contour_start.second);

                state.outlines.push_back(std::move(state.current_outline));
                state.current_outline = {};
                break;
            }

            case Type2Operators::CallSubr: {
                const auto num_operands = values.size();
                std::vector<int> operands;
                operands.resize(num_operands);
                for (int i = operands.size() - 1; i >= 0; i--) {
                    operands[i] = values.top();
                    values.pop();
                }

                for (int c : operands) {
                    const size_t subr_index =
                        subroutine_index_correction(c, local_subrs.count);
                    std::cout << std::format("callsubr: {}", subr_index) << std::endl;
                    const auto subroutine = local_subrs[subr_index];
                    decode_glyph(subroutine, global_subrs, local_subrs, state);
                }

                // TODO: I expect the lower part of the "i" to be visible. WHy is it not?
                // TODO: If the current path is open when a moveto operator is
                // encountered, the path should be closed before performing the moveto
                // TODO: Return multiple outlines
                /*DEBUG_ASSERT(false, "Error: CallSubr operator not yet implemented");*/
                break;
            }

            case Type2Operators::CallGSubr: {
                DEBUG_ASSERT(false, "Error: CallGSubr operator not yet implemented");
                break;
            }

            case Type2Operators::Return: {
                std::cout << "return" << std::endl;
                DEBUG_ASSERT(
                    i == operators.size() - 1,
                    "Error: return operator found when there are operators following.");

                break;
            }

            // These operators are handles above
            case Type2HintOperators::HStem:
            case Type2HintOperators::VStem:
            case Type2HintOperators::HStemHM:
            case Type2HintOperators::VStemHM:
            case Type2HintOperators::CntrMask:
                break;

            default: {
                DEBUG_ASSERT(false,
                             std::format("Error: operator {} not yet implemented", oper));
                break;
            }
            }
        }

        // Check if the sequence started with a width
        if (value_stacks[0].size() > 0) {
            state.width = value_stacks[0].top();
            value_stacks[0].pop();
        }

        DEBUG_ASSERT(value_stacks[0].size() == 0,
                     "Error: glyph sequence still has an unprocessed width value");
        DEBUG_CODE(for (auto i = 1; i < value_stacks.size(); i++) {
            DEBUG_ASSERT(value_stacks[i].size() == 0,
                         std::format("Error: glyph sequence {} (operand {}) still has an "
                                     "unprocessed value",
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
};

}; // namespace font::detail::otf_font::cff
