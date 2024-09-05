#pragma once
#include <vector>

template <typename T, typename A = std::allocator<T>> using Vector = std::vector<T, A>;
