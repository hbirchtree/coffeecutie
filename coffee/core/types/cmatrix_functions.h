#ifndef COFFEE_GRAPHICS_FUNCTIONS_MATRICES_H
#define COFFEE_GRAPHICS_FUNCTIONS_MATRICES_H

#include "coffee/core/coffee_macros.h"
#include "coffee/core/types/vector_types.h"

namespace Coffee{
namespace CGraphicsData{

using namespace CVectors;

template<typename T>
_cbasic_tmatrix<T,4> coffee_graphics_gen_orthographic(
        _cbasic_rect<T> const& view, const _cbasic_zfield<T>& zfield)
{
    _cbasic_tmatrix<T,4> mat;

    mat[0][0] = T(2)/(view.right()-view.left());
    mat[1][1] = T(2)/(view.top()-view.bottom());
    mat[2][2] = T(2)/(zfield.far-zfield.near);

    mat[3][0] = -(view.right()+view.left())/(view.right()-view.left());
    mat[3][1] = -(view.top()+view.bottom())/(view.top()-view.bottom());
    mat[3][2] = -(zfield.far+zfield.near)/(zfield.far-zfield.near);

    return mat;
}

template<typename T>
_cbasic_tmatrix<T,4> coffee_graphics_gen_perspective(
        const T& fov,
        const T& aspect,
        const _cbasic_zfield<T>& zfield)
{
    CASSERT(CMath::abs(aspect - CMath::numeric_limits<T>::epsilon()) > T(0));

    CVectors::_cbasic_tmatrix<T,4> matrix(0);

    T thalffov = CMath::tan(CMath::radians(fov)/T(2));

    matrix[0][0] = T(1)/(aspect*thalffov);
    matrix[1][1] = T(1)/(thalffov);
    matrix[2][2] = -(zfield.far+zfield.near)/(zfield.far-zfield.near);
    matrix[2][3] = T(-1);
    matrix[3][2] = (T(-2)*zfield.far*zfield.near)/(zfield.far-zfield.near);

    return matrix;
}

template<typename T>
_cbasic_tmatrix<T,4> coffee_graphics_gen_transform(
        _cbasic_vec3<T> const& pos,
        _cbasic_vec3<T> const& scl,
        _cbasic_tquaternion<T> const& rot)
{
    _cbasic_tmatrix<T,4> mat;

    mat = CVectors::matrixify(rot);
    mat = CVectors::scale(mat,scl);
    mat = CVectors::translation(mat,pos);

    return mat;
}

template<typename T>
_cbasic_tmatrix<T,4> coffee_graphics_gen_orthographic(
        _cbasic_graphics_camera<T> const& camera)
{
    return coffee_graphics_gen_orthographic(
                camera.orthoview,camera.zVals);
}

template<typename T>
_cbasic_tmatrix<T,4> coffee_graphics_gen_perspective(
        _cbasic_graphics_camera<T> const& camera)
{
    return coffee_graphics_gen_perspective(
                camera.fieldOfView,
                camera.aspect,
                camera.zVals);
}

template<typename T>
_cbasic_tmatrix<T,4> coffee_graphics_gen_transform(
        _cbasic_graphics_transform<T> const& transform)
{
    return coffee_graphics_gen_transform(
                transform.position,
                transform.scale,
                transform.rotation);
}

template<typename T>
_cbasic_tmatrix<T,4> coffee_graphics_gen_transform(
        _cbasic_graphics_camera<T> const& camera)
{
    return coffee_graphics_gen_transform(
                camera.position,
                CVec3(1.0),
                camera.rotation);
}

}
}

#endif
