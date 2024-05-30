#ifndef SYMBOLIC_INCLUDE_PRODUCT_HPP
#define SYMBOLIC_INCLUDE_PRODUCT_HPP

#include <tuple>
#include <utility>

#include "metaprogramming.hpp"
#include "symbolic_base.hpp"
#include "constants.hpp"
#include "sum.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<class... ExprTypes>
class TupleProduct;

template<class TupleType, std::size_t... I>
constexpr auto MakeTupleProduct(const TupleType& expr_tuple, const std::index_sequence<I...>)
{
  if constexpr (sizeof...(I) == 1) {
    return std::get<I...>(expr_tuple);
  } else {
    return TupleProduct(std::get<I>(expr_tuple)...);
  }
}

template<class... ExprTypes>
constexpr auto MakeTupleProduct(const std::tuple<ExprTypes...> expr_tuple)
{
  return MakeTupleProduct(expr_tuple, std::make_index_sequence<sizeof...(ExprTypes)>());
}


template<class... ExprTypes>
class TupleProduct : public SymbolicBase< TupleProduct<ExprTypes...> >
{
private:
  std::tuple<typename BranchType<ExprTypes>::type...> exprs_;

  template<std::size_t N, typename FloatType>
  constexpr FloatType RecursiveEvaluate(const FloatType& input) const
  {
    if constexpr (N == 0) {
      return std::get<0>(exprs_).Evaluate(input);
    }
    else {
      return std::get<N>(exprs_).Evaluate(input) * RecursiveEvaluate<N-1>(input);
    }
  }

  template<std::size_t N>
  constexpr auto RecursiveDerivative() const
  {
    if constexpr (N == 1) {
      return (std::get<1>(exprs_) * std::get<0>(exprs_).Derivative())
        + (std::get<1>(exprs_).Derivative() * std::get<0>(exprs_));
    }
    else {
      return (std::get<N>(exprs_) * RecursiveDerivative<N-1>())
        + ( std::get<N>(exprs_).Derivative() * MakeTupleProduct(exprs_, std::make_index_sequence<N>()) );
    }
  }

  template<std::size_t N>
  std::string sub_str() const
  {
    if constexpr (N == 0) {
      return std::get<0>(exprs_).str();
    }
    else {
      return sub_str<N-1>() + " * " + std::get<N>(exprs_).str();
    }
  }

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = (ExprTypes::is_dynamic || ...);

  constexpr TupleProduct(const ExprTypes&... exprs) : exprs_{exprs...}
  {
    static_assert(sizeof...(ExprTypes) > 1, "TupleProduct must have more than one expression");
  }

  template<class SymType, class... Factors>
  friend constexpr auto ExtendTupleProduct(const TupleProduct<Factors...>& prod, const SymbolicBase<SymType>& factor);

  template<class SymType, class... Factors>
  friend constexpr auto ExtendTupleProduct(const SymbolicBase<SymType>& factor, const TupleProduct<Factors...>& prod);

  template<class... Sym1, class... Sym2>
  friend constexpr auto ExtendTupleProduct(const TupleProduct<Sym1...>& prod1, const TupleProduct<Sym2...>& prod2);

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  {
    return RecursiveEvaluate<(sizeof...(ExprTypes))-1, FloatType>(input);
  }

  constexpr auto Derivative() const
  { 
    return RecursiveDerivative<(sizeof...(ExprTypes))-1>();
  }

  std::string str() const
  {
    return "(" + sub_str<(sizeof...(ExprTypes))-1>() + ")";
  }

  template<std::size_t N, class... Exprs>
  friend constexpr auto get(const TupleProduct<Exprs...>& prod);

