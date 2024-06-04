#ifndef SYMBOLIC_INCLUDE_COTCSC_HPP
#define SYMBOLIC_INCLUDE_COTCSC_HPP

#include <cmath>

#include "constants.hpp"
#include "exp.hpp"

namespace SYMBOLIC_NAMESPACE_NAME {

template<typename SymType>
class Cotangent;

template<typename SymType>
class Cosecant;

template<typename SymType>
class ArcCotangent;

// template<typename SymType>
// class ArcCosecant;


template<typename SymType>
class Cotangent : public SymbolicBase< Cotangent<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Cotangent(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    return 1.0 / std::tan(expr_.Evaluate(input));
  }

  auto Derivative() const
  {
    return pow<2>( Cosecant<SymType>(expr_) ) * (-expr_.Derivative());
  }

  std::string str() const
  {
    return "cot(" + expr_.str() + ")";
  }
};


template<typename SymType>
class Cosecant : public SymbolicBase< Cosecant<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Cosecant(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    return 1.0 / std::sin(expr_.Evaluate(input));
  }

  auto Derivative() const
  {
    return Cosecant<SymType>(expr_) * Cotangent<SymType>(expr_) * (-expr_.Derivative());
  }

  std::string str() const
  {
    return "csc(" + expr_.str() + ")";
  }
};


template<typename SymType>
class ArcCotangent : public SymbolicBase< ArcCotangent<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr ArcCotangent(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  {
    return std::atan(1.0 / expr_.Evaluate(input));
  }

  auto Derivative() const
  {
    return -expr_.Derivative() / ( One<>() + pow<2>(expr_) );
  }

  std::string str() const
  {
    return "arccot(" + expr_.str() + ")";
  }
};


//TODO ArcCosecant - need absolute value first


template<typename SymType>
auto cot(const SymbolicBase<SymType>& expr)
{
  return Cotangent<SymType>(expr.derived());
}

template<typename SymType>
auto csc(const SymbolicBase<SymType>& expr)
{
  return Cosecant<SymType>(expr.derived());
}

template<typename SymType>
auto arccot(const SymbolicBase<SymType>& expr)
{
  return ArcCotangent<SymType>(expr.derived());
}

// template<typename SymType>
// auto arccsc(const SymbolicBase<SymType>& expr)
// {
//   return ArcCosecant<SymType>(expr.derived());
// }


} // symbolic namespace
#endif