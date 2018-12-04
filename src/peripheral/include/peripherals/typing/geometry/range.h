#pragma once

#include <peripherals/stl/type_safety.h>

namespace typing {
namespace geometry {

using namespace type_safety;

template<
    typename T,

    typename type_safety::is_pod<T>::type* = nullptr

    >
struct range
{
    range() : near_(0.1), far_(10)
    {
    }
    range(T vnear, T vfar) : near_(vnear), far_(vfar)
    {
    }

    T near_;
    T far_;
};

}
}
