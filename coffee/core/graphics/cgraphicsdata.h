#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include "coffee/core/coffee.h"
#include "coffee/core/base/cmath.h"

namespace Coffee{
namespace CGraphicsData{

struct CGCamera
{
    CGCamera():
        flags(0),
        aspect(1),
        fieldOfView(90),
        position(0,0,0),
        rotation(2,0,0,0),
        orthoview(0,0,1,1)
    {
    }

    enum CameraFlags{
        OrthographicFlag = 0x1, //When off, assume perspective
    };

    uint8   flags;

    scalar  aspect;
    CZField zVals;
    scalar  fieldOfView;

    CMath::vec3 position;
    CMath::quat rotation;

    CRectF orthoview;

    CMath::mat4 matrix;
};

struct CTransform
{
    CMath::mat4 matrix;
    CMath::quat rotation;
    CMath::vec3 position;
    CMath::vec3 scale;
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
extern glm::mat4 &coffee_graphics_gen_matrix_perspective(CGCamera* cam);
/*!
 * \brief Update orthographic matrix for camera
 * \param cam
 */
extern glm::mat4 &coffee_graphics_gen_matrix_orthographic(CGCamera* cam);

} //CGraphicsData
} //Coffee

#endif // CGRAPHICSDATA_H