  template<std::size_t N, class SymType>
  constexpr auto ModifyElement(const SymbolicBase<SymType>& expr) const
  {
    static_assert(N < sizeof...(ExprTypes), "TupleProduct::ModifyElement called with invalid index");
    if constexpr (N == (sizeof...(ExprTypes)-1)) {
      return MakeTupleProduct(std::tuple_cat(
        tuple_slice<0,N>(exprs_),
        std::make_tuple(std::get<N>(exprs_) * expr.derived())
      ));
    }
    else if constexpr (N == 0) {
      return MakeTupleProduct(std::tuple_cat(
        std::make_tuple(std::get<N>(exprs_) * expr.derived()),
        tuple_slice<N+1, sizeof...(ExprTypes)>(exprs_)
      ));
    }
    else {
      return MakeTupleProduct(std::tuple_cat(
        tuple_slice<0,N>(exprs_),
        std::make_tuple(std::get<N>(exprs_) * expr.derived()),
        tuple_slice<N+1, sizeof...(ExprTypes)>(exprs_)
      ));
    }
  }

  // template<std::size_t I>
  // auto Without() const
  // {
  //   static_assert(I < sizeof...(ExprTypes), "TupleProduct::Without<I> index (I) out of range");
  //   if constexpr (sizeof...(ExprTypes) == 2) {
  //     return std::get<(I==0) ? 1 : 0>(exprs_);
  //   }
  //   else {
  //     return MakeTupleProduct(
  //         exprs_,
  //         index_sequence_cat(make_index_range<0,I>(), make_index_range<I+1,sizeof...(ExprTypes)>())
  //       );
  //   }
  // }

  template<std::size_t I, std::size_t... Is>
  constexpr auto Without() const
  {
    static_assert(
      (I < sizeof...(ExprTypes)) &&
      ((Is < sizeof...(ExprTypes)) && ...),
      "TupleProduct::Without<Is...> has at least one index out of range");
    
    return MakeTupleProduct(exprs_, index_sequence_without<sizeof...(ExprTypes), I, Is...>());
  }

  template<std::size_t... Is>
  constexpr auto Subset() const
  {
    return MakeTupleProduct(exprs_, std::index_sequence<Is...>());
  }

};


template<std::size_t N, class... Exprs>
constexpr auto get(const TupleProduct<Exprs...>& prod)
{
  return std::get<N>(prod.exprs_);
}


template<class TupleType, class SymType, std::size_t... I>
constexpr auto
ExtendTupleProductImpl(const TupleType& expr_tuple, const SymbolicBase<SymType>& factor, const std::index_sequence<I...>)
{
  return TupleProduct(std::get<I>(expr_tuple)..., factor.derived());
}

template<class SymType, class... Factors>
constexpr auto
ExtendTupleProduct(const TupleProduct<Factors...>& prod, const SymbolicBase<SymType>& factor)
{
  return ExtendTupleProductImpl(prod.exprs_, factor.derived(), std::make_index_sequence<sizeof...(Factors)>());
}


template<class TupleType, class SymType, std::size_t... I>
constexpr auto
ExtendTupleProductImpl(const SymbolicBase<SymType>& factor, const TupleType& expr_tuple, const std::index_sequence<I...>)
{
  return TupleProduct(factor.derived(), std::get<I>(expr_tuple)...);
}

template<class SymType, class... Factors>
constexpr auto
ExtendTupleProduct(const SymbolicBase<SymType>& factor, const TupleProduct<Factors...>& prod)
{
  return ExtendTupleProductImpl(factor.derived(), prod.exprs_, std::make_index_sequence<sizeof...(Factors)>());
}


template<class TupleType1, class TupleType2, std::size_t... I1, std::size_t... I2>
constexpr auto ExtendTupleProductImpl(
    const TupleType1& expr_tuple1,
    const TupleType2& expr_tuple2,
    const std::index_sequence<I1...>,
    const std::index_sequence<I2...>)
{
  return TupleProduct(std::get<I1>(expr_tuple1)..., std::get<I2>(expr_tuple2)...);
}

template<class... Sym1, class... Sym2>
constexpr auto
ExtendTupleProduct(const TupleProduct<Sym1...>& prod1, const TupleProduct<Sym2...>& prod2)
{
  return ExtendTupleProductImpl(prod1.exprs_, prod2.exprs_,
    std::make_index_sequence<sizeof...(Sym1)>(),
    std::make_index_sequence<sizeof...(Sym2)>());
}


} // Symbolic namespace
#endif