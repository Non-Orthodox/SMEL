#ifndef SYMBOLIC_INCLUDE_CONSTANT_OPERATORS_HPP
#define SYMBOLIC_INCLUDE_CONSTANT_OPERATORS_HPP

#include <type_traits>

#include "concepts.hpp"
#include "constants.hpp"


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename... Ts>
struct operation_return;

template<typename T>
struct operation_return<T>
{
  typedef std::decay_t<T> type;
};

template<std::floating_point T1, std::floating_point T2>
struct operation_return<T1,T2>
{
  typedef typename std::conditional_t<
      sizeof(T1) >= sizeof(T2), std::decay_t<T1>, std::decay_t<T2>
    > type;
};

template<std::floating_point T1, IntegralInt T2>
struct operation_return<T1,T2>
{
  typedef std::decay_t<T1> type;
};

template<IntegralInt T1, std::floating_point T2>
struct operation_return<T1,T2>
{
  typedef std::decay_t<T2> type;
};

template<IntegralInt T1, IntegralInt T2>
struct operation_return<T1,T2>
{
  typedef typename std::conditional_t<
      sizeof(T1) >= sizeof(T2), std::decay_t<T1>, std::decay_t<T2>
    > type;
};

template<typename T1, typename T2, typename... Ts>
struct operation_return<T1,T2,Ts...>
{
  typedef typename operation_return<
      typename operation_return<T1,T2>::type, Ts...
    >::type type;
};

template<typename... Ts>
using operation_return_t = typename operation_return<Ts...>::type;


// ------------------- Constant op Constant -------------------
template<typename T1, T1 Val1, typename T2, T2 Val2>
constexpr auto operator+(const Constant<T1,Val1>, const Constant<T2,Val2>)
{
  typedef operation_return_t<T1,T2> ResultType;
  return Constant<ResultType, static_cast<ResultType>(Val1) + static_cast<ResultType>(Val2)>();
}

template<typename T1, T1 Val1, typename T2, T2 Val2>
constexpr auto operator-(const Constant<T1,Val1>, const Constant<T2,Val2>)
{
  typedef operation_return_t<T1,T2> ResultType;
  return Constant<ResultType, static_cast<ResultType>(Val1) - static_cast<ResultType>(Val2)>();
}

template<typename T, T Val>
constexpr auto operator-(const Constant<T,Val>)
{
  return Constant<T,-Val>();
}

template<typename T1, T1 Val1, typename T2, T2 Val2>
constexpr auto operator*(const Constant<T1,Val1>, const Constant<T2,Val2>)
{
  typedef operation_return_t<T1,T2> ResultType;
  return Constant<ResultType, static_cast<ResultType>(Val1) * static_cast<ResultType>(Val2)>();
}

template<typename T1, T1 Val1, typename T2, T2 Val2>
constexpr auto operator/(const Constant<T1,Val1>, const Constant<T2,Val2>)
{
  static_assert(Val2 != static_cast<T2>(0), "Cannot divide by zero");
  typedef operation_return_t<T1,T2> ResultType;
  if constexpr (IntegralInt<T1> && IntegralInt<T2>) {
    constexpr ResultType num = static_cast<ResultType>(Val1);
    constexpr ResultType den = static_cast<ResultType>(Val2);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (den == factor) {
      return Constant<ResultType,num/factor>();
    } else {
      return Quotient(Constant<ResultType,num/factor>(), Constant<ResultType,den/factor>());
    }
  } else {
    return Constant<ResultType, static_cast<ResultType>(Val1) / static_cast<ResultType>(Val2)>();
  }
}


//TODO test with use of unsigned types, consider making this function force signed result if needed
//TODO ProductCombinable additions

//TODO combining literals with RuntimeConstant
//TODO combining Constant and RuntimeConstant?
//TODO SumCombinable additions for RuntimeConstant op Constant

