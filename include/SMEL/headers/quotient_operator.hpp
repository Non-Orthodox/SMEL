#ifndef SYMBOLIC_INCLUDE_QUOTIENT_OPERATOR_HPP
#define SYMBOLIC_INCLUDE_QUOTIENT_OPERATOR_HPP

#include "prototyping.hpp"
#include "type_deductions.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

//TODO try cancellation if either is a product
//TODO cancellation of powers: x^2 / x^3
template<typename Sym1, typename Sym2>
constexpr auto
operator/(const SymbolicBase<Sym1>& expr1, const SymbolicBase<Sym2>& expr2)
{
  static_assert(!is_zero_v<Sym2>, "Cannot divide by zero");
  if constexpr (is_zero_v<Sym1>) {
    return Zero<>();
  }
  else if constexpr (is_same_v<Sym1,Sym2>) {
    return One<>();
  }
  else {
    return Quotient(expr1.derived(),expr2.derived());
  }
}

} // Symbolic namespace
#endif