#pragma once

#include "camera.h"
#include "matrices.h"
#include "transform.h"

namespace typing {
namespace vectors {
namespace scene {

using namespace stl_types::math;

template<typename T>
FORCEDINLINE tmatrix<T, 4> GenOrthographic(
    geometry::rect<T> const& view, const geometry::range<T>& zfield)
{
    tmatrix<T, 4> mat;

    mat[0][0] = T(2) / (view.right() - view.left());
    mat[1][1] = T(2) / (view.top() - view.bottom());
    mat[2][2] = -T(2) / (zfield.far_ - zfield.near_);

    mat[3][0] = -(view.right() + view.left()) / (view.right() - view.left());
    mat[3][1] = -(view.top() + view.bottom()) / (view.top() - view.bottom());
    mat[3][2] = -(zfield.far_ + zfield.near_) / (zfield.far_ - zfield.near_);

    return mat;
}

template<typename T>
FORCEDINLINE tmatrix<T, 4> GenInfinitePerspective(
    const T& fov, const T& aspect, const T& near_)
{
    tmatrix<T, 4> out;

    T foc = 1. / tan(fov / 2.);

    T e = std::numeric_limits<scalar>::epsilon();

    out[0] = {foc, 0, 0, 0};
    out[1] = {0, foc / aspect, 0, 0};
    out[2] = {0, 0, e - 1., (e - 2) * near_};
    out[3] = {0, 0, -1, 0};

    return out;
}

template<typename T>
FORCEDINLINE tmatrix<T, 4> GenPerspective(
    const T& fov, const T& aspect, const geometry::range<T>& zfield)
{
    CASSERT(abs(aspect - numeric_limits<T>::epsilon()) > T(0));

    tmatrix<T, 4> matrix;

    T thalffov = tan(radians(fov) / T(2));

    matrix[0][0] = T(1) / (aspect * thalffov);
    matrix[1][1] = T(1) / (thalffov);
    matrix[2][2] = -(zfield.far_ + zfield.near_) / (zfield.far_ - zfield.near_);
    matrix[2][3] = T(-1);
    matrix[3][2] =
        (T(-2) * zfield.far_ * zfield.near_) / (zfield.far_ - zfield.near_);

    return matrix;
}

template<typename T>
/*!
 * \brief Generate user clipping plane in perspective, reference for this is an
 * article released by ImgTec for OpenGL ES 2.0 rendering. It was meant for
 * water rendering, but can also be used for VR clipping if you know how to use
 * it. This is meant to replace GL_CLIP_DIST, but this one is more
 * hardware-independent. \param clipPlane \param perspective \param out
 */
FORCEDINLINE void GenUserClipSpace(
    const tvector<T, 4>& clipPlane,
    const tmatrix<T, 4>& perspective,
    tmatrix<T, 4>&       out)
{
    out    = perspective;
    out[3] = clipPlane;
    out[3] += tvector<T, 4>(0, 0, 1, 0);
}

template<typename T>
/*!
 * \brief Along with GenUserClipSpace, corrects the far plane when rendering the
 * user clip space. Scales the output vertex. You should definitely use this.
 * \param clipPlane
 * \param perspective
 * \param corner
 */
FORCEDINLINE void GenUserClipSpaceScale(
    const tvector<T, 4>& clipPlane,
    const tmatrix<T, 4>& perspective,
    tvector<T, 4>&       corner)
{
    corner =
        tvector<T, 4>(signbit(clipPlane.x()), signbit(clipPlane.y()), 1.f, 1.f);

    corner = corner * inverse(perspective);
}

template<typename T>
/*!
 * \brief Generate a standard translate, scale, rotate matrix
 * \param pos
 * \param scl
 * \param rot
 * \return
 */
FORCEDINLINE tmatrix<T, 4> GenTransform(
    tvector<T, 3> const&  pos,
    tvector<T, 3> const&  scl,
    tquaternion<T> const& rot)
{
    tmatrix<T, 4> mat;

    mat = scale(mat, scl);
    mat = mat * matrixify(rot);
    mat = translation(mat, pos);

    return mat;
}

template<typename T>
FORCEDINLINE tmatrix<T, 4> GenOrthographic(camera<T> const& camera)
{
    return GenOrthographic<T>(camera.orthoview, camera.zVals);
}

template<typename T>
FORCEDINLINE tmatrix<T, 4> GenPerspective(camera<T> const& camera)
{
    return GenPerspective<T>(camera.fieldOfView, camera.aspect, camera.zVals);
}

template<typename T>
/*!
 * \brief Shortcut for GenTransform with TSR, but using a data structure.
 * \param transform
 * \return
 */
FORCEDINLINE tmatrix<T, 4> GenTransform(transform<T> const& transform)
{
    return GenTransform<T>(
        transform.position, transform.scale, transform.rotation);
}

template<typename T>
/*!
 * \brief Generate a standard, parameterized camera matrix
 * \param camera
 * \return
 */
FORCEDINLINE tmatrix<T, 4> GenTransform(camera<T> const& camera)
{
    return GenTransform<T>(
        camera.position, tvector<T, 3>(1.0), camera.rotation);
}

template<typename T>
/*!
 * \brief Emulates gluLookAt()
 * \param observer
 * \param up
 * \param subject
 * \return
 */
FORCEDINLINE tmatrix<T, 3> GenLookat(
    const tvector<T, 3>& observer,
    const tvector<T, 3>& up,
    const tvector<T, 3>& subject)
{
    typedef tvector<T, 3> tvec3;
    typedef tmatrix<T, 4> tmat3;

    tvec3 dir  = normalize(subject - observer);
    tvec3 side = cross(dir, up);
    tvec3 dup  = cross(side, dir);

    tmat3 mat;
    mat[0] = side;
    mat[1] = dup;
    mat[2] = dir;

    return mat;
}

} // namespace scene
} // namespace vectors
} // namespace typing
