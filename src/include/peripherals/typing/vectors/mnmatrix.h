#pragma once

#include "vectors.h"

namespace typing {
namespace vectors {
template<typename T, size_t MSize, size_t NSize>
struct tmnmatrix
{
    typedef tvector<T, NSize> col_type;
    static constexpr size_t           size_m = MSize;
    static constexpr size_t           size_n = NSize;
    static constexpr size_t           size   = MSize * NSize;

    col_type d[MSize];

    tmnmatrix(const T& c)
    {
        for(size_t y = 0; y < NSize; y++)
            for(size_t x = 0; x < MSize; x++)
                (*this)[x][y] = c;
    }
    tmnmatrix()
    {
        for(size_t y = 0; y < NSize; y++)
            for(size_t x = 0; x < MSize; x++)
                if(x == y)
                    (*this)[x][y] = T(1);
                else
                    (*this)[x][y] = T(0);
    }
};

}
} // namespace Coffee
