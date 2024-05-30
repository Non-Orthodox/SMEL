#ifndef SYMBOLIC_INCLUDE_SUM_OPERATOR_HPP
#define SYMBOLIC_INCLUDE_SUM_OPERATOR_HPP

#include "type_deductions.hpp"
#include "symbolic_base.hpp"
#include "constants.hpp"
#include "concepts.hpp"
#include "negation.hpp"
#include "sum.hpp"
#include "product.hpp"
#include "sincos.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename Sym1, typename Sym2>
struct SumCombinable
{
  static constexpr bool value = 
    is_zero_v<Sym1>
    || is_zero_v<Sym2>
    || (is_negation_v<Sym1> && is_negation_v<Sym2>)
    || is_same_v<Sym1,Sym2>;
};


// Combining constants
template<typename T>
struct SumCombinable<T, RuntimeConstant<T>>
{
  static constexpr bool value = true;
};

template<typename T>
struct SumCombinable<RuntimeConstant<T>, T>
{
  static constexpr bool value = true;
};

template<int64_t N1, int64_t D1, int64_t N2, int64_t D2>
struct SumCombinable<RationalConstant<N1,D1>, RationalConstant<N2,D2>>
{
  static constexpr bool value = true;
};

template<typename T, T Value1, T Value2>
struct SumCombinable<Constant<T,Value1>,Constant<T,Value2>>
{
  static constexpr bool value = true;
};

template<typename T>
struct SumCombinable<RuntimeConstant<T>,RuntimeConstant<T>>
{
  static constexpr bool value = true;
};

// Factoring
template<typename Sym1, typename Sym2>
struct SumCombinable<Negation<Sym1>,Negation<Sym2>>
{
  static constexpr bool value = true;
};

//TODO requires pow
// // Trig
// template<typename SymType>
// struct SumCombinable<
//     FractionalPower<Cosine<SymType>,2,1>,
//     FractionalPower<Sine<SymType>,2,1>
//   >
// {
//   static constexpr bool value = true;
// };

// template<typename SymType>
// struct SumCombinable<
//     FractionalPower<Sine<SymType>,2,1>,
//     FractionalPower<Cosine<SymType>,2,1>
//   >
// {
//   static constexpr bool value = true;
// };


template<class... Sym1, class... Sym2>
struct is_same<TupleSum<Sym1...>,TupleSum<Sym2...>>
{
  template<std::size_t N, size_t M>
  static constexpr bool find_value_impl()
  {
    if constexpr (is_same_v<NthTypeOf<N,Sym1...>,NthTypeOf<M,Sym2...>>) {
      if constexpr ((N+1) < sizeof...(Sym1)) {
        return find_value_impl<N+1,0>();
      }
      else {
        return true;
      }
    }
    else {
      if constexpr ((M+1) < sizeof...(Sym2)) {
        return find_value_impl<N,M+1>();
      }
      else {
        return false;
      }
    }
  }

  static constexpr bool value = find_value_impl<0,0>();
};


// Helper Functions
template<std::size_t N, typename Sym1, typename... Sym2>
constexpr auto FactorCheck(const SymbolicBase<Sym1>& expr1, const TupleProduct<Sym2...>& expr2)
{
  if constexpr (is_same_v<Sym1, NthTypeOf<N,Sym2...>>) {
    return (One<>() + expr2.template Without<N>()) * expr1.derived();
  }
  else if constexpr (N == 0) {
    return TupleSum<Sym1, TupleProduct<Sym2...>>(expr1.derived(), expr2);
  }
  else {
    return FactorCheck<N-1, Sym1, Sym2...>(expr1, expr2);
  }
}

template<typename Sym1, typename... Sym2>
constexpr auto Factor(const SymbolicBase<Sym1>& expr1, const TupleProduct<Sym2...>& expr2)
{
  return FactorCheck<sizeof...(Sym2) - 1>(expr1, expr2);
}


