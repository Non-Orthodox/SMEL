#ifndef SYMBOLIC_INCLUDE_CONCEPTS_HPP
#define SYMBOLIC_INCLUDE_CONCEPTS_HPP

#include <concepts>

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename T>
concept IntegralFloat = IsAnyOf<T, float, double, long double>;

template<typename T>
concept IntegralInt = IsAnyOf<T,signed char, short int, int, long int, long long int>;

template<typename T>
concept IntegralConstant = IntegralFloat<T> || IntegralInt<T>;

#endif