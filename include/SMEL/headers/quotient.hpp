#ifndef SYMBOLIC_INCLUDE_QUOTIENT_HPP
#define SYMBOLIC_INCLUDE_QUOTIENT_HPP

#include "symbolic_base.hpp"

namespace SYMBOLIC_NAMESPACE_NAME {


template<typename NumExpr, typename DenExpr>
class Quotient : public SymbolicBase< Quotient<NumExpr,DenExpr> >
{
private:
  typename std::conditional_t<NumExpr::is_leaf, const NumExpr&, const NumExpr> num_;
  typename std::conditional_t<DenExpr::is_leaf, const DenExpr&, const DenExpr> den_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = NumExpr::is_dynamic || DenExpr::is_dynamic;

  constexpr Quotient(const NumExpr& num, const DenExpr& den)
    : num_{num}, den_{den}
  {
    static_assert(!is_zero_v<DenExpr>, "Quotient cannot have zero as denominator");
  }

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  { 
    return num_.Evaluate(input) / den_.Evaluate(input);
  }

  constexpr auto Derivative() const
  {
    return ( (num_.Derivative() * den_) - (num_ * den_.Derivative()) ) / (den_ * den_); //TODO change to pow<2>
  }

  std::string str() const
  {
    return "(" + num_.str() + " / " + den_.str() + ")";
  }
};



} // Symbolic namespace
#endif