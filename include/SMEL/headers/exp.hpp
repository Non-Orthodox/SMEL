#ifndef SYMBOLIC_POW_HPP
#define SYMBOLIC_POW_HPP

#include <cmath>

#include "symbolic_base.hpp"
#include "constants.hpp"
#include "product.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename SymType>
constexpr auto exp(const SymbolicBase<SymType>& expr)
{
  //TODO if expr is natural logarithm
  return Exponential(constant_e<>(), expr.derived());
}


template<typename Base, typename Exponent>
class Exponential : public SymbolicBase< Exponential<Base,Exponent> >
{
private:
  typename std::conditional_t<Base::is_leaf, const Base&, const Base> base_;
  typename std::conditional_t<Exponent::is_leaf, const Exponent&, const Exponent> exponent_;
  
public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = Base::is_dynamic || Exponent::is_dynamic;

  constexpr Exponential(const Base& base, const Exponent& exponent)
    : exponent_{exponent}, base_{base}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    if constexpr (is_constant_e_v<Base>) {
      return std::exp(exponent_.Evaluate(input));
    } else {
      return std::pow(base_.Evaluate(input), exponent_.Evaluate(input));
    }
  }

  constexpr auto Derivative() const
  {
    if constexpr (zero_derivative_v<Exponent>) {
      if constexpr (zero_derivative_v<Base>) {
        return Zero<>();
      } else {
        return exponent_ * (base_ ^ (exponent_ - One<>())) * base_.Derivative();
      }
    }
    else {
      if constexpr (zero_derivative_v<Base>) {
        if constexpr (is_constant_e_v<Base>) {
          return (base_ ^ exponent_) * exponent_.Derivative();
        }
        else {
          return ln(base_) * (base_ ^ exponent_) * exponent_.Derivative();
        }
      } else {
        return exp(exponent_ * ln(base_)).Derivative();
      }
    }
  }

  std::string str() const
  {
    if constexpr (zero_derivative_v<Exponent>) {
      if constexpr (zero_derivative_v<Base>) {
        return "(" + base_.str() + " ^ " + exponent_.str() + ")";
      } else {
        return "(" + base_.str() + ")^" + exponent_.str();
      }
    }
    else {
      if constexpr (zero_derivative_v<Base>) {
        return base_.str() + " ^ (" + exponent_.str() + ")";
      } else {
        return "(" + base_.str() + ") ^ (" + exponent_.str() + ")";
      }
    }
  }

};


// expr ^ constant
template<int64_t Power, typename SymType>
constexpr auto pow(const SymType expr)
{
  return Exponential(expr, Constant<int64_t,Power>());
}

template<double Power, typename SymType>
constexpr auto pow(const SymType expr)
{
  return Exponential(expr, Constant<double,Power>());
}


} // Symbolic namespace
#endif