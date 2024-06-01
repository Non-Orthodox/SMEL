#ifndef SYMBOLIC_INCLUDE_CONSTANTS_HPP
#define SYMBOLIC_INCLUDE_CONSTANTS_HPP

#include <string>
#include <cstdint>
#include <cassert>
#include <numeric>

#include "symbolic_base.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {


// template<int64_t Numerator_, int64_t Denominator_>
// class RationalConstant;

template<typename T, T value_>
class Constant;

template<typename T>
class RuntimeConstant;

template<typename T>
class Reference;


// template<int64_t Numerator_, int64_t Denominator_ = 1>
// class RationalConstant : public SymbolicBase< RationalConstant<Numerator_,Denominator_> >
// {
// public:
//   static constexpr int64_t Numerator {Numerator_};
//   static constexpr int64_t Denominator {Denominator_};
//   static constexpr bool is_leaf = false;
//   static constexpr bool is_dynamic = false;

//   constexpr RationalConstant()
//   { static_assert(Denominator_ != 0, "RationalConstant cannot have (Denominator == 0)"); }

//   template<typename FloatType>
//   constexpr auto Evaluate(const FloatType input) const
//   { return static_cast<FloatType>(Numerator_) / static_cast<FloatType>(Denominator_); }

//   constexpr RationalConstant<0,1> Derivative() const
//   { return RationalConstant<0,1>(); }

//   std::string str() const
//   {
//     if constexpr (Denominator_ == 1)
//       return std::to_string(Numerator_);
//     else
//       return '(' + std::to_string(Numerator_) + '/' + std::to_string(Denominator_) + ')';
//   }
// };


// template<int64_t N, int64_t D>
// constexpr auto Fraction()
// {
//   constexpr int64_t factor = std::gcd(N,D);
//   return RationalConstant<N/factor, D/factor>();
// }


// template<int64_t N>
// constexpr auto Int()
// {
//   return RationalConstant<N,1>();
// }


// template<int64_t Denominator = 1>
// using Zero = RationalConstant<0,Denominator>;

// template<int64_t N = 1>
// using One = RationalConstant<N,N>;


template<typename T, T value_>
class Constant : public SymbolicBase< Constant<T,value_> >
{
public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = false;

  constexpr Constant()
  {}

  constexpr T Value() const
  { return value_; }
  
  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  { return static_cast<FloatType>(value_); }

  constexpr Constant<int64_t,0> Derivative() const
  { return Constant<int64_t,0>(); }

  std::string str() const
  { return std::to_string(value_); }
};

template<IntegralInt T1, T1 Val1, IntegralInt T2, T2 Val2>
using IntegerFraction = Quotient<Constant<T1,Val1>,Constant<T2,Val2>>;

template<typename T = int64_t>
using Zero = Constant<T,static_cast<T>(0)>;

template<typename T = int64_t>
using One = Constant<T,static_cast<T>(1)>;

template<int64_t N, int64_t D>
constexpr auto Fraction()
{
  constexpr int64_t factor = std::gcd(N,D);
  if constexpr (D == factor) {
    return Constant<int64_t,N/factor>();
  } else {
    return IntegerFraction<int64_t, N/factor, int64_t, D/factor>();
  }
}

template<int64_t N>
constexpr auto Int()
{
  return Constant<int64_t,N>();
}


// This is for storing any constant that isn't compile-time
template<typename T = double>
class RuntimeConstant : public SymbolicBase< RuntimeConstant<T> >
{
private:
  const T c_;

public:
  static constexpr bool is_leaf = false;
  static constexpr bool is_dynamic = true;

  constexpr RuntimeConstant(const T c) : c_{c}
  {}

  constexpr RuntimeConstant(const RuntimeConstant<T>& c) : c_{c.c_}
  {}

  constexpr RuntimeConstant(const RuntimeConstant<T> c1, const RuntimeConstant<T> c2)
    : c_{c1.c_ + c2.c_}
  {}

  T Value() const
  { return c_; }
  
  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  { return static_cast<FloatType>(c_); }

  constexpr Zero<> Derivative() const
  { return Zero<>(); }

  std::string str() const
  { return std::to_string(c_); }
};


template<typename T = double>
class Reference : public SymbolicBase< Reference<T> >
{
private:
  const T& c_;

public:
  static constexpr bool is_leaf = true;
  static constexpr bool is_dynamic = true;

  explicit constexpr Reference(const T& c) : c_{c}
  {}
  
  template<typename FloatType>
  FloatType Evaluate(const FloatType input) const
  { return static_cast<FloatType>(c_); }

  constexpr Zero<> Derivative() const
  { return Zero<>(); }

  std::string str() const
  { return std::to_string(c_); }

  template<typename Type>
  friend bool IsSame(const Reference<Type>&, const Reference<Type>&);
};


// // ---------- OPERATORS ----------
// // General addition of rational constants
// template<int64_t N1, int64_t D1, int64_t N2, int64_t D2>
// constexpr auto operator+(
//   const RationalConstant<N1,D1>& c1,
//   const RationalConstant<N2,D2>& c2
//   )
// {
//   if constexpr (D1 == D2) {
//     return RationalConstant<N1+N2, D1>();
//   } else {
//     constexpr int64_t num = (D2*N1) + (D1*N2);
//     constexpr int64_t denom = D1*D2;
//     constexpr int64_t factor = std::gcd(num,denom);
//     return RationalConstant<(num/factor), (denom/factor)>();
//   }
// }


