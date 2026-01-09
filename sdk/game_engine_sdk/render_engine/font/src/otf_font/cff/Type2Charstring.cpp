#include "font/detail/otf_font/cff/Type2Charstring.h"
#include "font/winding.h"
#include "math/interpolate.h"

constexpr bool font::detail::otf_font::cff::Type2Charstring::is_off_curve_point(
    const std::variant<OffCurvePoint, OnCurvePoint> &p) {
    return std::visit(
        [](const auto &p) -> bool {
            using T = std::decay_t<decltype(p)>;
            return std::is_same_v<T, OffCurvePoint>;
        },
        p);
}

constexpr std::pair<int, int>
font::detail::otf_font::cff::Type2Charstring::decay_to_point(
    const std::variant<OffCurvePoint, OnCurvePoint> &p) {
    return std::visit(
        [](const auto &p) -> std::pair<int, int> {
            using T = std::decay_t<decltype(p)>;
            if constexpr (std::is_same_v<T, OnCurvePoint>) {
                return std::make_pair(p.x, p.y);
            } else if constexpr (std::is_same_v<T, OffCurvePoint>) {
                return std::make_pair(p.x, p.y);
            }
            throw std::runtime_error("Error: Unkown curve point type");
        },
        p);
}

font::GlyphOutline font::detail::otf_font::cff::Type2Charstring::parse_outline(
    const OutlineControlPoints &control_points) {
    GlyphOutline outline;
    outline.vertices.reserve(control_points.size());
    outline.curves.reserve(control_points.size());

    for (size_t i = 0; i < control_points.size(); i++) {
        const auto &control_point = control_points[i];

        // If the following control point is an off-curve point, the current point
        // is the first off-curve control point of a cubic bezier curve
        if (is_off_curve_point(control_point)) {
            const std::pair<int, int> p0 = decay_to_point(
                control_points[(i + control_points.size() - 1) % control_points.size()]);
            const std::pair<int, int> p1 = decay_to_point(control_point);
            const std::pair<int, int> p2 = decay_to_point(control_points[i + 1]);
            const std::pair<int, int> p3 = decay_to_point(control_points[i + 2]);

            const math::CubicBezier cubic_bezier = math::CubicBezier{p0, p1, p2, p3};
            const std::pair<math::CubicBezier, math::CubicBezier> split =
                math::de_casteljaus(cubic_bezier);

            const math::QuadraticBezer quad_bezier1 =
                math::approximate_quadratic_bezier(split.first);
            const math::QuadraticBezer quad_bezier2 =
                math::approximate_quadratic_bezier(split.second);

            font::Triangle<float> triangle1_vertices =
                std::array{quad_bezier1.p0, quad_bezier1.p1, quad_bezier1.p2};
            ExteriorTriangle triangle1 = {
                .clockwise_winding = font::is_clockwise_winding(triangle1_vertices),
                .vertices = std::move(triangle1_vertices),
                .uvw =
                    {
                        UVW{0.0f, 0.0f, 1.0f},
                        UVW{0.5f, 0.0f, 1.0f},
                        UVW{1.0f, 1.0f, 1.0f},
                    },
            };
            outline.curves.emplace_back(std::move(triangle1));

            if (triangle1.clockwise_winding) {
                outline.vertices.emplace_back(quad_bezier1.p1);
            }

            // When approximating the cubic bezier with 2 quadratic bezier we leave
            // a second under the curve that always should be filled
            outline.vertices.emplace_back(std::move(quad_bezier1.p2));

            font::Triangle triangle2_vertices =
                std::array{quad_bezier2.p0, quad_bezier2.p1, quad_bezier2.p2};
            ExteriorTriangle triangle2 = {
                .clockwise_winding = is_clockwise_winding(triangle2_vertices),
                .vertices = std::move(triangle2_vertices),
                .uvw =
                    {
                        UVW{0.0f, 0.0f, 1.0f},
                        UVW{0.5f, 0.0f, 1.0f},
                        UVW{1.0f, 1.0f, 1.0f},
                    },
            };
            outline.curves.emplace_back(std::move(triangle2));

            if (triangle2.clockwise_winding) {
                outline.vertices.emplace_back(quad_bezier2.p1);
            }

            i++; // Increment past the second off-curve control point
        } else {
            const auto p = decay_to_point(control_point);
            outline.vertices.emplace_back(std::move(p));
        }
    }

    outline.vertices.shrink_to_fit();
    outline.curves.shrink_to_fit();

    return outline;
}

