#pragma once

#include <iostream>
template <typename T>
concept Printable = requires(T t, std::ostream &os) { os << t; };

template <typename T>
concept StaticCastableToFloat = requires(T t) {
    { static_cast<float>(t) };
};
