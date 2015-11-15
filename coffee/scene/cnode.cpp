#include "cnode.h"

#include "coffee/core/base/cmath_glm.h"

namespace Coffee{
namespace CGraphicsData{

CMat4 _coffee_math_copy_mat4(const CMath::mat4& mat)
{
    CMat4 ret;

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            ret[i][j] = mat[i][j];

    return ret;
}

/*!
 * \brief This function is used behind the scenes by coffee_node_get_transform()
 * \param node
 * \return
 */
CMath::mat4 _coffee_node_get_transform(const CNode* node)
{
    if(!node || !node->transform)
        return CMath::mat4(1);

    CMath::mat4* matrix = (CMath::mat4*)node->transform;

    return _coffee_node_get_transform(node->parent)*(*matrix);
}

CMat4 coffee_node_get_transform(const CNode *node)
{
    if(!node || !node->transform)
        return CMat4(1);

    return coffee_node_get_transform(node->parent) * (*node->transform);

//    return _coffee_math_copy_mat4(_coffee_node_get_transform(node));
}

}
}
