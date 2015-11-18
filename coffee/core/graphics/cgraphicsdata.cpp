#include "cgraphicsdata.h"

#include "coffee/core/base/cmath_glm.h"

//#define USE_GLM_MATRICES

namespace Coffee{
namespace CGraphicsData{

CGCamera::CGCamera():
    flags(0),
    aspect(1),
    zVals(1.0,100.0),
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
    CMat4 matrix(0);
    CMath::mat4* tmat = (CMath::mat4*)&matrix;
    *tmat = CMath::perspective(CMath::radians(fov),aspect,zfield.fnear,zfield.ffar);
    return matrix;
}

CMat4 _coffee_gen_ortho(const CRectF& viewrect, const CZField& zfield)
{
    CMat4 matrix;
    CMath::mat4* tmat = (CMath::mat4*)&matrix;
    *tmat = CMath::ortho(viewrect.left(),viewrect.right(),
                         viewrect.bottom(),viewrect.top(),
                         zfield.fnear,zfield.ffar);
    return matrix;
}

#ifndef USE_GLM_MATRICES

void _coffee_graphics_rotate_translate(
        CMat4& matrix, const CQuat& rotation, const CVec3& position)
{
    matrix *= CVectors::matrixify(rotation);
    matrix  = CVectors::translation(matrix,position);
}

void coffee_graphics_gen_matrix(CTransform *mat)
{
    mat->matrix  = CVectors::scale(CMat4(),mat->scale);
    mat->matrix *= CVectors::matrixify(mat->rotation);
    mat->matrix  = CVectors::translation(mat->matrix,mat->position);
}

CMat4& coffee_graphics_gen_matrix_perspective(CGCamera *cam)
{
    cam->matrix = coffee_graphics_gen_perspective(
                cam->fieldOfView,cam->aspect,cam->zVals);
    _coffee_graphics_rotate_translate(cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}

CMat4 &coffee_graphics_gen_matrix_orthographic(CGCamera *cam)
{
    cam->matrix = _coffee_gen_ortho(cam->orthoview,cam->zVals);
    _coffee_graphics_rotate_translate(cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}
#else

void _coffee_graphics_rotate_translate(
        CMat4& matrix, const CQuat& rotation, const CVec3& position)
{
    CMath::mat4 tmat = CMath::mat4_cast(*(CMath::quat*)&rotation);
    tmat = CMath::translate(tmat,*(CMath::vec3*)&position);
    matrix = *(CMat4*)&tmat;
}

void coffee_graphics_gen_matrix(CTransform *mat)
{
    CMath::mat4 tmat = CMath::scale(CMath::mat4(),*(CMath::vec3*)&mat->scale);
    tmat *= CMath::mat4_cast(*(CMath::quat*)&mat->rotation);
    tmat = CMath::translate(tmat,*(CMath::vec3*)&mat->position);
    mat->matrix = *(CMat4*)&tmat;
}

CMat4& coffee_graphics_gen_matrix_perspective(CGCamera *cam)
{
    cam->matrix = _coffee_gen_perspective(
                cam->fieldOfView,cam->aspect,cam->zVals);
    _coffee_graphics_rotate_translate(cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}

CMat4 &coffee_graphics_gen_matrix_orthographic(CGCamera *cam)
{
    cam->matrix = _coffee_gen_ortho(cam->orthoview,cam->zVals);
    _coffee_graphics_rotate_translate(cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}

#endif

}
}
