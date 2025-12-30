#include "font/detail/otf_font/cff/Type2Charstring.h"
#include "math/area.h"
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

constexpr bool font::detail::otf_font::cff::Type2Charstring::is_clockwise_winding(
    const font::Triangle<float> &triangle) {
    return math::signed_area_triangle(triangle) > 0.0f;
}

font::GlyphVertices font::detail::otf_font::cff::Type2Charstring::parse_outline(
    const OutlineControlPoints &control_points) {
    GlyphVertices vertices;
    vertices.interior.reserve(control_points.size());
    vertices.exterior.reserve(control_points.size());

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
                .clockwise_winding = is_clockwise_winding(triangle1_vertices),
                .vertices = std::move(triangle1_vertices),
                .uvw =
                    {
                        UVW{0.0f, 0.0f, 1.0f},
                        UVW{0.5f, 0.0f, 1.0f},
                        UVW{1.0f, 1.0f, 1.0f},
                    },
            };
            vertices.exterior.emplace_back(std::move(triangle1));

            if (triangle1.clockwise_winding) {
                vertices.interior.emplace_back(quad_bezier1.p1);
            }

            // When approximating the cubic bezier with 2 quadratic bezier we leave
            // a second under the curve that always should be filled
            vertices.interior.emplace_back(std::move(quad_bezier1.p2));

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
            vertices.exterior.emplace_back(std::move(triangle2));

            if (triangle2.clockwise_winding) {
                vertices.interior.emplace_back(quad_bezier2.p1);
            }

            i++; // Increment past the second off-curve control point
        } else {
            const auto p = decay_to_point(control_point);
            vertices.interior.emplace_back(std::move(p));
        }
    }

    vertices.interior.shrink_to_fit();
    vertices.exterior.shrink_to_fit();

    return vertices;
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
    const int dy1 = operands.top();
    operands.pop();
    const int dx1 = operands.top();
    operands.pop();
    state.x += dx1;
    state.y += dy1;
    if (state.path_open) {
        state.path_open = false;
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                           state.contour_start.first,
                                           state.contour_start.second);
        state.outlines.push_back(std::move(state.current_outline));
        state.current_outline = {};
        /*std::cout << std::format("rmoveto (closed path): ({},{})", state.x, state.y)*/
        /*          << std::endl;*/
    } else {
        state.path_open = true;

        /*std::cout << std::format("rmoveto (opened path): ({},{})", state.x, state.y)*/
        /*          << std::endl;*/
    }
    state.contour_start = std::make_pair(state.x, state.y);
}

void font::detail::otf_font::cff::Type2Charstring::handle_hmoveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const int dx1 = operands.top();
    operands.pop();
    state.x += dx1;
    if (state.path_open) {
        state.path_open = false;
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                           state.contour_start.first,
                                           state.contour_start.second);
        state.outlines.push_back(std::move(state.current_outline));
        state.current_outline = {};
        /*std::cout << std::format("hmoveto (closed path): ({},{})", state.x, state.y)*/
        /*          << std::endl;*/
    } else {
        state.path_open = true;

        /*std::cout << std::format("hmoveto (opened path): ({},{})", state.x, state.y)*/
        /*          << std::endl;*/
    }
    state.contour_start = std::make_pair(state.x, state.y);
}

void font::detail::otf_font::cff::Type2Charstring::handle_vmoveto(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    const int dy1 = operands.top();
    operands.pop();
    state.y += dy1;
    if (state.path_open) {
        state.path_open = false;
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                           state.contour_start.first,
                                           state.contour_start.second);
        state.outlines.push_back(std::move(state.current_outline));
        state.current_outline = {};
        /*std::cout << std::format("vmoveto (closed path): ({},{})", state.x, state.y)*/
        /*          << std::endl;*/

        state.contour_start = std::make_pair(state.x, state.y);
    } else {
        state.contour_start = std::make_pair(state.x, state.y);
        state.path_open = true;

        /*std::cout << std::format("vmoveto (opened path): ({},{})", state.x, state.y)*/
        /*          << std::endl;*/
    }
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
        /*std::cout << std::format("rlineto: ({},{}) ", state.x, state.y) << std::endl;*/
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
        /*std::cout << std::format("vlineto ({},{})", state.x, state.y) << std::endl;*/
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
        /*std::cout << std::format("hlineto ({},{})", state.x, state.y) << std::endl;*/
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

void font::detail::otf_font::cff::Type2Charstring::handle_vhcurveline(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    DEBUG_ASSERT(false, "Error: VHCurveToperator is not yet implemented");
}

void font::detail::otf_font::cff::Type2Charstring::handle_hvcurveline(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    DEBUG_ASSERT(false, "Error: HVCurveToperator is not yet implemented");
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

        //  Control point A is the previous value of the points vector
        //  Control point B
        state.x += dxa;
        state.y += dya;
        /*std::cout << std::format("rrcurveto: ({},{}) ", state.x, state.y);*/
        state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                           state.y);

        // Control point C
        state.x += dxb;
        state.y += dyb;
        /*std::cout << std::format("({},{}) ", state.x, state.y);*/
        state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                           state.y);

        // Control point D
        state.x += dxc;
        state.y += dyc;
        /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>, state.x,
                                           state.y);
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

        state.y += dya;
        /*std::cout << std::format("vvcurveto: ({},{}) ", state.x, state.y);*/
        state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                           state.y);

        // Second control point
        state.x += dxb;
        state.y += dyb;
        /*std::cout << std::format("({},{}) ", state.x, state.y);*/
        state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                           state.y);

        state.y += dyc;
        /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>, state.x,
                                           state.y);
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

        state.x += dxa;
        /*std::cout << std::format("hhcurveto: ({},{}) ", state.x, state.y);*/
        state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                           state.y);

        state.x += dxb;
        state.y += dyb;
        /*std::cout << std::format("({},{}) ", state.x, state.y);*/
        state.current_outline.emplace_back(std::in_place_type<OffCurvePoint>, state.x,
                                           state.y);

        state.x += dxc;
        /*std::cout << std::format("({},{})", state.x, state.y) << std::endl;*/
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>, state.x,
                                           state.y);
    }
}

void font::detail::otf_font::cff::Type2Charstring::handle_endchar(
    font::detail::otf_font::cff::DecodeState &state, std::stack<int> &operands) {
    /*std::cout << "endchar: "*/
    /*<< std::format("({},{})", state.contour_start.first,*/
    /*               state.contour_start.second)*/
    /*<< std::endl;*/

    state.path_open = false;

    // Most fonts contain atleast one "empty" glyph. If we encounter an empty glyph, do
    // not create an outline
    if (state.current_outline.size() != 0) {
        state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                           state.contour_start.first,
                                           state.contour_start.second);
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

    for (int c : operands_vec) {
        const size_t subr_index = subroutine_index_correction(c, local_subrs.count);
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
