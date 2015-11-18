#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include "coffee/core/coffee.h"
#include "coffee/core/base/cmath.h"

namespace Coffee{
namespace CGraphicsData{

struct CGCamera
{
    CGCamera();

    enum CameraFlags : uint8{
        OrthographicFlag = 0x1,
    };

    uint8   flags;

    scalar  aspect;
    CZField zVals;
    scalar  fieldOfView;

    CVec3 position;
    CQuat rotation;

    CRectF orthoview;

    CMat4 matrix;
};

struct CTransform
{
    CTransform();

    CMat4 matrix;
    CQuat rotation;
    CVec3 position;
    CVec3 scale;
};

extern CMat4 _coffee_gen_perspective(scalar fov, scalar aspect, const CZField& zfield);

/*!
 * \brief Update a model transform matrix
 * \param mat
 */
extern void coffee_graphics_gen_matrix(CTransform* mat);
/*!
 * \brief Update projection matrix for camera
 * \param cam
 */
extern CMat4 &coffee_graphics_gen_matrix_perspective(CGCamera* cam);
/*!
 * \brief Update orthographic matrix for camera
 * \param cam
 */
extern CMat4 &coffee_graphics_gen_matrix_orthographic(CGCamera* cam);

template<typename T>
CVectors::_cbasic_tmatrix<T,4>
coffee_graphics_gen_perspective(
        const T& fov,
        const T& aspect,
        const _cbasic_zfield<T>& zfield)
{
    CASSERT(CMath::abs(aspect - CMath::numeric_limits<T>::epsilon()) > T(0));

    CVectors::_cbasic_tmatrix<T,4> matrix(0);

    T thalffov = CMath::tan(CMath::radians(fov)/T(2));

    matrix[0][0] = T(1)/(aspect*thalffov);
    matrix[1][1] = T(1)/(thalffov);
    matrix[2][2] = (-zfield.zfar-zfield.znear)/(zfield.zfar-zfield.znear);
    matrix[2][3] = T(-1);
    matrix[3][2] = (T(-2)*zfield.zfar*zfield.znear)/(zfield.zfar-zfield.znear);

    return matrix;
}

} //CGraphicsData
} //Coffee

#endif // CGRAPHICSDATA_H
