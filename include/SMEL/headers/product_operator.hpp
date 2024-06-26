#ifndef SYMBOLIC_INCLUDE_PRODUCT_OPERATOR_HPP
#define SYMBOLIC_INCLUDE_PRODUCT_OPERATOR_HPP

#include "type_deductions.hpp"
#include "symbolic_base.hpp"
#include "concepts.hpp"
#include "metaprogramming.hpp"
#include "product.hpp"
#include "constants.hpp"
#include "negation.hpp"
// #include "pow.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename Sym1, typename Sym2>
struct ProductCombinable
{
  static constexpr bool value = 
    is_zero_v<Sym1>
    || is_zero_v<Sym2>
    || is_one_v<Sym1>
    || is_one_v<Sym2>
    || is_same_v<Sym1,Sym2>;
};


// Constant combos
template<typename T1, T1 Value1, typename T2, T2 Value2>
struct ProductCombinable<Constant<T1,Value1>,Constant<T2,Value2>>
{
  static constexpr bool value = true;
};

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralInt T3, T3 N2, IntegralInt T4, T4 D2>
struct ProductCombinable<IntegerFraction<T1,N1,T2,D1>, IntegerFraction<T3,N2,T4,D2>>
{
  static constexpr bool value = true;
};

template<IntegralConstant T1, T1 N1, IntegralInt T2, T2 N2,
    IntegralInt T3, T3 D2>
struct ProductCombinable<Constant<T1,N1>, IntegerFraction<T2,N2,T3,D2>>
{
  static constexpr bool value = true;
};

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralConstant T3, T3 N2>
struct ProductCombinable<IntegerFraction<T1,N1,T2,D1>, Constant<T3,N2>>
{
  static constexpr bool value = true;
};

template<typename T>
struct ProductCombinable<RuntimeConstant<T>,RuntimeConstant<T>>
{
  static constexpr bool value = true;
};


// Combining exponents
template<typename Sym1, typename Sym2>
struct ProductCombinable< Exponential<Sym1,Sym2>, SymbolicBase<Sym1> >
{
  static constexpr bool value = true;
};

template<typename Sym1, typename Sym2>
struct ProductCombinable < SymbolicBase<Sym1>, Exponential<Sym1,Sym2> >
{
  static constexpr bool value = true;
};

template<typename Sym1, typename Sym2, typename Sym3>
struct ProductCombinable < Exponential<Sym1,Sym2>&, Exponential<Sym1,Sym3> >
{
  static constexpr bool value = true;
};


template<class... Sym1, class... Sym2>
struct is_same<TupleProduct<Sym1...>,TupleProduct<Sym2...>>
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

//TODO power re-distribution

template<std::size_t N, class Sym1, class... Sym2>
constexpr auto extended_product_impl(const SymbolicBase<Sym1>& expr1, const TupleProduct<Sym2...>& expr2)
{
  if constexpr (ProductCombinable<Sym1, NthTypeOf<N, Sym2...>>::value) {
    return expr2.template ModifyElement<N,Sym1>(expr1.derived());
  }
  else if constexpr (N+1 < sizeof...(Sym2)) {
    return extended_product_impl<N+1>(expr1, expr2);
  }
  else {
    return ExtendTupleProduct(expr1.derived(), expr2);
  }
}

// template<class... Sym1, class Sym2>
// auto operator*(const TupleProduct<Sym1...>& expr1, const SymbolicBase<Sym2>& expr2)
// {
//   return extended_product_impl<0>(expr2.derived(), expr1);
// }

// template<class... Sym1, class Sym2>
// auto operator*(const SymbolicBase<Sym2>& expr1, const TupleProduct<Sym1...>& expr2)
// {
//   return extended_product_impl<0>(expr1.derived(), expr2);
// }

//TODO x * (1/x) and x^N * (1/x^M) in general function
//TODO Product * Quotient functions
//TODO Sum * Quotient functions (sum * (f / g) -> (sum * f) / g)
template<class Sym1, class Sym2>
constexpr auto
operator*(const SymbolicBase<Sym1>& expr1, const SymbolicBase<Sym2>& expr2)
{
  if constexpr (is_zero_v<Sym1> || is_zero_v<Sym2>) {
    return Zero<>();
  }
  else if constexpr (is_one_v<Sym1>) {
    return expr2.derived();
  }
  else if constexpr (is_one_v<Sym2>) {
    return expr1.derived();
  }
  else if constexpr (is_negation_v<Sym1>) {
    if constexpr (is_negation_v<Sym2>) {
      return expr1.derived().Negate() * expr2.derived().Negate();
    } else {
      return -(expr1.derived().Negate() * expr2.derived());
    }
  }
  else if constexpr (is_negation_v<Sym2>) {
    return -(expr1.derived() * expr2.derived().Negate());
  }
  else if constexpr (is_same_v<Sym1,Sym2>) {
    return expr1.derived() ^ Int<2>();
  }
  else if constexpr (is_negative_constant_v<Sym1>) {
    return -( (-expr1.derived()) * expr2.derived() );
  }
  else if constexpr (is_negative_constant_v<Sym2>) {
    return -( expr1.derived() * (-expr2.derived()) );
  }
  else if constexpr (is_product_v<Sym1>) {
    return extended_product_impl<0>(expr2, expr1.derived());
  }
  else if constexpr (is_product_v<Sym2>) {
    return extended_product_impl<0>(expr1, expr2.derived());
  }
  else {
    return TupleProduct<Sym1,Sym2>(expr1.derived(), expr2.derived());
  }
}


