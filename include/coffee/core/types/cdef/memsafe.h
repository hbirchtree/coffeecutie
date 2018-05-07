#pragma once

#include <iterator>

#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee {

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        !is_specialized<T2, _cbasic_data_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2& target)
{
    std::copy(src.begin(), src.end(), std::back_insert_iterator<T2>(target));
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        is_specialized<T2, _cbasic_data_chunk>::value>::type* = nullptr>
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
        is_specialized<T2, _cbasic_data_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2&& target)
{
    MemCpy(src, target);
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        is_specialized<T2, _cbasic_data_chunk>::value>::type* = nullptr>
FORCEDINLINE bool MemCmp(T1 const& a1, T2 const& a2)
{
    auto it_a1 = a1.begin();
    auto it_a2 = a2.begin();

    while(it_a1 != a1.end() && it_a2 != a2.end())
    {
        /* First case of being different: different value */
        if(*it_a1 != *it_a2)
            return false;

        ++it_a1, ++it_a2;
    }

    /* Second case: one container is shorter */
    return it_a1 == a1.end() && it_a2 == a2.end();
}

template<typename T>
FORCEDINLINE void MemClear(T& mem)
{
    auto it = mem.begin();

    for(;it != mem.end(); ++it)
    {
        *it = T();
    }
}

} // namespace Coffee
