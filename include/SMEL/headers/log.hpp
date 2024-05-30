#ifndef SYMBOLIC_INCLUDE_LOG_HPP
#define SYMBOLIC_INCLUDE_LOG_HPP



namespace SYMBOLIC_NAMESPACE_NAME {

//? Thought - what about if the base is an expression, such that it can vary?

template<typename Base, typename SymType>
class Logarithm;


template<typename SymType>
auto ln(const SymbolicBase<SymType>& expr)
{
  
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
  }

  template<typename FloatType>
  constexpr FloatType Evaluate(const FloatType input) const
  { 
    
  }

  constexpr auto Derivative() const
  {
    expr_.Derivative() / ( ln(base_) * expr_ );
  }

  std::string str() const
  { 
    //TODO if constexpr (is_same_v<Base,e>)
    //TODO if constexpr (is_integral_int_v<Base>)
    return "log(" + expr_.str() + ')';
  }
};



} // Symbolic namespace
#endif