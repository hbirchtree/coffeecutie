#include <coffee/graphics_apis/scene/cnode.h>

namespace Coffee{
namespace CGraphicsData{

CMat4 AccumulateTransform(const CNode *node)
{
    if(!node || !node->transform)
        return CMat4();

    return (*node->transform)*AccumulateTransform(node->parent);
}

}
}
