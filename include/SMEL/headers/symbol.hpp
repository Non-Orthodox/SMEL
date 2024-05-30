#ifndef SYMBOLIC_INCLUDE_SYMBOL_HPP
#define SYMBOLIC_INCLUDE_SYMBOL_HPP

#include "symbolic_base.hpp"
#include "constants.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

class Symbol : public SymbolicBase< Symbol >
{
public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = false;

  constexpr Symbol() {}

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  { return input; }

  constexpr One<> Derivative() const
  { return One<>(); }

  std::string str() const
  { return "x"; }
};


} // Symbolic namespace
#endif