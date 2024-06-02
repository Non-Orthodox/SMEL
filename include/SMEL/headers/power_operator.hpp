#ifndef SYMBOLIC_INCLUDE_POWER_OPERATOR_HPP
#define SYMBOLIC_INCLUDE_POWER_OPERATOR_HPP

#include "prototyping.hpp"
#include "type_deductions.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename Sym1, typename Sym2>
constexpr auto
operator^(const SymbolicBase<Sym1>& expr1, const SymbolicBase<Sym2>& expr2)
{
  //TODO assert that it isn't zero to a negative power
  if constexpr (is_one_v<Sym2>) {
    return expr1.derived();
  }
  else if constexpr (is_zero_v<Sym2>) {
    return One<>();
  }
  else if constexpr (is_zero_v<Sym1>) {
    return Zero<>();
  }
  else if constexpr (is_one_v<Sym1>) {
    return One<>();
  }
  else {
    return Exponential(expr1.derived(),expr2.derived());
  }
}


// (a^b)^c

} // Symbolic namespace
#endif