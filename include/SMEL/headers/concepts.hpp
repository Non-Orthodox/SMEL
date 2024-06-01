#ifndef SYMBOLIC_INCLUDE_CONCEPTS_HPP
#define SYMBOLIC_INCLUDE_CONCEPTS_HPP

#include <concepts>

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template<typename T>
concept IntegralInt = IsAnyOf<T,char, signed char, short int, int, long int, long long int>;

template<typename T>
concept IntegralConstant = std::floating_point<T> || IntegralInt<T>;

#endif