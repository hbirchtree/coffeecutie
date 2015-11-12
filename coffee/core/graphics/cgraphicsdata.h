#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include "coffee/core/coffee.h"
#include "coffee/core/base/cmath.h"

namespace Coffee{
namespace CGraphicsData{

struct CGCamera
{
    CGCamera();

    enum CameraFlags{
        OrthographicFlag = 0x1, //When off, assume perspective
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

} //CGraphicsData
} //Coffee

#endif // CGRAPHICSDATA_H
