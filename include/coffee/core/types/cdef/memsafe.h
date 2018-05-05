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
    std::copy(
        src.begin(),
        src.end(),
        std::insert_iterator<T2>(target, target.begin()));
}

} // namespace Coffee
