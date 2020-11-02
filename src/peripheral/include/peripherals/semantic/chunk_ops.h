#pragma once

#include "chunk.h"
#include <peripherals/stl/types.h>

namespace semantic {
namespace chunk_ops {

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        !is_specialized<T2, ::semantic::mem_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2& target)
{
    std::copy(src.begin(), src.end(), std::back_insert_iterator<T2>(target));
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        is_specialized<T2, ::semantic::mem_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2& target)
{
    std::copy(
        src.begin(),
        src.end(),
        std::insert_iterator<T2>(target, target.begin()));
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        is_specialized<T2, ::semantic::mem_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2&& target)
{
    MemCpy(src, target);
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        is_specialized<T2, ::semantic::mem_chunk>::value>::type* = nullptr>
FORCEDINLINE bool MemCmp(T1 const& a1, T2 const& a2)
{
    return std::equal(a1.begin(), a1.end(), a2.begin());

    /* TODO: Replace with std::equal() for C++14 and up */

    //    auto it_a1 = a1.begin();
    //    auto it_a2 = a2.begin();

    //    while(it_a1 != a1.end() && it_a2 != a2.end())
    //    {
    //        /* First case of being different: different value */
    //        if(*it_a1 != *it_a2)
    //            return false;

    //        ++it_a1, ++it_a2;
    //    }

    //    /* Second case: one container is shorter */
    //    return it_a1 == a1.end() && it_a2 == a2.end();
}

template<typename T>
FORCEDINLINE void MemClear(T& mem)
{
    std::fill(mem.begin(), mem.end(), typename T::value_type());
}

template<typename T>
FORCEDINLINE void MemClear(T&& mem)
{
    std::fill(mem.begin(), mem.end(), typename T::value_type());
}

} // namespace chunk_ops
} // namespace semantic