// ------------------- IntegerFraction op IntegerFraction -------------------
template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralInt T3, T3 N2, IntegralInt T4, T4 D2>
constexpr auto operator+(
  IntegerFraction<T1,N1,T2,D1>,
  IntegerFraction<T3,N2,T4,D2>)
{
  typedef operation_return_t<T1,T2,T3,T4> ResultType;
  constexpr ResultType num = (static_cast<ResultType>(D2) * static_cast<ResultType>(N1))
      + (static_cast<ResultType>(D1) * static_cast<ResultType>(N2));
  constexpr ResultType den = static_cast<ResultType>(D1) * static_cast<ResultType>(D2);
  constexpr ResultType factor = std::gcd(num,den);
  if constexpr (factor == den) {
    return Constant<ResultType,num/factor>();
  } else {
    return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
  }
}

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralInt T3, T3 N2, IntegralInt T4, T4 D2>
constexpr auto operator-(
  IntegerFraction<T1,N1,T2,D1>,
  IntegerFraction<T3,N2,T4,D2>)
{
  typedef operation_return_t<T1,T2,T3,T4> ResultType;
  constexpr ResultType num = (static_cast<ResultType>(D2) * static_cast<ResultType>(N1))
      - (static_cast<ResultType>(D1) * static_cast<ResultType>(N2));
  constexpr ResultType den = static_cast<ResultType>(D1) * static_cast<ResultType>(D2);
  constexpr ResultType factor = std::gcd(num,den);
  if constexpr (factor == den) {
    return Constant<ResultType,num/factor>();
  } else {
    return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
  }
}

//TODO consider using negation here
template<IntegralInt T1, T1 N, IntegralInt T2, T2 D>
constexpr auto operator-(IntegerFraction<T1,N,T2,D>)
{
  return IntegerFraction<T1,-N,T2,D>();
}

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralInt T3, T3 N2, IntegralInt T4, T4 D2>
constexpr auto operator*(
  IntegerFraction<T1,N1,T2,D1>,
  IntegerFraction<T3,N2,T4,D2>)
{
  typedef operation_return_t<T1,T2,T3,T4> ResultType;
  constexpr ResultType num = static_cast<ResultType>(N1) * static_cast<ResultType>(N2);
  constexpr ResultType den = static_cast<ResultType>(D1) * static_cast<ResultType>(D2);
  constexpr ResultType factor = std::gcd(num,den);
  if constexpr (factor == den) {
    return Constant<ResultType,num/factor>();
  } else {
    return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
  }
}

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralInt T3, T3 N2, IntegralInt T4, T4 D2>
constexpr auto operator/(
  IntegerFraction<T1,N1,T2,D1>,
  IntegerFraction<T3,N2,T4,D2>)
{
  typedef operation_return_t<T1,T2,T3,T4> ResultType;
  constexpr ResultType num = static_cast<ResultType>(N1) * static_cast<ResultType>(D2);
  constexpr ResultType den = static_cast<ResultType>(D1) * static_cast<ResultType>(N2);
  constexpr ResultType factor = std::gcd(num,den);
  if constexpr (factor == den) {
    return Constant<ResultType,num/factor>();
  } else {
    return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
  }
}

// ------------------- Constant op IntegerFraction -------------------
template<IntegralConstant T1, T1 N1, IntegralInt T2, T2 N2,
    IntegralInt T3, T3 D2>
