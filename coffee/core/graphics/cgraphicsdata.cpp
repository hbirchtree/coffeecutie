#include "cgraphicsdata.h"

#include "coffee/core/base/cmath_glm.h"

namespace Coffee{
namespace CGraphicsData{

CGCamera::CGCamera():
    flags(0),
    aspect(1),
    zVals(0.1,100.0),
    fieldOfView(90),
    position(0,0,0),
    rotation(),
    orthoview(0,0,1,1)
{
}

CTransform::CTransform():
    matrix(0.0),
    rotation(),
    position(0),
    scale(1.0)
{
}

CMat4 _coffee_gen_perspective(scalar fov, scalar aspect, const CZField& zfield)
{
    CMat4 matrix;
    CMath::mat4* tmat = (CMath::mat4*)&matrix;
    *tmat = CMath::perspective(CMath::radians(fov),aspect,zfield.near,zfield.far);
    return matrix;
}

CMat4 _coffee_gen_ortho(const CRectF& viewrect, const CZField& zfield)
{
    CMat4 matrix;
    CMath::mat4* tmat = (CMath::mat4*)&matrix;
    *tmat = CMath::ortho(viewrect.left(),viewrect.right(),
                         viewrect.bottom(),viewrect.top(),
                         zfield.near,zfield.far);
    return matrix;
}

void coffee_graphics_gen_matrix(CTransform *mat)
{
    mat->matrix = CVectors::scale(CMat4(),mat->scale);
    mat->matrix = CVectors::translate(mat->matrix,mat->position) * CVectors::matrixify(mat->rotation);
}

void _coffee_graphics_rotate_translate(
        CMat4* matrix, const CQuat& rotation, const CVec3& position)
{
    *matrix = (*matrix) * CVectors::matrixify(rotation);
    *matrix = CVectors::translate(*matrix,position);
}

CMat4& coffee_graphics_gen_matrix_perspective(CGCamera *cam)
{
    cam->matrix = _coffee_gen_perspective(cam->fieldOfView,cam->aspect,cam->zVals);
    _coffee_graphics_rotate_translate(&cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}

CMat4 &coffee_graphics_gen_matrix_orthographic(CGCamera *cam)
{
    cam->matrix = _coffee_gen_ortho(cam->orthoview,cam->zVals);
    _coffee_graphics_rotate_translate(&cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}

}
}