template<std::size_t N, std::size_t M, typename... Sym1, typename... Sym2, std::size_t... I1, std::size_t... I2>
auto FactorCheck(
  const TupleProduct<Sym1...>& expr1,
  const TupleProduct<Sym2...>& expr2,
  const std::index_sequence<I1...> i1,
  const std::index_sequence<I2...> i2)
{
  if constexpr (is_same_v<NthTypeOf<N,Sym1...>,NthTypeOf<M,Sym2...>>) {
    if constexpr ((N+1) < sizeof...(Sym1)) {
      return FactorCheck<N+1,0>(expr1, expr2, std::index_sequence<I1..., N>(), std::index_sequence<I2..., M>());
    }
    else {
      return expr1.template Subset<I1..., N>() *
        ( expr1.template Without<I1..., N>() + expr2.template Without<I2..., M>() );
    }
  }
  else {
    if constexpr ((M+1) < sizeof...(Sym2)) {
      return FactorCheck<N,M+1>(expr1, expr2, i1, i2);
    }
    else {
      if constexpr ((N+1) < sizeof...(Sym1)) {
        return FactorCheck<N+1,0>(expr1, expr2, i1, i2);
      }
      else {
        return expr1.template Subset<I1...>() *
          ( expr1.template Without<I1...>() + expr2.template Without<I2...>() );
      }
    }
  }
}

template<std::size_t N, std::size_t M, typename... Sym1, typename... Sym2>
constexpr auto FactorCheck(const TupleProduct<Sym1...>& expr1, const TupleProduct<Sym2...>& expr2)
{
  // change M for a given N
  if constexpr (is_same_v<NthTypeOf<N, Sym1...>, NthTypeOf<M, Sym2...>>) {
    if constexpr ((N+1) < sizeof...(Sym1)) {
      return FactorCheck<N+1,0>(expr1, expr2, std::index_sequence<N>(), std::index_sequence<M>());
    }
    else {
      return get<N>(expr1) * ( expr1.template Without<N>() + expr2.template Without<M>() );
    }
  }
  else {
    if constexpr ((M+1) < sizeof...(Sym2)) {
      return FactorCheck<N,M+1>(expr1, expr2);
    }
    else {
      if constexpr ((N+1) < sizeof...(Sym1)) {
        return FactorCheck<N+1,0>(expr1, expr2);
      }
      else {
        return TupleSum(expr1, expr2);
      }
    }
  }
}

template<typename... Sym1, typename... Sym2>
constexpr auto Factor(const TupleProduct<Sym1...>& expr1, const TupleProduct<Sym2...>& expr2)
{
  return FactorCheck<0,0>(expr1,expr2);
}


// General Addition
template<class Sym1, class Sym2>
constexpr auto
operator+(const SymbolicBase<Sym1>& expr1, const SymbolicBase<Sym2>& expr2)
{
  if constexpr (is_zero_v<Sym1>) {
    return expr2.derived();
  }
  else if constexpr (is_zero_v<Sym2>) {
    return expr1.derived();
  }
  else if constexpr (is_negation_v<Sym1> && is_negation_v<Sym2>) {
    return -(expr1.derived().Negate() + expr2.derived().Negate());
  }
  else if constexpr (is_same_v<Sym1,Sym2>) {
    return Int<2>() * expr1.derived();
  }
  // Factoring attempts start here
  else if constexpr (is_product_v<Sym1> && !is_product_v<Sym2>) {
    // if constexpr (is_sum_v<Sym2>) {

    // } else {

    // }
    return Factor(expr2, expr1.derived());
  }
  else if constexpr (is_product_v<Sym2> && !is_product_v<Sym1>) {
    // if constexpr (is_sum_v<Sym1>) {

    // } else {

    // }
    return Factor(expr1, expr2.derived());
  }
  else if constexpr (is_product_v<Sym1> && is_product_v<Sym2>) {
    return Factor(expr1.derived(), expr2.derived());
  }
  else {
    return TupleSum<Sym1,Sym2>(expr1.derived(), expr2.derived());
  }
}


// Sum of TupleSums
template<std::size_t N, class Sym1, class... Sym2>
constexpr auto extended_sum_impl(const SymbolicBase<Sym1>& expr1, const TupleSum<Sym2...>& expr2)
{
  if constexpr (SumCombinable<Sym1, NthTypeOf<N, Sym2...>>::value) {
    return expr2.template ModifyElement<N,Sym1>(expr1.derived());
  }
  else if constexpr (N == 0) {
    return ExtendTupleSum(expr1.derived(), expr2);
  }
  else {
    return extended_sum_impl<N-1,Sym1,Sym2...>(expr1, expr2);
  }
}

