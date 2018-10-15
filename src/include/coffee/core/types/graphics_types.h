#pragma once

#include "cdef/geometry.h"
#include "vector_types.h"

namespace Coffee {
namespace SceneGraph {

enum CameraType
{
    Perspective,
    Orthographic,
};

template<typename T>
struct _cbasic_camera_base
{
    FORCEDINLINE _cbasic_camera_base() :
        rotation(), position(T(0)), zVals(T(1), T(100))
    {
    }

    _cbasic_tquaternion<T> rotation;
    _cbasic_vec3<T>        position;
    _cbasic_range<T>       zVals;
};

template<typename T>
struct _cbasic_camera : _cbasic_camera_base<T>
{
    FORCEDINLINE _cbasic_camera() : fieldOfView(T(85)), aspect(T(1))
    {
    }

    T fieldOfView;
    T aspect;
};

template<typename T>
struct _cbasic_camera_ortho : _cbasic_camera_base<T>
{
    FORCEDINLINE _cbasic_camera_ortho() : orthoview(T(0), T(0), T(1), T(1))
    {
    }

    _cbasic_rect<T> orthoview;
};

template<typename T>
using _cbasic_graphics_camera = _cbasic_camera<T>;

template<typename T>
using camera = _cbasic_camera<T>;

template<typename T>
using camera_ortho = _cbasic_camera_ortho<T>;

template<typename T>
struct _cbasic_graphics_transform
{
    FORCEDINLINE _cbasic_graphics_transform() :
        rotation(), position(0), scale(1.0)
    {
    }

    _cbasic_tquaternion<T> rotation;
    _cbasic_vec3<T>        position;
    _cbasic_vec3<T>        scale;
};

} // namespace SceneGraph
} // namespace Coffee
