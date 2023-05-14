#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/typing/geometry/range.h>
#include <peripherals/typing/geometry/rect.h>

#include <peripherals/typing/vectors/vector_types.h>

namespace typing {
namespace vectors {
namespace scene {

template<typename T>
struct camera_base
{
    tquaternion<T>     rotation{};
    tvector<T, 3>      position{};
    geometry::range<T> zVals{T(1), T(100)};
};

template<typename T>
struct camera : camera_base<T>
{
    T fieldOfView{85};
    T aspect{1};
};

template<typename T>
struct camera_ortho : camera_base<T>
{
    FORCEDINLINE camera_ortho() : orthoview(T(0), T(0), T(1), T(1))
    {
    }

    geometry::rect<T> orthoview;
};

} // namespace scene
} // namespace vectors
} // namespace typing