// Multiplication of TupleProducts
template<std::size_t N, std::size_t M, std::size_t... I, class... Sym1, class... Sym2>
constexpr auto 
merge_products_impl2(const TupleProduct<Sym1...>& expr1, const TupleProduct<Sym2...>& expr2)
{
  if constexpr (ProductCombinable< NthTypeOf<N, Sym1...>, NthTypeOf<M, Sym2...> >::value) {
    if constexpr ((N+1) < sizeof...(Sym1)) {
      return merge_products_impl2<N+1, 0, I..., M>(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2);
    }
    else {
      return ExtendTupleProduct(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2.template Without<I..., M>()
        );
    }
  }
  else {
    if constexpr ((M+1) < sizeof...(Sym2)) {
      return merge_products_impl2<N,M+1,I...>(expr1, expr2);
    }
    else {
      if constexpr ((N+1) < sizeof...(Sym1)) {
        return merge_products_impl2<N+1,0,I...>(expr1, expr2);
      }
      else {
        return ExtendTupleProduct(expr1, expr2.template Without<I...>());
      }
    }
  }
}

template<std::size_t N, std::size_t M, class... Sym1, class... Sym2>
constexpr auto
merge_products_impl1(const TupleProduct<Sym1...>& expr1, const TupleProduct<Sym2...>& expr2)
{
  if constexpr (ProductCombinable< NthTypeOf<N, Sym1...>, NthTypeOf<M, Sym2...> >::value) {
    if constexpr ((N+1) < sizeof...(Sym1)) {
      return merge_products_impl2<N+1, 0, M>(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2);
    }
    else {
      return ExtendTupleProduct(
        expr1.template ModifyElement<N>(get<M>(expr2)), expr2.template Without<M>()
        );
    }
  }
  else {
    if constexpr ((M+1) < sizeof...(Sym2)) {
      return merge_products_impl1<N,M+1>(expr1, expr2);
    }
    else {
      if constexpr ((N+1) < sizeof...(Sym1)) {
        return merge_products_impl1<N+1,0>(expr1, expr2);
      }
      else {
        return ExtendTupleProduct(expr1, expr2);
      }
    }
  }
}

template<class... Sym1, class... Sym2>
constexpr auto
operator*(const TupleProduct<Sym1...>& expr1, const TupleProduct<Sym2...>& expr2)
{
  if constexpr (is_same_v<TupleProduct<Sym1...>,TupleProduct<Sym2...>>) {
    return pow<2>(expr1);
  } else {
    return merge_products_impl1<0,0>(expr1,expr2);
  }
  // return TupleProduct(expr1,expr2);
}


// Multiplying by integral types
template<IntegralConstant ConstantType, typename T>
constexpr Zero<> operator*(const ConstantType scale, const Zero<T>& zero)
{
  return Zero<>();
}

template<IntegralConstant ConstantType, typename T>
constexpr Zero<> operator*(const Zero<T>& zero, const ConstantType scale)
{
  return Zero<>();
}

template<IntegralConstant ConstantType, typename T>
constexpr RuntimeConstant<ConstantType> operator*(const ConstantType scale, const One<T>& one)
{
  return RuntimeConstant<ConstantType>(scale);
}

template<IntegralConstant ConstantType, typename T>
constexpr RuntimeConstant<ConstantType> operator*(const One<T>& one, const ConstantType scale)
{
  return RuntimeConstant<ConstantType>(scale);
}


// Combining Powers
template<typename Sym1, typename Sym2>
constexpr auto
operator*(const Exponential<Sym1,Sym2>& expr, const SymbolicBase<Sym1>&)
{
  return expr.Base() ^ (expr.Exponent() + One<>());
}

template<typename Sym1, typename Sym2>
constexpr auto
operator*(const SymbolicBase<Sym1>&, const Exponential<Sym1,Sym2>& expr)
{
  return expr.Base() ^ (expr.Exponent() + One<>());
}

template<typename Sym1, typename Sym2, typename Sym3>
constexpr auto
operator*(const Exponential<Sym1,Sym2>& expr1, const Exponential<Sym1,Sym3>& expr2)
{
  return expr1.Base() ^ (expr1.Exponent() + expr2.Exponent());
}


// Scaling by integral type
template<class SymType, IntegralConstant T>
constexpr auto
operator*(const T c, const SymbolicBase<SymType>& expr)
{
  return RuntimeConstant<T>(c) * expr.derived();
}

template<class SymType, IntegralConstant T>
constexpr auto
operator*(const SymbolicBase<SymType>& expr, const T c)
{
  return RuntimeConstant<T>(c) * expr.derived();
}


} // Symbolic namespace
#endif