#ifndef COFFEE_TYPES_GRAPHICS_TYPES_H
#define COFFEE_TYPES_GRAPHICS_TYPES_H

#include "composite_types.h"
#include "vector_types.h"

namespace Coffee{
namespace CGraphicsData{

template<typename T> struct _cbasic_graphics_camera
{
    _cbasic_graphics_camera():
        flags(CameraFlags::Perspective),
        aspect(T(1)),
        zVals(T(1),T(100)),
        fieldOfView(T(60)),
        position(T(0)),
        rotation(),
        orthoview(T(0),T(0),T(1),T(1))
    {
    }

    enum class CameraFlags : uint8{
        Perspective = 0x0,
        Orthographic = 0x1,
    };

    CameraFlags flags;

    T aspect;
    _cbasic_zfield<T> zVals;
    T fieldOfView;

    _cbasic_vec3<T> position;
    CVectors::_cbasic_tquaternion<T> rotation;

    _cbasic_rect<T> orthoview;
};

template<typename T> struct _cbasic_graphics_transform
{
    _cbasic_graphics_transform():
        rotation(),
        position(0),
        scale(1.0)
    {
    }

    CVectors::_cbasic_tquaternion<T> rotation;
    _cbasic_vec3<T> position;
    _cbasic_vec3<T> scale;
};

}
}

#endif
