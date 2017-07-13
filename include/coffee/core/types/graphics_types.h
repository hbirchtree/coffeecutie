#ifndef COFFEE_TYPES_GRAPHICS_TYPES_H
#define COFFEE_TYPES_GRAPHICS_TYPES_H

#include "cdef/geometry.h"
#include "vector_types.h"

namespace Coffee{
namespace CGraphicsData{

template<typename T> struct _cbasic_graphics_camera
{
    FORCEDINLINE _cbasic_graphics_camera():
        rotation(),
        orthoview(T(0),T(0),T(1),T(1)),
        position(T(0)),
        zVals(T(1),T(100)),
        fieldOfView(T(85)),
        aspect(T(1)),
        flags(CameraFlags::Perspective)
    {
    }

    enum class CameraFlags : uint8{
        Perspective = 0x0,
        Orthographic = 0x1,
    };

    _cbasic_tquaternion<T> rotation;
    _cbasic_rect<T> orthoview;
    _cbasic_vec3<T> position;

    _cbasic_range<T> zVals;

    T fieldOfView;
    T aspect;

    CameraFlags flags;
};

template<typename T> struct _cbasic_graphics_transform
{
    FORCEDINLINE _cbasic_graphics_transform():
        rotation(),
        position(0),
        scale(1.0)
    {
    }

    _cbasic_tquaternion<T> rotation;
    _cbasic_vec3<T> position;
    _cbasic_vec3<T> scale;
};

}
}

#endif
