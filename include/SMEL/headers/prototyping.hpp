#ifndef SYMBOLIC_INCLUDE_PROTOTYPING_HPP
#define SYMBOLIC_INCLUDE_PROTOTYPING_HPP


namespace SYMBOLIC_NAMESPACE_NAME {

class Symbol;

template<typename SymType>
class Negation;

template<class... ExprTypes>
class TupleSum;

template<class... ExprTypes>
class TupleProduct;

template<typename NumExpr, typename DenExpr>
class Quotient;

template<typename Base, typename SymType>
class Logarithm;

template<typename Base, typename Exponent>
class Exponential;

template<typename SymType>
class Sine;

template<typename SymType>
class Cosine;

template<typename SymType>
class ArcSine;

template<typename SymType>
class ArcCosine;

template<typename SymType>
class Tangent;

template<typename SymType>
class Secant;

template<typename SymType>
class ArcTangent;

// template<typename SymType>
// class ArcSecant;

} // Symbolic namespace
#endif