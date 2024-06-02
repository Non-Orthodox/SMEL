#ifndef SYMBOLIC_INCLUDE_LOG_HPP
#define SYMBOLIC_INCLUDE_LOG_HPP

#include <cmath>

#include "constants.hpp"

namespace SYMBOLIC_NAMESPACE_NAME {


template<typename SymType>
constexpr auto ln(const SymbolicBase<SymType>& expr)
{
  if constexpr (is_constant_e_v<SymType>) {
    return One<>();
  } else {
    return Logarithm(constant_e<>(), expr.derived());
  }
}


template<typename Base, typename SymType>
class Logarithm : public SymbolicBase< Logarithm<Base,SymType> >
{
private:
  typename std::conditional_t<Base::is_leaf, const Base&, const Base> base_;
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic || Base::is_dynamic;

  constexpr Logarithm(const Base& base, const SymType& expr)
      : base_{base}, expr_{expr}
  {
    static_assert(zero_derivative_v<Base>,"Logarithm must have constant base");
    static_assert(!is_zero_v<Base>, "Logarithm cannot have a base of zero");
  }

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  { 
    if constexpr (is_constant_e_v<Base>) {
      return std::log(expr_.Evaluate(input));
    }
    // else if constexpr () {

    // }
    // else if constexpr () {

    // }
    else {
      return std::log(expr_.Evaluate(input)) / std::log(base_.Evaluate(input));
    }
  }

  constexpr auto Derivative() const
  {
    if constexpr (is_constant_e_v<Base>) {
      return expr_.Derivative() / expr_ ;
    } else {
      return expr_.Derivative() / ( ln(base_) * expr_ );
    }
  }

  std::string str() const
  { 
    if constexpr (is_constant_e_v<Base>) {
      return "ln(" + expr_.str() + ')';
    }
    else {
      return "log(" + base_.str() + "," + expr_.str() + ')';
    }
  }
};



} // Symbolic namespace
#endif