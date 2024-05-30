#ifndef SYMBOLIC_INCLUDE_DIFFERENCE_OPERATOR_HPP
#define SYMBOLIC_INCLUDE_DIFFERENCE_OPERATOR_HPP

#include "type_deductions.hpp"
#include "symbolic_base.hpp"
#include "sum_operator.hpp"
#include "negation.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename Sym1, typename Sym2>
constexpr auto
operator-(const SymbolicBase<Sym1>& expr1, const SymbolicBase<Sym2>& expr2)
{
  if constexpr (is_zero_v<Sym1>) {
    return expr2.derived();
  }
  else if constexpr (is_zero_v<Sym2>) {
    return -expr1.derived();
  }
  else if constexpr (is_same_v<Sym1,Sym2>) {
    return Zero<>();
  }
  else {
    return expr1.derived() + (-expr2.derived());
  }
}

} // Symbolic namespace
#endif