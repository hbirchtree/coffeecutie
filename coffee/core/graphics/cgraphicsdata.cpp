#include "cgraphicsdata.h"

namespace Coffee{
namespace CGraphicsData{

CGCamera::CGCamera():
    flags(0),
    aspect(1),
    zVals(0.1,100.0),
    fieldOfView(90),
    position(0,0,0),
    rotation(2,0,0,0),
    orthoview(0,0,1,1)
{
}

CTransform::CTransform():
    matrix(0.0),
    rotation(2,0,0,0),
    position(0),
    scale(1.0)
{
}

void coffee_graphics_gen_matrix(CTransform *mat)
{
    mat->matrix = CMath::scale(CMath::mat4(),mat->scale);
    mat->matrix = CMath::translate(mat->matrix,mat->position) * CMath::mat4_cast(mat->rotation);
}

void _coffee_graphics_rotate_translate(
        CMath::mat4* matrix, const CMath::quat& rotation, const CMath::vec3& position)
{
    *matrix *= CMath::mat4_cast(rotation);
    *matrix = CMath::translate(*matrix,position);
}

glm::mat4& coffee_graphics_gen_matrix_perspective(CGCamera *cam)
{
    cam->matrix = CMath::perspective(
                CMath::radians(cam->fieldOfView), cam->aspect,
                cam->zVals.near, cam->zVals.far);
    _coffee_graphics_rotate_translate(&cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}

glm::mat4& coffee_graphics_gen_matrix_orthographic(CGCamera *cam)
{
    cam->matrix = CMath::ortho(cam->orthoview.x,cam->orthoview.w,
                          cam->orthoview.y,cam->orthoview.h);
    _coffee_graphics_rotate_translate(&cam->matrix,cam->rotation,cam->position);
    return cam->matrix;
}

}
}
