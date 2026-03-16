#pragma once
#include <optional>
#include <typeindex>

namespace util {

struct StateTransition {
    std::optional<std::type_index> next = std::nullopt;

    template <typename S> static StateTransition to() {
        return {std::type_index(typeid(S))};
    }

    static StateTransition none() { return {}; }
};

} // namespace util
