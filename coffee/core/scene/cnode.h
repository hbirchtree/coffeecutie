#ifndef COFFEE_SCENE_CNODE_H
#define COFFEE_SCENE_CNODE_H

#include "coffee/core/CTypes"
#include <coffee/core/graphics/cgraphicsdata.h>

namespace Coffee{
namespace CGraphicsData{

struct CNode
{
    CNode():
        transform(nullptr),
        parent(nullptr)
    {
        name[0] = 0;
    }

    CMat4* transform;
    CNode* parent;
    byte   name[32];
};

/*!
 * \brief Get complete translation of this node
 * \param node
 * \return This node's complete matrix unless the pointer is NULL
 */
extern CMat4 coffee_node_get_transform(const CNode* node);

extern CMat4 coffee_node_glm_get_transform(const CNode* node);

}
}

#endif
