#ifndef SYMBOLIC_INCLUDE_SINCOS_HPP
#define SYMBOLIC_INCLUDE_SINCOS_HPP

#include <cmath>

#include "constants.hpp"
// #include "pow.hpp"

namespace SYMBOLIC_NAMESPACE_NAME {

template<typename SymType>
class Sine;

template<typename SymType>
class Cosine;

// template<typename SymType>
// class ArcSine;

// template<typename SymType>
// class ArcCosine;


template<typename SymType>
class Sine : public SymbolicBase< Sine<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Sine(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  {
    return std::sin(expr_.Evaluate(input));
  }

  constexpr auto Derivative() const
  {
    return Cosine<SymType>(expr_) * expr_.Derivative();
  }

  std::string str() const
  {
    return "sin(" + expr_.str() + ")";
  }
};


template<typename SymType>
class Cosine : public SymbolicBase< Cosine<SymType> >
{
private:
  typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = SymType::is_dynamic;

  constexpr Cosine(const SymType& expr) : expr_{expr}
  {}

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  {
    return std::cos(expr_.Evaluate(input));
  }

  constexpr auto Derivative() const
  {
    return Sine<SymType>(expr_) * (-expr_.Derivative());
  }

  std::string str() const
  {
    return "cos(" + expr_.str() + ")";
  }
};


//TODO requires pow
// template<typename SymType>
// class ArcSine : public SymbolicBase< ArcSine<SymType> >
// {
// private:
//   typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

// public:
//   static constexpr bool is_leaf = false;
//   static constexpr bool is_dynamic = SymType::is_dynamic;

//   constexpr ArcSine(const SymType& expr) : expr_{expr}
//   {}

//   template<typename FloatType>
//   FloatType Evaluate(const FloatType input) const
//   {
//     //TODO wrap input to be between [-1,1]?
//     return std::asin(expr_.Evaluate(input));
//   }

//   constexpr auto Derivative() const
//   {
//     return pow<-1,2>( One<>() - pow<2>(expr_) ) * expr_.Derivative();
//   }

//   std::string str() const
//   {
//     return "arcsin(" + expr_.str() + ")";
//   }
// };


// template<typename SymType>
// class ArcCosine : public SymbolicBase< ArcCosine<SymType> >
// {
// private:
//   typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> expr_;

// public:
//   static constexpr bool is_leaf = false;
//   static constexpr bool is_dynamic = SymType::is_dynamic;

//   constexpr ArcCosine(const SymType& expr) : expr_{expr}
//   {}

//   template<typename FloatType>
//   FloatType Evaluate(const FloatType input) const
//   {
//     //TODO wrap input to be between [-1,1]?
//     return std::acos(expr_.Evaluate(input));
//   }

//   constexpr auto Derivative() const
//   {
//     return pow<-1,2>( One<>() - pow<2>(expr_) ) * (-expr_.Derivative());
//   }

//   std::string str() const
//   {
//     return "arccos(" + expr_.str() + ")";
//   }
// };


template<typename SymType>
constexpr auto sin(const SymbolicBase<SymType>& expr)
{
  return Sine<SymType>(expr.derived());
}

template<typename SymType>
constexpr auto cos(const SymbolicBase<SymType>& expr)
{
  return Cosine<SymType>(expr.derived());
}

// template<typename SymType>
// constexpr auto arcsin(const SymbolicBase<SymType>& expr)
// {
//   return ArcSine<SymType>(expr.derived());
// }

// template<typename SymType>
// constexpr auto arccos(const SymbolicBase<SymType>& expr)
// {
//   return ArcCosine<SymType>(expr.derived());
// }


} // end of symbolic namespace
#endif