// // General subtraction of rational constants
// //TODO make negation happen if numerator should be negative
// template<int64_t N1, int64_t D1, int64_t N2, int64_t D2>
// constexpr auto operator-(
//   const RationalConstant<N1,D1>& c1,
//   const RationalConstant<N2,D2>& c2
//   )
// {
//   if constexpr (D1 == D2) {
//     return RationalConstant<N1-N2, D1>();
//   } else {
//     constexpr int64_t num = (D2*N1) - (D1*N2);
//     constexpr int64_t denom = D1*D2;
//     constexpr int64_t factor = std::gcd(num,denom);
//     return RationalConstant<(num/factor), (denom/factor)>();
//   }
// }


// // Negation
// template<int64_t N, int64_t D>
// constexpr auto operator-(const RationalConstant<N,D>& frac)
// {
//   return RationalConstant<-N,D>();
// }


// template<int64_t N1, int64_t D1, int64_t N2, int64_t D2>
// constexpr auto operator*(
//   const RationalConstant<N1,D1>& c1,
//   const RationalConstant<N2,D2>& c2
//   )
// {
//   constexpr int64_t num = N1 * N2;
//   constexpr int64_t denom = D1 * D2;
//   constexpr int64_t factor = std::gcd(num,denom);
//   return RationalConstant<(num/factor), (denom/factor)>();
// }


// template<int64_t N1, int64_t D1, int64_t N2, int64_t D2>
// constexpr auto operator/(
//   const RationalConstant<N1,D1>& c1,
//   const RationalConstant<N2,D2>& c2
//   )
// {
//   static_assert(N2 != 0, "Cannot perform operator/(RationalConstant<N1,D1>,RationalConstant<N2,D2>) with N2 = 0");
//   constexpr int64_t num = N1 * D2;
//   constexpr int64_t denom = D1 * N2;
//   constexpr int64_t factor = std::gcd(num,denom);
//   return RationalConstant<(num/factor), (denom/factor)>();
// }


// // Sum of zeros
// template<int64_t D1, int64_t D2>
// constexpr Zero<> operator+(
//   const Zero<D1>& zero1,
//   const Zero<D2>& zero2
//   )
// {
//   return Zero<>();
// }


// // Difference of zeros
// template<int64_t D1, int64_t D2>
// constexpr Zero<> operator-(
//   const Zero<D1>& zero1,
//   const Zero<D2>& zero2
//   )
// {
//   return Zero<>();
// }


// // Product of zeros
// template<int64_t D1, int64_t D2>
// constexpr Zero<> operator*(
//   const Zero<D1>& zero1,
//   const Zero<D2>& zero2
//   )
// {
//   return Zero<>();
// }


// // Combining constants
// template<typename T, T Val1, T Val2>
// constexpr auto operator+(
//   const Constant<T,Val1>& c1,
//   const Constant<T,Val2>& c2
//   )
// {
//   return Constant<T,Val1+Val2>();
// }

// template<typename T, T Val1, T Val2>
// constexpr auto operator-(
//   const Constant<T,Val1>& c1,
//   const Constant<T,Val2>& c2
//   )
// {
//   return Constant<T,Val1-Val2>();
// }

// template<typename T, T Val1, T Val2>
// constexpr auto operator*(
//   const Constant<T,Val1>& c1,
//   const Constant<T,Val2>& c2
//   )
// {
//   return Constant<T,Val1*Val2>();
// }


// Combining runtime constants
template<typename T>
constexpr auto operator+(
  const RuntimeConstant<T>& c1,
  const RuntimeConstant<T>& c2
  )
{
  return RuntimeConstant<T>(c1,c2);
}

template<typename T>
constexpr auto operator-(
  const RuntimeConstant<T>& c1,
  const RuntimeConstant<T>& c2
  )
{
  return RuntimeConstant<T>(c1.Value() - c2.Value());
}

template<typename T>
constexpr RuntimeConstant<T> operator*(const RuntimeConstant<T>& c1, const RuntimeConstant<T>& c2)
{
  return RuntimeConstant<T>(c1.Value() * c2.Value());
}


// // Combining Constant and RationalConstant
// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator+(
//   Constant<T,Val>, RationalConstant<N,D>)
// {
//   return Constant<T, Val + (static_cast<T>(N) / static_cast<T>(D))>();
// }

// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator+(
//   RationalConstant<N,D>, Constant<T,Val>)
// {
//   return Constant<T, Val + (static_cast<T>(N) / static_cast<T>(D))>();
// }

// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator-(
//   Constant<T,Val>, RationalConstant<N,D>)
// {
//   return Constant<T, Val - (static_cast<T>(N) / static_cast<T>(D))>();
// }

// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator-(
//   RationalConstant<N,D>, Constant<T,Val>)
// {
//   return Constant<T, Val - (static_cast<T>(N) / static_cast<T>(D))>();
// }

// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator*(
//   Constant<T,Val>, RationalConstant<N,D>)
// {
//   return Constant<T, Val * (static_cast<T>(N) / static_cast<T>(D))>();
// }

// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator*(
//   RationalConstant<N,D>, Constant<T,Val>)
// {
//   return Constant<T, Val * (static_cast<T>(N) / static_cast<T>(D))>();
// }

// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator/(
//   Constant<T,Val>, RationalConstant<N,D>)
// {
//   return Constant<T, Val * (static_cast<T>(D) / static_cast<T>(N))>();
// }

// template<typename T, T Val, int64_t N, int64_t D>
// constexpr auto operator/(
//   RationalConstant<N,D>, Constant<T,Val>)
// {
//   return Constant<T, (static_cast<T>(N) / static_cast<T>(D)) / Val>();
// }


} // Symbolic namespace
#endif