constexpr auto operator+(
  Constant<T1,N1>,
  IntegerFraction<T2,N2,T3,D2>)
{
  if constexpr (std::floating_point<T1>) {
    return Constant<T1, N1 + (static_cast<T1>(N2) / static_cast<T1>(D2))>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = (static_cast<ResultType>(N1) * static_cast<ResultType>(D2))
        + static_cast<ResultType>(N2);
    constexpr ResultType den = static_cast<ResultType>(D2);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralConstant T3, T3 N2>
constexpr auto operator+(
  IntegerFraction<T1,N1,T2,D1>,
  Constant<T3,N2>)
{
  if constexpr (std::floating_point<T3>) {
    return Constant<T3, N2 + (static_cast<T3>(N1) / static_cast<T3>(D1))>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = static_cast<ResultType>(N1)
        + (static_cast<ResultType>(N2) * static_cast<ResultType>(D1));
    constexpr ResultType den = static_cast<ResultType>(D1);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}

template<IntegralConstant T1, T1 N1, IntegralInt T2, T2 N2,
    IntegralInt T3, T3 D2>
constexpr auto operator-(
  Constant<T1,N1>,
  IntegerFraction<T2,N2,T3,D2>)
{
  if constexpr (std::floating_point<T1>) {
    return Constant<T1, N1 - (static_cast<T1>(N2) / static_cast<T1>(D2))>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = (static_cast<ResultType>(N1) * static_cast<ResultType>(D2))
        - static_cast<ResultType>(N2);
    constexpr ResultType den = static_cast<ResultType>(D2);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralConstant T3, T3 N2>
constexpr auto operator-(
  IntegerFraction<T1,N1,T2,D1>,
  Constant<T3,N2>)
{
  if constexpr (std::floating_point<T3>) {
    return Constant<T3, (static_cast<T3>(N1) / static_cast<T3>(D1)) - N2>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = static_cast<ResultType>(N1)
        - (static_cast<ResultType>(N2) * static_cast<ResultType>(D1));
    constexpr ResultType den = static_cast<ResultType>(D1);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}

template<IntegralConstant T1, T1 N1, IntegralInt T2, T2 N2,
    IntegralInt T3, T3 D2>
constexpr auto operator*(
  Constant<T1,N1>,
  IntegerFraction<T2,N2,T3,D2>)
{
  if constexpr (std::floating_point<T1>) {
    return Constant<T1, N1 * (static_cast<T1>(N2) / static_cast<T1>(D2))>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = static_cast<ResultType>(N1) * static_cast<ResultType>(N2);
    constexpr ResultType den = static_cast<ResultType>(D2);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralConstant T3, T3 N2>
constexpr auto operator*(
  IntegerFraction<T1,N1,T2,D1>,
  Constant<T3,N2>)
{
  if constexpr (std::floating_point<T3>) {
    return Constant<T3, N2 * (static_cast<T3>(N1) / static_cast<T3>(D1))>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = static_cast<ResultType>(N1) * static_cast<ResultType>(N2);
    constexpr ResultType den = static_cast<ResultType>(D1);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}

template<IntegralConstant T1, T1 N1, IntegralInt T2, T2 N2,
    IntegralInt T3, T3 D2>
constexpr auto operator/(
  Constant<T1,N1>,
  IntegerFraction<T2,N2,T3,D2>)
{
  if constexpr (std::floating_point<T1>) {
    return Constant<T1, N1 * (static_cast<T1>(D2) / static_cast<T1>(N2))>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = static_cast<ResultType>(N1) * static_cast<ResultType>(D2);
    constexpr ResultType den = static_cast<ResultType>(N2);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}

template<IntegralInt T1, T1 N1, IntegralInt T2, T2 D1,
    IntegralConstant T3, T3 N2>
constexpr auto operator/(
  IntegerFraction<T1,N1,T2,D1>,
  Constant<T3,N2>)
{
  if constexpr (std::floating_point<T3>) {
    return Constant<T3, static_cast<T3>(N1) / (static_cast<T3>(D1) * N2)>();
  }
  else {
    typedef operation_return_t<T1,T2,T3> ResultType;
    constexpr ResultType num = static_cast<ResultType>(N1);
    constexpr ResultType den = static_cast<ResultType>(D1) * static_cast<ResultType>(N2);
    constexpr ResultType factor = std::gcd(num,den);
    if constexpr (factor == den) {
      return Constant<ResultType,num/factor>();
    } else {
      return IntegerFraction<ResultType,num/factor,ResultType,den/factor>();
    }
  }
}



}// Symbolic namespace
#endif