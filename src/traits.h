#pragma once

#include <iostream>
template <typename T>
concept Printable = requires(T t) { std::cout << t; };
