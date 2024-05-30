#ifndef SYMBOLIC_NEGATION_HPP
#define SYMBOLIC_NEGATION_HPP

#include "symbolic_base.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename SymType>
class Negation : public SymbolicBase< Negation<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Negation(const SymType& expr)
      : expr_{expr}
  {}

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  { 
    return -(expr_.Evaluate(input));
  }

  constexpr auto Derivative() const
  { return -(expr_.Derivative()); }

  std::string str() const
  { return "-(" + expr_.str() + ')'; }

  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType>
  constexpr Negate() const
  {
    return expr_;
  }
};


template<typename SymType>
constexpr auto operator-(const SymbolicBase<SymType>& expr)
{
  return Negation<SymType>(expr.derived());
}


template<typename SymType>
constexpr auto operator-(const Negation<SymType>& expr)
{
  return expr.Negate();
}


} // symbolic namespace
#endif