template<class... Sym1, class Sym2>
constexpr auto
operator+(const TupleSum<Sym1...>& expr1, const SymbolicBase<Sym2>& expr2)
{
  return extended_sum_impl<sizeof...(Sym1)-1>(expr2.derived(), expr1);
}

template<class... Sym1, class Sym2>
constexpr auto
operator+(const SymbolicBase<Sym2>& expr1, const TupleSum<Sym1...>& expr2)
{
  return extended_sum_impl<sizeof...(Sym1)-1>(expr1.derived(), expr2);
}


template<std::size_t N, std::size_t M, std::size_t... I, class... Sym1, class... Sym2>
constexpr auto merge_sums_impl2(const TupleSum<Sym1...>& expr1, const TupleSum<Sym2...>& expr2)
{
  if constexpr (SumCombinable< NthTypeOf<N, Sym1...>, NthTypeOf<M, Sym2...> >::value) {
    if constexpr ((N+1) < sizeof...(Sym1)) {
      return merge_sums_impl2<N+1, 0, I..., M>(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2);
    }
    else {
      return ExtendTupleSum(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2.template Without<I..., M>()
        );
    }
  }
  else {
    if constexpr ((M+1) < sizeof...(Sym2)) {
      return merge_sums_impl2<N,M+1,I...>(expr1, expr2);
    }
    else {
      if constexpr ((N+1) < sizeof...(Sym1)) {
        return merge_sums_impl2<N+1,0,I...>(expr1, expr2);
      }
      else {
        return ExtendTupleSum(expr1, expr2.template Without<I...>());
      }
    }
  }
}

template<std::size_t N, std::size_t M, class... Sym1, class... Sym2>
constexpr auto merge_sums_impl1(const TupleSum<Sym1...>& expr1, const TupleSum<Sym2...>& expr2)
{
  if constexpr (SumCombinable< NthTypeOf<N, Sym1...>, NthTypeOf<M, Sym2...> >::value) {
    if constexpr ((N+1) < sizeof...(Sym1)) {
      return merge_sums_impl2<N+1, 0, M>(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2);
    }
    else {
      return ExtendTupleSum(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2.template Without<M>()
        );
    }
  }
  else {
    if constexpr ((M+1) < sizeof...(Sym2)) {
      return merge_sums_impl1<N,M+1>(expr1, expr2);
    }
    else {
      if constexpr ((N+1) < sizeof...(Sym1)) {
        return merge_sums_impl1<N+1,0>(expr1, expr2);
      }
      else {
        return ExtendTupleSum(expr1, expr2);
      }
    }
  }
}

template<class... Sym1, class... Sym2>
constexpr auto operator+(const TupleSum<Sym1...>& expr1, const TupleSum<Sym2...>& expr2)
{
  if constexpr (is_same_v<TupleSum<Sym1...>,TupleSum<Sym2...>>) {
    return Int<2>() * expr1;
  } else {
    return merge_sums_impl1<0,0>(expr1,expr2);
  }
  // return TupleSum(expr1,expr2);
}


// Biased general expression with integral type
template<typename SymType, IntegralConstant T>
constexpr auto operator+(const T bias, const SymbolicBase<SymType>& expr)
{
  return RuntimeConstant<T>(bias) + expr.derived();
}

template<typename SymType, IntegralConstant T>
constexpr auto operator+(const SymbolicBase<SymType>& expr, const T bias)
{
  return RuntimeConstant<T>(bias) + expr.derived();
}

// Adding Constants
template<IntegralConstant T>
constexpr auto operator+(const T c1, const RuntimeConstant<T>& c2)
{
  return RuntimeConstant<T>(c1 + c2.Value());
}

template<IntegralConstant T>
constexpr auto operator+(const RuntimeConstant<T>& c1, const T c2)
{
  return RuntimeConstant<T>(c1.Value() + c2);
}


//TODO requires pow
// // Trig
// template<typename SymType>
// constexpr One<> operator+(
//   const FractionalPower<Cosine<SymType>,2,1>& cos,
//   const FractionalPower<Sine<SymType>,2,1>& sin
// )
// {
//   return One<>();
// }

// template<typename SymType>
// constexpr One<> operator+(
//   const FractionalPower<Sine<SymType>,2,1>& sin,
//   const FractionalPower<Cosine<SymType>,2,1>& cos
// )
// {
//   return One<>();
// }



} // Symbolic namespace
#endif