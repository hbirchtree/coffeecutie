#pragma once

#include <peripherals/stl/type_safety.h>

namespace typing {
namespace graphics {

template<typename T>
union field
{
    field(T n = T(), T f = T())
        : near_(n)
        , far_(f)
    {
    }

    struct
    {
        T near_, far_;
    };

    T data[2];

    T range() const
    {
        return far_ - near_;
    }

    template<typename T2>
    field<T2> convert()
    {
        return field<T2>(C_FCAST<T2>(near_), C_FCAST<T2>(far_));
    }
};

} // namespace graphics
} // namespace typing
