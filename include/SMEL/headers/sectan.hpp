#ifndef SYMBOLIC_INCLUDE_SECTAN_HPP
#define SYMBOLIC_INCLUDE_SECTAN_HPP

#include <cmath>

#include "constants.hpp"
#include "exp.hpp"

namespace SYMBOLIC_NAMESPACE_NAME {

template<typename SymType>
class Tangent;

template<typename SymType>
class Secant;

template<typename SymType>
class ArcTangent;

template<typename SymType>
class ArcSecant;


template<typename SymType>
class Tangent : public SymbolicBase< Tangent<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Tangent(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    return std::tan(expr_.Evaluate(input));
  }

  constexpr auto Derivative() const
  {
    return pow<2>( Secant<SymType>(expr_) ) * expr_.Derivative();
  }

  std::string str() const
  {
    return "tan(" + expr_.str() + ")";
  }
};


template<typename SymType>
class Secant : public SymbolicBase< Secant<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Secant(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    return (1.0 / std::cos(expr_.Evaluate(input)));
  }

  constexpr auto Derivative() const
  {
    return Secant<SymType>(expr_) * Tangent<SymType>(expr_) * expr_.Derivative();
  }

  std::string str() const
  {
    return "sec(" + expr_.str() + ")";
  }
};


//TODO atan2
template<typename SymType>
class ArcTangent : public SymbolicBase< ArcTangent<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr ArcTangent(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    return std::atan(expr_.Evaluate(input));
  }

  constexpr auto Derivative() const
  {
    return expr_.Derivative() / ( One<>() + pow<2>(expr_) );
  }

  std::string str() const
  {
    return "arctan(" + expr_.str() + ")";
  }
};


template<typename SymType>
class ArcSecant : public SymbolicBase< ArcSecant<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr ArcSecant(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    //TODO verify
    return std::acos(1.0 / expr_.Evaluate(input));
  }

  constexpr auto Derivative() const
  {
    return expr_.Derivative() / ( abs(expr_) * sqrt((expr_ ^ Int<2>()) - One<>()) );
  }

  std::string str() const
  {
    return "arcsec(" + expr_.str() + ")";
  }
};


template<typename SymType>
auto tan(const SymbolicBase<SymType> expr)
{
  return Tangent<SymType>(expr.derived());
}

template<typename SymType>
auto sec(const SymbolicBase<SymType> expr)
{
  return Secant<SymType>(expr.derived());
}

template<typename SymType>
auto arctan(const SymbolicBase<SymType> expr)
{
  return ArcTangent<SymType>(expr.derived());
}

template<typename SymType>
auto arcsec(const SymbolicBase<SymType> expr)
{
  return ArcSecant<SymType>(expr.derived());
}


} // symbolic namespace
#endif