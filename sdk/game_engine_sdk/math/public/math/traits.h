#pragma once

namespace math {
template <typename T>
concept StaticCastableToFloat = requires(T t) {
    { static_cast<float>(t) };
};
} // namespace math
