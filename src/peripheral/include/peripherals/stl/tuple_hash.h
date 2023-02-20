#pragma once

#include <tuple>

//  https://stackoverflow.com/a/6894436/916549

template<typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<class... TupleArgs>
struct std::hash<std::tuple<TupleArgs...>>
{
  private:
    //  this is a termination condition
    //  N == sizeof...(TupleTypes)
    //
    template<size_t Idx, typename... TupleTypes>
    inline typename std::enable_if<Idx == sizeof...(TupleTypes), void>::type
    hash_combine_tup(
        size_t& /*seed*/, const std::tuple<TupleTypes...>& /*tup*/) const
    {
    }

    //  this is the computation function
    //  continues till condition N < sizeof...(TupleTypes) holds
    //
    template<size_t Idx, typename... TupleTypes>
        inline typename std::enable_if
        < Idx<sizeof...(TupleTypes), void>::type hash_combine_tup(
            size_t& seed, const std::tuple<TupleTypes...>& tup) const
    {
        hash_combine(seed, std::get<Idx>(tup));

        //  on to next element
        hash_combine_tup<Idx + 1>(seed, tup);
    }

  public:
    size_t operator()(const std::tuple<TupleArgs...>& tupleValue) const
    {
        size_t seed = 0;
        //  begin with the first iteration
        hash_combine_tup<0>(seed, tupleValue);
        return seed;
    }
};
