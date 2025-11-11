#pragma once

#include <iostream>
namespace window {
template <typename T>
concept Printable = requires(T t, std::ostream &os) { os << t; };

}; // namespace window
