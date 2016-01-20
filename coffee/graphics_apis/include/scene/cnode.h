#ifndef COFFEE_SCENE_CNODE_H
#define COFFEE_SCENE_CNODE_H

#include <coffee/core/CTypes>
#include <cgraphicsdata.h>

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
    byte_t   name[32];
};

/*!
 * \brief Get complete translation of this node. It is up to the user to check that the hierarchy does not contain loops.
 * \param node
 * \return This node's complete matrix unless the pointer is NULL
 */
extern CMat4 AccumulateTransform(const CNode *node);

}
}

#endif
