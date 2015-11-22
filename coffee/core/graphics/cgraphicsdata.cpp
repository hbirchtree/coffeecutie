#include "cgraphicsdata.h"

#include "coffee/core/base/cmath_glm.h"

#define USE_GLM_MATRICES

namespace Coffee{
namespace CGraphicsData{

CMat4 _coffee_gen_perspective(scalar fov, scalar aspect, const CZField& zfield)
{
    CMat4 matrix(0);
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


void _coffee_graphics_rotate_translate(
        CMat4& matrix, const CQuat& rotation, const CVec3& position)
{
    if(coffee_vectors_use_glm_operations)
    {
        CMath::mat4* tmat = (CMath::mat4*)&matrix;
        *tmat *= CMath::mat4_cast(*(CMath::quat*)&rotation);
        *tmat = CMath::translate(*tmat,*(CMath::vec3*)&position);
    }else{
        matrix *= CVectors::matrixify(rotation);
        matrix  = CVectors::translation(matrix,position);
    }
}

}
}
