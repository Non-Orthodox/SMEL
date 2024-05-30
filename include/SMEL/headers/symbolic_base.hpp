#ifndef SYMBOLIC_INCLUDE_SYMBOLICBASE_H
#define SYMBOLIC_INCLUDE_SYMBOLICBASE_H

#include <type_traits>
#include <string>


#define SYMBOLIC_NAMESPACE_NAME Smel


namespace SYMBOLIC_NAMESPACE_NAME {

template<typename SymType>
struct BranchType
{
  typedef typename std::conditional_t<SymType::is_leaf, const SymType&, const SymType> type;
};


template<class Derived>
class SymbolicBase
{
public:
  constexpr Derived& derived()
  { return static_cast<Derived&>(*this); }

  constexpr const Derived& derived() const
  { return static_cast<Derived const&>(*this); }

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  { return derived().Evaluate(input); }

  template<typename FloatType>
  constexpr FloatType operator()(const FloatType input) const
  { return derived().Evaluate(input); }
  
  constexpr auto Derivative() const
  { return derived().Derivative(); }

  constexpr std::string str() const
  { return derived().str(); }
};


template<typename Sym1, typename Sym2>
struct is_same
{
  static constexpr bool value = std::is_same_v<Sym1,Sym2> && !(Sym1::is_dynamic) && !(Sym2::is_dynamic);
};

template<typename Sym1, typename Sym2>
constexpr bool is_same_v = is_same<Sym1,Sym2>::value;


} // Symbolic namespace
#endif