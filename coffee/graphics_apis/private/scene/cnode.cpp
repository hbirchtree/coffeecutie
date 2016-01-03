#include "scene/cnode.h"

namespace Coffee{
namespace CGraphicsData{

CMat4 coffee_node_get_transform(const CNode *node)
{
    if(!node || !node->transform)
        return CMat4();

    return (*node->transform)*coffee_node_get_transform(node->parent);
}

}
}