void font::detail::otf_font::cff::Type2Charstring::handle_hstem(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    state.hint_count++;
    const auto num_pairs = (operands.size() - 2) / 2;
    std::vector<std::pair<int, int>> dys{};
    dys.reserve(num_pairs);
    for (auto i = 0; i < num_pairs; i++) {
        const int dyb = operands.top();
        operands.pop();
        const int dya = operands.top();
        operands.pop();
        dys.emplace_back(dya, dyb);
        state.hint_count++;
    }
    const int dy = operands.top();
    operands.pop();
    const int y = operands.top();
    operands.pop();
    state.hint_count++;

    // This is all for print
    /*std::string str;*/
    /*for (size_t i = 0; i < dys.size(); ++i) {*/
    /*    str += std::format("({},{}) ", dys[i].first, dys[i].second);*/
    /*}*/
    /*std::cout << std::format("hstem: {} {} [{}]", y, dy, str) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_hstemhm(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    state.hint_count++;
    const auto num_pairs = (operands.size() - 2) / 2;
    std::vector<std::pair<int, int>> dys{};
    dys.reserve(num_pairs);
    for (auto i = 0; i < num_pairs; i++) {
        const int dyb = operands.top();
        operands.pop();
        const int dya = operands.top();
        operands.pop();
        dys.emplace_back(dya, dyb);
        state.hint_count++;
    }
    const int dy = operands.top();
    operands.pop();
    const int y = operands.top();
    operands.pop();
    state.hint_count++;

    // This is all for print
    /*std::string str;*/
    /*for (size_t i = 0; i < dys.size(); ++i) {*/
    /*    str += std::format("({},{}) ", dys[i].first, dys[i].second);*/
    /*}*/
    /*std::cout << std::format("hstemhm: {} {} [{}]", y, dy, str) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_vstem(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const auto num_pairs = (operands.size() - 2) / 2;
    std::vector<std::pair<int, int>> dxs{};
    dxs.reserve(num_pairs);
    for (auto i = 0; i < num_pairs; i++) {
        const int dxb = operands.top();
        operands.pop();
        const int dxa = operands.top();
        operands.pop();
        dxs.emplace_back(dxa, dxb);
        state.hint_count++;
    }
    const int dx = operands.top();
    operands.pop();
    const int x = operands.top();
    operands.pop();
    state.hint_count++;

    // This is all for print
    /*std::string str;*/
    /*for (size_t i = 0; i < dxs.size(); ++i) {*/
    /*    str += std::format("{{{} {}}},", dxs[i].first, dxs[i].second);*/
    /*}*/
    /*std::cout << std::format("vstem {} {} [{}] ", x, dx, str) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_vstemhm(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const auto num_pairs = (operands.size() - 2) / 2;
    std::vector<std::pair<int, int>> dxs{};
    dxs.reserve(num_pairs);
    for (auto i = 0; i < num_pairs; i++) {
        const int dxb = operands.top();
        operands.pop();
        const int dxa = operands.top();
        operands.pop();
        dxs.emplace_back(dxa, dxb);
        state.hint_count++;
    }
    const int dx = operands.top();
    operands.pop();
    const int x = operands.top();
    operands.pop();
    state.hint_count++;

    // This is all for print
    /*std::string str;*/
    /*for (size_t i = 0; i < dxs.size(); ++i) {*/
    /*    str += std::format("{{{} {}}},", dxs[i].first, dxs[i].second);*/
    /*}*/
    /*std::cout << std::format("vstemhm {} {} [{}] ", x, dx, str) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_rmoveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {

    if (state.current_outline.size() != 0) {
        if (!(state.x == state.contour_start.first &&
              state.y == state.contour_start.second)) {
            state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                               state.contour_start.first,
                                               state.contour_start.second);
        }
        state.outlines.push_back(std::move(state.current_outline));
        state.current_outline = {};
        /*std::cout << std::format("rmoveto (closed path): ({},{})", state.x, state.y)*/
        /*          << std::endl;*/
    }

    const int dy1 = operands.top();
    operands.pop();
    const int dx1 = operands.top();
    operands.pop();
    state.x += dx1;
    state.y += dy1;

    state.contour_start = std::make_pair(state.x, state.y);
}

