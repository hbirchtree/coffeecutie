#ifndef COFFEE_GRAPHICS_FUNCTIONS_MATRICES_H
#define COFFEE_GRAPHICS_FUNCTIONS_MATRICES_H

#include <coffee/core/types/composite_types.h>

namespace Coffee{
namespace CGraphicsData{

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> GenOrthographic(
        _cbasic_rect<T> const& view, const _cbasic_zfield<T>& zfield)
{
    _cbasic_tmatrix<T,4> mat;

    mat[0][0] = T(2)/(view.right()-view.left());
    mat[1][1] = T(2)/(view.top()-view.bottom());
    mat[2][2] = -T(2)/(zfield.far-zfield.near);

    mat[3][0] = -(view.right()+view.left())
                /(view.right()-view.left());
    mat[3][1] = -(view.top()+view.bottom())
                /(view.top()-view.bottom());
    mat[3][2] = -(zfield.far+zfield.near)
                /(zfield.far-zfield.near);

    return mat;
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> GenPerspective(
        const T& fov,
        const T& aspect,
        const _cbasic_zfield<T>& zfield)
{
    CASSERT(CMath::abs(aspect - CMath::numeric_limits<T>::epsilon()) > T(0));

    _cbasic_tmatrix<T,4> matrix;

    T thalffov = CMath::tan(CMath::radians(fov)/T(2));

    matrix[0][0] = T(1)/(aspect*thalffov);
    matrix[1][1] = T(1)/(thalffov);
    matrix[2][2] = -(zfield.far+zfield.near)/(zfield.far-zfield.near);
    matrix[2][3] = T(-1);
    matrix[3][2] = (T(-2)*zfield.far*zfield.near)/(zfield.far-zfield.near);

    return matrix;
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> GenTransform(
        _cbasic_vec3<T> const& pos,
        _cbasic_vec3<T> const& scl,
        _cbasic_tquaternion<T> const& rot)
{
    _cbasic_tmatrix<T,4> mat;

    mat = matrixify(rot);
    mat = scale(mat,scl);
    mat = translation(mat,pos);

    return mat;
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> GenOrthographic(
        _cbasic_graphics_camera<T> const& camera)
{
    return GenOrthographic(
                camera.orthoview,camera.zVals);
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> GenPerspective(
        _cbasic_graphics_camera<T> const& camera)
{
    return GenPerspective(
                camera.fieldOfView,
                camera.aspect,
                camera.zVals);
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> GenTransform(
        _cbasic_graphics_transform<T> const& transform)
{
    return GenTransform(
                transform.position,
                transform.scale,
                transform.rotation);
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,4> GenTransform(
        _cbasic_graphics_camera<T> const& camera)
{
    return GenTransform(
                camera.position,
                _cbasic_vec3<T>(1.0),
                camera.rotation);
}

template<typename T>
FORCEDINLINE _cbasic_tmatrix<T,3> GenLookat(
        const _cbasic_tvector<T,3>& observer,
        const _cbasic_tvector<T,3>& up,
        const _cbasic_tvector<T,3>& subject)
{
    typedef _cbasic_tvector<T,3> tvec3;
    typedef _cbasic_tmatrix<T,4> tmat3;

    tvec3 dir = normalize(subject-observer);
    tvec3 side = cross(dir,up);
    tvec3 dup = cross(side,dir);

    tmat3 mat;
    mat[0] = side;
    mat[1] = dup;
    mat[2] = dir;

    return mat;
}

}
}

#endif
