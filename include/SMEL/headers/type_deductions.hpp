#ifndef SYMBOLIC_INCLUDE_TYPE_DEDUCTIONS_HPP
#define SYMBOLIC_INCLUDE_TYPE_DEDUCTIONS_HPP

#include "symbolic_base.hpp"
#include "constants.hpp"
#include "negation.hpp"
#include "product.hpp"

namespace SYMBOLIC_NAMESPACE_NAME {

// IS ZERO
template<typename SymType>
struct is_zero
{
  static constexpr bool value = false;
};

template<int64_t D>
struct is_zero<Zero<D>>
{
  static constexpr bool value = true;
};

template<>
struct is_zero<Constant<float,0.0f>>
{
  static constexpr bool value = true;
};

template<>
struct is_zero<Constant<double,0.0>>
{
  static constexpr bool value = true;
};

template<typename SymType>
constexpr bool is_zero_v = is_zero<SymType>::value;

// IS ONE
template<typename SymType>
struct is_one
{
  static constexpr bool value = false;
};

template<int64_t D>
struct is_one<One<D>>
{
  static constexpr bool value = true;
};

template<>
struct is_one<Constant<float,1.0f>>
{
  static constexpr bool value = true;
};

template<>
struct is_one<Constant<double,1.0>>
{
  static constexpr bool value = true;
};

template<typename SymType>
constexpr bool is_one_v = is_one<SymType>::value;

// // IS NEGATIVE ONE
// template<typename SymType>
// struct is_negative_one
// {
//   static constexpr bool value = false;
// };

// template<int64_t N>
// struct is_negative_one<RationalConstant<-N,N>>
// {
//   static constexpr bool value = true;
// };

// template<typename SymType>
// constexpr bool is_negative_one_v = is_negative_one<SymType>::value;

// IS RATIONAL CONSTANT
template<typename SymType>
struct is_rational_constant
{
  static constexpr bool value = false;
};

template<int64_t N, int64_t D>
struct is_rational_constant<RationalConstant<N,D>>
{
  static constexpr bool value = true;
};

template<typename SymType>
constexpr bool is_rational_constant_v = is_rational_constant<SymType>::value;

// IS SYMBOL
//? Although std::is_same could be used here instead, there will be multiple symbol types in the future
template<typename SymType>
struct is_symbol
{
  static constexpr bool value = false;
};

template<>
struct is_symbol<Symbol>
{
  static constexpr bool value = true;
};

template<typename SymType>
constexpr bool is_symbol_v = is_symbol<SymType>::value;

// IS NEGATION
template<typename SymType>
struct is_negation
{
  static constexpr bool value = false;
};

template<typename SymType>
struct is_negation<Negation<SymType>>
{
  static constexpr bool value = true;
};

template<typename SymType>
constexpr bool is_negation_v = is_negation<SymType>::value;

//TODO requires pow
// // IS RATIONAL POWER
// template<typename SymType>
// struct is_rational_power
// {
//   static constexpr bool value = false;
// };

// template<typename SymType, int64_t N, int64_t D>
// struct is_rational_power<FractionalPower<SymType,N,D>>
// {
//   static constexpr bool value = true;
// };

// template<typename SymType>
// constexpr bool is_rational_power_v = is_rational_power<SymType>::value;

// IS SUM
template<typename SymType>
struct is_sum
{
  static constexpr bool value = false;
};

template<typename... Syms>
struct is_sum<TupleSum<Syms...>>
{
  static constexpr bool value = true;
};

template<typename SymType>
constexpr bool is_sum_v = is_sum<SymType>::value;

// IS PRODUCT
template<typename SymType>
struct is_product
{
  static constexpr bool value = false;
};

template<typename... Syms>
struct is_product<TupleProduct<Syms...>>
{
  static constexpr bool value = true;
};

template<typename SymType>
constexpr bool is_product_v = is_product<SymType>::value;


} // Symbolic namespace
#endif