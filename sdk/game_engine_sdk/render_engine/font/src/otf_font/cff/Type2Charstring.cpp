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
    std::string str;
    for (size_t i = 0; i < dys.size(); ++i) {
        str += std::format("({},{}) ", dys[i].first, dys[i].second);
    }
    std::cout << std::format("hstem: {} {} [{}]", y, dy, str) << std::endl;
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
    std::string str;
    for (size_t i = 0; i < dys.size(); ++i) {
        str += std::format("({},{}) ", dys[i].first, dys[i].second);
    }
    std::cout << std::format("hstemhm: {} {} [{}]", y, dy, str) << std::endl;
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
    std::string str;
    for (size_t i = 0; i < dxs.size(); ++i) {
        str += std::format("{{{} {}}},", dxs[i].first, dxs[i].second);
    }
    std::cout << std::format("vstem {} {} [{}] ", x, dx, str) << std::endl;
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
    std::string str;
    for (size_t i = 0; i < dxs.size(); ++i) {
        str += std::format("{{{} {}}},", dxs[i].first, dxs[i].second);
    }
    std::cout << std::format("vstemhm {} {} [{}] ", x, dx, str) << std::endl;
}
