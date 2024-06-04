#ifndef SYMBOLIC_INCLUDE_ABS_HPP
#define SYMBOLIC_INCLUDE_ABS_HPP

#include <cassert>

#include "symbolic_base.hpp"
#include "constants.hpp"

namespace SYMBOLIC_NAMESPACE_NAME {

template<typename SymType>
class Signum : public SymbolicBase< Signum<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Signum(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    assert(expr_.Evaluate(input) != 0);
    return (expr_.Evaluate(input) < static_cast<FloatType>(0))
            ? static_cast<FloatType>(-1) : static_cast<FloatType>(1);
  }

  constexpr auto Derivative() const
  {
    //TODO requires delta function
    // return Int<2>() * Delta<SymType>(expr_);
    return Zero<>();
  }

  std::string str() const
  {
    return "sgn(" + expr_.str() + ")";
  }
};


template<typename SymType>
class AbsoluteValue : public SymbolicBase< AbsoluteValue<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr AbsoluteValue(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    FloatType val = expr_.Evaluate(input);
    return (val < static_cast<FloatType>(0)) ? -val : val;
  }

  constexpr auto Derivative() const
  {
    return Signum(expr_) * expr_.Derivative();
  }

  std::string str() const
  {
    return "|" + expr_.str() + "|";
  }
};


template<typename SymType>
auto abs(const SymbolicBase<SymType> expr)
{
  return AbsoluteValue<SymType>(expr.derived());
}


} // symbolic namespace
#endif