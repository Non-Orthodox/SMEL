#ifndef SYMBOLIC_INCLUDE_SUM_HPP
#define SYMBOLIC_INCLUDE_SUM_HPP

#include <tuple>
#include <utility>

#include "symbolic_base.hpp"
#include "constants.hpp"
#include "concepts.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<class... ExprTypes>
class TupleSum;

template<class TupleType, std::size_t... I>
constexpr auto MakeTupleSum(const TupleType& expr_tuple, const std::index_sequence<I...>)
{
  if constexpr (sizeof...(I) == 1) {
    return std::get<I...>(expr_tuple);
  } else {
    return TupleSum(std::get<I>(expr_tuple)...);
  }
}

template<class... ExprTypes>
constexpr auto MakeTupleSum(const std::tuple<ExprTypes...> expr_tuple)
{
  return MakeTupleSum(expr_tuple, std::make_index_sequence<sizeof...(ExprTypes)>());
}


template<class... ExprTypes>
class TupleSum : public SymbolicBase< TupleSum<ExprTypes...> >
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
      return std::get<N>(exprs_).Evaluate(input) + RecursiveEvaluate<N-1>(input);
    }
  }

  template<std::size_t N>
  constexpr auto RecursiveDerivative() const
  {
    if constexpr (N == 0) {
      return std::get<0>(exprs_).Derivative();
    }
    else {
      return RecursiveDerivative<N-1>() + std::get<N>(exprs_).Derivative();
    }
  }

  template<std::size_t N>
  std::string sub_str() const
  {
    if constexpr (N == 0) {
      return std::get<0>(exprs_).str();
    }
    else {
      return sub_str<N-1>() + " + " + std::get<N>(exprs_).str();
    }
  }

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = (ExprTypes::is_dynamic || ...);

  constexpr TupleSum(const ExprTypes&... exprs) : exprs_{exprs...}
  {
    static_assert(sizeof...(ExprTypes) > 1, "TupleSum must have more than one expression");
  }

  template<class SymType, class... Factors>
  friend constexpr auto ExtendTupleSum(const TupleSum<Factors...>& sum, const SymbolicBase<SymType>& factor);

  template<class SymType, class... Factors>
  friend constexpr auto ExtendTupleSum(const SymbolicBase<SymType>& factor, const TupleSum<Factors...>& sum);

  template<class... Sym1, class... Sym2>
  friend constexpr auto ExtendTupleSum(const TupleSum<Sym1...>& sum1, const TupleSum<Sym2...>& sum2);

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
  friend constexpr auto get(const TupleSum<Exprs...>& sum);

  template<std::size_t N, class SymType>
  constexpr auto ModifyElement(const SymbolicBase<SymType>& expr) const
  {
    static_assert(N < sizeof...(ExprTypes), "TupleSum::ModifyElement called with invalid index");
    if constexpr (N == (sizeof...(ExprTypes)-1)) {
      return MakeTupleSum(std::tuple_cat(
        tuple_slice<0,N>(exprs_),
        std::make_tuple(std::get<N>(exprs_) + expr.derived())
      ));
    }
    else if constexpr (N == 0) {
      return MakeTupleSum(std::tuple_cat(
        std::make_tuple(std::get<N>(exprs_) + expr.derived()),
        tuple_slice<N+1, sizeof...(ExprTypes)>(exprs_)
      ));
    }
    else {
      return MakeTupleSum(std::tuple_cat(
        tuple_slice<0,N>(exprs_),
        std::make_tuple(std::get<N>(exprs_) + expr.derived()),
        tuple_slice<N+1, sizeof...(ExprTypes)>(exprs_)
      ));
    }
  }

  // template<std::size_t I, std::size_t... Is>
  // auto Without() const
  // {
  //   static_assert(
  //     (I < sizeof...(ExprTypes)) &&
  //     ((Is < sizeof...(ExprTypes)) && ...),
  //     "TupleSum::Without<Is...> has at least one index out of range");
    
  //   return MakeTupleSum(exprs_, index_sequence_without<sizeof...(ExprTypes), I, Is...>());
  // }

  template<std::size_t... Is>
  constexpr auto Without() const
  {
    static_assert(
      ((Is < sizeof...(ExprTypes)) && ...),
      "TupleSum::Without<Is...> has at least one index out of range");
    
    return MakeTupleSum(exprs_, index_sequence_without<sizeof...(ExprTypes), Is...>());
  }

  template<std::size_t... Is>
  constexpr auto Subset() const
  {
    return MakeTupleSum(exprs_, std::index_sequence<Is...>());
  }

};


template<std::size_t N, class... Exprs>
constexpr auto get(const TupleSum<Exprs...>& sum)
{
  return std::get<N>(sum.exprs_);
}

template<class TupleType, class SymType, std::size_t... I>
constexpr auto ExtendTupleSumImpl(const TupleType& expr_tuple, const SymbolicBase<SymType>& factor, const std::index_sequence<I...>)
{
  return TupleSum(std::get<I>(expr_tuple)..., factor.derived());
}

template<class SymType, class... Factors>
constexpr auto ExtendTupleSum(const TupleSum<Factors...>& sum, const SymbolicBase<SymType>& factor)
{
  return ExtendTupleSumImpl(sum.exprs_, factor.derived(), std::make_index_sequence<sizeof...(Factors)>());
}


template<class TupleType, class SymType, std::size_t... I>
constexpr auto ExtendTupleSumImpl(const SymbolicBase<SymType>& factor, const TupleType& expr_tuple, const std::index_sequence<I...>)
{
  return TupleSum(factor.derived(), std::get<I>(expr_tuple)...);
}

template<class SymType, class... Factors>
constexpr auto ExtendTupleSum(const SymbolicBase<SymType>& factor, const TupleSum<Factors...>& sum)
{
  return ExtendTupleSumImpl(factor.derived(), sum.exprs_, std::make_index_sequence<sizeof...(Factors)>());
}


template<class TupleType1, class TupleType2, std::size_t... I1, std::size_t... I2>
constexpr auto ExtendTupleSumImpl(
    const TupleType1& expr_tuple1,
    const TupleType2& expr_tuple2,
    const std::index_sequence<I1...>,
    const std::index_sequence<I2...>)
{
  return TupleSum(std::get<I1>(expr_tuple1)..., std::get<I2>(expr_tuple2)...);
}

template<class... Sym1, class... Sym2>
constexpr auto ExtendTupleSum(const TupleSum<Sym1...>& sum1, const TupleSum<Sym2...>& sum2)
{
  return ExtendTupleSumImpl(sum1.exprs_, sum2.exprs_,
    std::make_index_sequence<sizeof...(Sym1)>(),
    std::make_index_sequence<sizeof...(Sym2)>());
}


} // Symbolic namespace
#endif