void font::detail::otf_font::cff::Type2Charstring::handle_hmoveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {

    if (state.current_outline.size() != 0) {
        if (!(state.x == state.contour_start.first &&
              state.y == state.contour_start.second)) {
            state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                               state.contour_start.first,
                                               state.contour_start.second);
        }
        state.outlines.push_back(std::move(state.current_outline));
        state.current_outline = {};
        /*std::cout << std::format("hmoveto (closed path): ({},{})", state.x,
         * state.y)*/
        /*          << std::endl;*/
    }

    const int dx1 = operands.top();
    operands.pop();
    state.x += dx1;

    state.contour_start = std::make_pair(state.x, state.y);
}

void font::detail::otf_font::cff::Type2Charstring::handle_vmoveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {

    if (state.current_outline.size() != 0) {
        if (!(state.x == state.contour_start.first &&
              state.y == state.contour_start.second)) {
            state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                               state.contour_start.first,
                                               state.contour_start.second);
        }
        state.outlines.push_back(std::move(state.current_outline));
        state.current_outline = {};

        state.contour_start = std::make_pair(state.x, state.y);
    }

    const int dy1 = operands.top();
    operands.pop();
    state.y += dy1;

    state.contour_start = std::make_pair(state.x, state.y);
}

void font::detail::otf_font::cff::Type2Charstring::handle_rlineto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const auto num_pairs = operands.size() / 2;
    std::vector<std::pair<int, int>> ds{};
    ds.reserve(num_pairs);

    for (auto i = 0; i < num_pairs; i++) {
        const int dya = operands.top();
        operands.pop();
        const int dxa = operands.top();
        operands.pop();
        ds.emplace_back(dxa, dya);
    }

    for (int i = ds.size() - 1; i >= 0; i--) {
        state.x += ds[i].first;
        state.y += ds[i].second;
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>, state.x,
                                           state.y);
        /*std::cout << std::format("rlineto: ({},{}) ", state.x, state.y) <<
         * std::endl;*/
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_vlineto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const auto num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = operands_vec.size() - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    for (auto i = 0; i < operands_vec.size(); i++) {
        if (i % 2 == 0) {
            state.y += operands_vec[i];
        } else {
            state.x += operands_vec[i];
        }
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>, state.x,
                                           state.y);
        /*std::cout << std::format("vlineto ({},{})", state.x, state.y) <<
         * std::endl;*/
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_hlineto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const auto num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = operands.size() - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    for (auto i = 0; i < operands_vec.size(); i++) {
        if (i % 2 == 0) {
            state.x += operands_vec[i];
        } else {
            state.y += operands_vec[i];
        }
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>, state.x,
                                           state.y);
        /*std::cout << std::format("hlineto ({},{})", state.x, state.y) <<
         * std::endl;*/
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_rcurveline(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {

    const int dyd = operands.top();
    operands.pop();
    const int dxd = operands.top();
    operands.pop();

    std::stack<int> rlineto_stack;
    rlineto_stack.push(dxd);
    rlineto_stack.push(dyd);

    /*std::cout << std::format("rcurveline: ") << std::endl;*/
    handle_rrcurveto(state, operands);
    handle_rlineto(state, rlineto_stack);

    DEBUG_ASSERT(rlineto_stack.size() == 0, "Error: handle_rlineto() did not consume "
                                            "stack when called from handle_rrcurveto().");
}

void font::detail::otf_font::cff::Type2Charstring::handle_hvcurveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dx1 dx2 dy2 dy3 {dya dxb dyb dxc dxd dxe dye dyf}* dxf? hvcurveto (31) |-
    // |- {dxa dxb dyb dyc dyd dxe dye dxf}+ dyf? hvcurveto (31) |-

    const size_t num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = num_operands - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    size_t i = 0;
    bool horizontal_first = true;

    while (i < num_operands) {
        if (horizontal_first) {
            // First curve starts horizontally: dx1 dx2 dy2 dy3
            if (i + 4 <= num_operands) {
                const int dx1 = operands_vec[i++];
                const int dx2 = operands_vec[i++];
                const int dy2 = operands_vec[i++];
                const int dy3 = operands_vec[i++];

                // Check for optional final argument
                const bool has_final = (i == num_operands - 1);
                const int dyf = has_final ? operands_vec[i++] : 0;

                /*std::cout << std::format("hvcurveto: ");*/
                append_bezier(state, dx1, 0, dx2, dy2, 0, dy3 + dyf);
                /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/

                horizontal_first = false;
            } else {
                break;
            }
        } else {
            // Subsequent curves start vertically: dya dxb dyb dxc
            if (i + 4 <= num_operands) {
                const int dya = operands_vec[i++];
                const int dxb = operands_vec[i++];
                const int dyb = operands_vec[i++];
                const int dxc = operands_vec[i++];

                // Check for optional final argument
                const bool has_final = (i == num_operands - 1);
                const int dxf = has_final ? operands_vec[i++] : 0;

                append_bezier(state, 0, dya, dxb, dyb, dxc + dxf, 0);

                horizontal_first = true;
            } else {
                break;
            }
        }
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_vhcurveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dy1 dx2 dy2 dx3 {dxa dxb dyb dyc dyd dxe dye dxf}* dyf? vhcurveto (30) |-
    // |- {dya dxb dyb dxc dxd dxe dye dyf}+ dxf? vhcurveto (30) |-

    const size_t num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = num_operands - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    size_t i = 0;
    bool vertical_first = true;

    while (i < num_operands) {
        if (vertical_first) {
            // First curve starts vertically: dy1 dx2 dy2 dx3
            if (i + 4 <= num_operands) {
                const int dy1 = operands_vec[i++];
                const int dx2 = operands_vec[i++];
                const int dy2 = operands_vec[i++];
                const int dx3 = operands_vec[i++];

                // Check for optional final argument
                const bool has_final = (i == num_operands - 1);
                const int dxf = has_final ? operands_vec[i++] : 0;

                /*std::cout << std::format("vhcurveto: ");*/
                append_bezier(state, 0, dy1, dx2, dy2, dx3 + dxf, 0);
                /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/

                vertical_first = false;
            } else {
                break;
            }
        } else {
            // Subsequent curves start horizontally: dxa dxb dyb dyc
            if (i + 4 <= num_operands) {
                const int dxa = operands_vec[i++];
                const int dxb = operands_vec[i++];
                const int dyb = operands_vec[i++];
                const int dyc = operands_vec[i++];

                // Check for optional final argument
                const bool has_final = (i == num_operands - 1);
                const int dyf = has_final ? operands_vec[i++] : 0;

                /*std::cout << std::format("vhcurveto: ");*/
                append_bezier(state, dxa, 0, dxb, dyb, 0, dyc + dyf);
                /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/

                vertical_first = true;
            } else {
                break;
            }
        }
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_rrcurveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const auto num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = operands.size() - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    for (auto i = 0; i < num_operands / 6; i++) {
        const int dxa = operands_vec[i * 6 + 0];
        const int dya = operands_vec[i * 6 + 1];
        const int dxb = operands_vec[i * 6 + 2];
        const int dyb = operands_vec[i * 6 + 3];
        const int dxc = operands_vec[i * 6 + 4];
        const int dyc = operands_vec[i * 6 + 5];
        append_bezier(state, dxa, dya, dxb, dyb, dxc, dyc);
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_vvcurveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dx1? {dya dxb dyb dyc}+ vvcurveto (26) |-
    // We do some odd things with multiples of 4 to only read exactly what we
    // expect. The assert at the end of this function will catch any errors.
    const size_t num_operations = operands.size() / 4;
    std::vector<int> operands_vec;
    operands_vec.resize(num_operations * 4);
    auto count = operands.size() - 1;
    while (operands.size() >= 4) {
        operands_vec[count--] = operands.top();
        operands.pop();
        operands_vec[count--] = operands.top();
        operands.pop();
        operands_vec[count--] = operands.top();
        operands.pop();
        operands_vec[count--] = operands.top();
        operands.pop();
    }

    if (operands.size() > 0) {
        const int dx1 = operands.top();
        state.x += dx1;
        operands.pop();
    }

    for (auto i = 0; i < num_operations; i++) {
        const int dya = operands_vec[i * 4 + 0];
        const int dxb = operands_vec[i * 4 + 1];
        const int dyb = operands_vec[i * 4 + 2];
        const int dyc = operands_vec[i * 4 + 3];
        append_bezier(state, 0, dya, dxb, dyb, 0, dyc);
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_rlinecurve(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const int dyd = operands.top();
    operands.pop();
    const int dxd = operands.top();
    operands.pop();
    const int dyc = operands.top();
    operands.pop();
    const int dxc = operands.top();
    operands.pop();
    const int dyb = operands.top();
    operands.pop();
    const int dxb = operands.top();
    operands.pop();
    append_bezier(state, dxb, dyb, dxc, dyc, dxd, dyd);

    if (operands.size() > 0) {
        handle_rlineto(state, operands);
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_hhcurveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dy1? {dxa dxb dyb dxc}+ hhcurveto (27) |-
    // We do some odd things with multiples of 4 to only read exactly what we
    // expect. The assert at the end of this function will catch any errors.
    const size_t num_operations = operands.size() / 4;
    std::vector<int> operands_vec;
    operands_vec.resize(num_operations * 4);
    auto count = operands.size() - 1;
    while (operands.size() >= 4) {
        operands_vec[count--] = operands.top();
        operands.pop();

        operands_vec[count--] = operands.top();
        operands.pop();

        operands_vec[count--] = operands.top();
        operands.pop();

        operands_vec[count--] = operands.top();
        operands.pop();
    }

    if (operands.size() > 0) {
        const int dy1 = operands.top();
        state.y += dy1;
        operands.pop();
    }

    for (auto i = 0; i < num_operations; i++) {
        const int dxa = operands_vec[i * 4 + 0];
        const int dxb = operands_vec[i * 4 + 1];
        const int dyb = operands_vec[i * 4 + 2];
        const int dxc = operands_vec[i * 4 + 3];

        append_bezier(state, dxa, 0, dxb, dyb, dxc, 0);
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_flex(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dx1 dy1 dx2 dy2 dx3 dy3 dx4 dy4 dx5 dy5 dx6 dy6 fd flex (12 35) |-
    // Draws two Bézier curves that join at an inflection point
    // fd (flex depth) is ignored in modern implementations

    DEBUG_ASSERT(operands.size() == 13,
                 "Error: flex operator requires exactly 13 operands");

    const size_t num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = num_operands - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    const int dx1 = operands_vec[0];
    const int dy1 = operands_vec[1];
    const int dx2 = operands_vec[2];
    const int dy2 = operands_vec[3];
    const int dx3 = operands_vec[4];
    const int dy3 = operands_vec[5];
    const int dx4 = operands_vec[6];
    const int dy4 = operands_vec[7];
    const int dx5 = operands_vec[8];
    const int dy5 = operands_vec[9];
    const int dx6 = operands_vec[10];
    const int dy6 = operands_vec[11];
    // const int fd = operands_vec[12];  // flex depth - ignored

    /*std::cout << std::format("flex: ");*/
    append_bezier(state, dx1, dy1, dx2, dy2, dx3, dy3);
    append_bezier(state, dx4, dy4, dx5, dy5, dx6, dy6);
    /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_hflex(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dx1 dx2 dy2 dx3 dx4 dx5 dx6 hflex (12 34) |-
    // Horizontal flex: first curve starts horizontal, ends at dy2
    // Second curve mirrors back to y = 0

    DEBUG_ASSERT(operands.size() == 7,
                 "Error: hflex operator requires exactly 7 operands");

    const size_t num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = num_operands - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    const int dx1 = operands_vec[0];
    const int dx2 = operands_vec[1];
    const int dy2 = operands_vec[2];
    const int dx3 = operands_vec[3];
    const int dx4 = operands_vec[4];
    const int dx5 = operands_vec[5];
    const int dx6 = operands_vec[6];

    /*std::cout << std::format("hflex: ");*/
    // First curve: horizontal start, curves up/down
    append_bezier(state, dx1, 0, dx2, dy2, dx3, 0);
    // Second curve: curves back to baseline
    append_bezier(state, dx4, 0, dx5, -dy2, dx6, 0);
    /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_hflex1(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dx1 dy1 dx2 dy2 dx3 dx4 dx5 dy5 dx6 hflex1 (12 36) |-
    // Horizontal flex variant: more flexible than hflex

    DEBUG_ASSERT(operands.size() == 9,
                 "Error: hflex1 operator requires exactly 9 operands");

    const size_t num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = num_operands - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    const int dx1 = operands_vec[0];
    const int dy1 = operands_vec[1];
    const int dx2 = operands_vec[2];
    const int dy2 = operands_vec[3];
    const int dx3 = operands_vec[4];
    const int dx4 = operands_vec[5];
    const int dx5 = operands_vec[6];
    const int dy5 = operands_vec[7];
    const int dx6 = operands_vec[8];

    // Calculate dy6 to return to starting y coordinate
    const int dy6 = -(dy1 + dy2 + dy5);

    /*std::cout << std::format("hflex1: ");*/
    // First curve
    append_bezier(state, dx1, dy1, dx2, dy2, dx3, 0);
    // Second curve
    append_bezier(state, dx4, 0, dx5, dy5, dx6, dy6);
    /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_flex1(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    // |- dx1 dy1 dx2 dy2 dx3 dy3 dx4 dy4 dx5 dy5 d6 flex1 (12 37) |-
    // Most flexible variant: d6 applies to whichever axis has larger displacement

    DEBUG_ASSERT(operands.size() == 11,
                 "Error: flex1 operator requires exactly 11 operands");

    const size_t num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = num_operands - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    const int dx1 = operands_vec[0];
    const int dy1 = operands_vec[1];
    const int dx2 = operands_vec[2];
    const int dy2 = operands_vec[3];
    const int dx3 = operands_vec[4];
    const int dy3 = operands_vec[5];
    const int dx4 = operands_vec[6];
    const int dy4 = operands_vec[7];
    const int dx5 = operands_vec[8];
    const int dy5 = operands_vec[9];
    const int d6 = operands_vec[10];

    // Calculate total displacement to determine which axis is primary
    const int total_dx = dx1 + dx2 + dx3 + dx4 + dx5;
    const int total_dy = dy1 + dy2 + dy3 + dy4 + dy5;

    int dx6, dy6;
    if (std::abs(total_dx) > std::abs(total_dy)) {
        // Horizontal displacement is larger, d6 is dx6
        dx6 = d6;
        dy6 = -total_dy;
    } else {
        // Vertical displacement is larger, d6 is dy6
        dx6 = -total_dx;
        dy6 = d6;
    }

    /*std::cout << std::format("flex1: ");*/
    // First curve
    append_bezier(state, dx1, dy1, dx2, dy2, dx3, dy3);
    // Second curve
    append_bezier(state, dx4, dy4, dx5, dy5, dx6, dy6);
    /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/
}

void font::detail::otf_font::cff::Type2Charstring::handle_endchar(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    /*std::cout << "endchar: "*/
    /*<< std::format("({},{})", state.contour_start.first,*/
    /*               state.contour_start.second)*/
    /*<< std::endl;*/

    // Most fonts contain atleast one "empty" glyph. If we encounter an empty glyph,
    // do not create an outline
    if (state.current_outline.size() != 0) {
        if (!(state.x == state.contour_start.first &&
              state.y == state.contour_start.second)) {
            state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                               state.contour_start.first,
                                               state.contour_start.second);
        }
    }

    state.outlines.push_back(std::move(state.current_outline));
    state.current_outline = {};
}

void font::detail::otf_font::cff::Type2Charstring::handle_callsubr(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands,
    const CFFIndex &global_subrs, const CFFIndex &local_subrs) {

    const auto num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = operands.size() - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    /*std::cout << "Local subr count: " << local_subrs.count << std::endl;*/
    /*std::cout << "Global subr count: " << global_subrs.count << std::endl;*/
    for (int c : operands_vec) {
        /*std::cout << "Unbiased local subroutine index: " << c << std::endl;*/
        const int subr_index = subroutine_index_correction(c, local_subrs.count);
        /*std::cout << std::format("callsubr: {}", subr_index) << std::endl;*/
        const auto subroutine = local_subrs[subr_index];
        decode_glyph(subroutine, global_subrs, local_subrs, state);
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_callgsubr(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands,
    const CFFIndex &global_subrs, const CFFIndex &local_subrs) {

    const auto num_operands = operands.size();
    std::vector<int> operands_vec;
    operands_vec.resize(num_operands);
    for (int i = operands.size() - 1; i >= 0; i--) {
        operands_vec[i] = operands.top();
        operands.pop();
    }

    for (int c : operands_vec) {
        const size_t subr_index = subroutine_index_correction(c, global_subrs.count);
        /*std::cout << std::format("callgsubr: {}", subr_index) << std::endl;*/
        const auto subroutine = global_subrs[subr_index];
        decode_glyph(subroutine, global_subrs, local_subrs, state);
    }
}

void font::detail::otf_font::cff::Type2Charstring::append_bezier(
    font::detail::otf_font::cff::DecodeState &state, const int x1, const int y1,
    const int x2, const int y2, const int x3, const int y3) {

    state.x += x1;
    state.y += y1;
    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                       state.y);

    // Second control point
    state.x += x2;
    state.y += y2;
    state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                       state.y);

    state.x += x3;
    state.y += y3;
    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>, state.x,
                                       state.y);
}
