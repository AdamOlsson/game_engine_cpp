#pragma once

#include <iostream>
namespace vulkan {

template <typename T>
concept Printable = requires(T t, std::ostream &os) { os << t; };

} // namespace vulkan
