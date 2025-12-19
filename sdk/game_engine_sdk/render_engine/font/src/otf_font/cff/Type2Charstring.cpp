#include "font/detail/otf_font/cff/Type2Charstring.h"

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

        //  Control point A is the last value of the points vector
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
    state.current_outline.emplace_back(std::in_place_type<OnCurvePoint>,
                                       state.contour_start.first,
                                       state.contour_start.second);

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
