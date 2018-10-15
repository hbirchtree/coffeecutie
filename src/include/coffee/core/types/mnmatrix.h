#pragma once

#include "vectors.h"

namespace Coffee {
template<typename T, size_t MSize, size_t NSize>
struct _cbasic_tmnmatrix
{
    typedef _cbasic_tvector<T, NSize> col_type;
    static constexpr size_t           size_m = MSize;
    static constexpr size_t           size_n = NSize;
    static constexpr size_t           size   = MSize * NSize;

    col_type d[MSize];

    _cbasic_tmnmatrix(const T& c)
    {
        for(size_t y = 0; y < NSize; y++)
            for(size_t x = 0; x < MSize; x++)
                (*this)[x][y] = c;
    }
    _cbasic_tmnmatrix()
    {
        for(size_t y = 0; y < NSize; y++)
            for(size_t x = 0; x < MSize; x++)
                if(x == y)
                    (*this)[x][y] = T(1);
                else
                    (*this)[x][y] = T(0);
    }
};
} // namespace Coffee
