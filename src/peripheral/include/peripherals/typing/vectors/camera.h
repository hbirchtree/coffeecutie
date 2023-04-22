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
    FORCEDINLINE camera_base() : rotation(), position(T(0)), zVals(T(1), T(100))
    {
    }

    tquaternion<T>     rotation;
    tvector<T, 3>      position;
    geometry::range<T> zVals;
};

template<typename T>
struct camera : camera_base<T>
{
    FORCEDINLINE camera() : fieldOfView(T(85)), aspect(T(1))
    {
    }

    T fieldOfView;
    T aspect;
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
