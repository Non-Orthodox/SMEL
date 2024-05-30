#ifndef SYMBOLIC_INCLUDE_METAPROGRAMMING_HPP
#define SYMBOLIC_INCLUDE_METAPROGRAMMING_HPP

#include <utility>
#include <tuple>


template<int N, typename... Ts> using NthTypeOf =
        typename std::tuple_element<N, std::tuple<Ts...>>::type;

template<typename T, T Val>
constexpr bool ValueMatch()
{
  return false;
}

template<typename T, T Val, T Num, T... Nums>
constexpr bool ValueMatch()
{
  return (Val == Num) || ValueMatch<std::size_t, Val, Nums...>();
}

template<std::size_t Val, std::size_t... Nums>
constexpr bool IndexMatch()
{
  return ValueMatch<std::size_t, Val, Nums...>();
}

template<int Val, int... Nums>
constexpr bool IntMatch()
{
  return ValueMatch<int, Val, Nums...>();
}

template<std::size_t N, std::size_t... Seq>
constexpr std::index_sequence<N + Seq...>
integer_sequence_bias(std::index_sequence<Seq...>)
{ return {}; }

// Generates integer set [Start,End)
template<std::size_t Start, std::size_t End>
using make_index_range = decltype(integer_sequence_bias<Start>(std::make_index_sequence<End-Start>()));

template<typename TupleType, std::size_t... I>
constexpr auto tuple_slice_impl(const TupleType& tup, const std::index_sequence<I...>)
{
  return std::forward_as_tuple(std::get<I>(tup)...);
}

// Returns tuple of elements [Start,End)
template<std::size_t Start, std::size_t End, typename... Ts>
constexpr auto tuple_slice(const std::tuple<Ts...>& tup)
{
  return tuple_slice_impl(tup, make_index_range<Start,End>());
}

template<std::size_t... Seq1, std::size_t... Seq2>
constexpr auto index_sequence_cat(const std::index_sequence<Seq1...>, const std::index_sequence<Seq2...>)
{
  return std::index_sequence<Seq1..., Seq2...>();
}

// template<std::size_t I, std::size_t... Nums>
// constexpr auto index_sequence_without_helper()
// {
//   if constexpr (!IndexMatch<I, Nums...>()) {
//     return I;
//   }
// }

// template<std::size_t... I, std::size_t... Remove>
// constexpr auto index_sequence_without(const std::index_sequence<I...>, const std::index_sequence<Remove...>)
// {
//   return std::index_sequence< index_sequence_without_helper<I, Remove...>()... >();
// }

// template<std::size_t N, std::size_t... Remove>
// constexpr auto index_sequence_without(const std::index_sequence<Remove...> remove)
// {
//   return index_sequence_without(std::make_index_sequence<N>(), remove);
// }

template<std::size_t I, std::size_t N, std::size_t... Remove, std::size_t... Stored>
constexpr auto index_sequence_without_helper(const std::index_sequence<Stored...> stored)
{
  static_assert(I < N, "index_sequence_without_helper: !(I < N)");
  if constexpr (!IndexMatch<I, Remove...>()) {
    if constexpr (I == (N-1)) {
      return std::index_sequence<Stored..., I>();
    } else {
      return index_sequence_without_helper<I+1, N, Remove...>(std::index_sequence<Stored..., I>());
    }
  }
  else {
    if constexpr (I == (N-1)) {
      return std::index_sequence<Stored...>();
    } else {
      return index_sequence_without_helper<I+1, N, Remove...>(stored);
    }
  }
}

// I is index, N is total size
template<std::size_t I, std::size_t N, std::size_t... Remove>
constexpr auto index_sequence_without_helper()
{
  static_assert(I < N, "index_sequence_without_helper: !(I < N)");
  if constexpr (!IndexMatch<I, Remove...>()) {
    if constexpr (I == (N-1)) {
      return std::index_sequence<I>();
    } else {
      return index_sequence_without_helper<I+1, N, Remove...>(std::index_sequence<I>());
    }
  }
  else {
    if constexpr (I == (N-1)) {
      return;
    } else {
      return index_sequence_without_helper<I+1, N, Remove...>();
    }
  }
}

template<std::size_t N, std::size_t... Remove>
constexpr auto index_sequence_without(const std::index_sequence<Remove...>)
{
  return index_sequence_without_helper<0,N,Remove...>();
}

template<std::size_t N, std::size_t... Remove>
constexpr auto index_sequence_without()
{
  return index_sequence_without_helper<0,N,Remove...>();
}


#endif