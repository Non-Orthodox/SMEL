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

template<typename SymType>
class Sine;

template<typename SymType>
class Cosine;


} // Symbolic namespace
#endif