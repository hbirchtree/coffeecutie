#ifndef COFFEE_SCENE_CNODE_H
#define COFFEE_SCENE_CNODE_H

#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/vector_types.h>

namespace Coffee{
namespace CGraphicsData{

struct Node
{
    FORCEDINLINE Node():
        transform(nullptr),
        parent(nullptr)
    {
        name[0] = 0;
    }

    CMat4* transform;
    Node* parent;
    byte_t name[32];
};

struct InplaceNode
{
    Matf4 transform;
    InplaceNode* parent;
    cstring name;
};

/*TODO: Transitional! */
using CNode = Node;

template<typename Key>
struct NodeHierarchy
{
    enum ErrorState
    {
        NoError = 0,
        InvalidHierarchy = 0x1,
        LoopingError = 0x2,
        RootNotFound = 0x4,
    };

private:
    Node* m_root;
    Map<Key,Matf4> m_matrices;
    Map<Key,Node> m_container;

public:
    /*!
     * \brief Create a new node with key k, adding it to a parent if it exists and is not itself
     * \param k
     * \param parent
     */
    void createNode(Key k, Key parent)
    {
        m_container[k] = Node();
        m_matrices[k] = Matf4();
        m_container[k].transform = &m_matrices[k];

        if(parent != k && m_container.find(k)!=m_container.end())
            m_container[k].parent = &m_container[parent];
    }

    /*!
     * \brief Resolves node hierarchy, defining a root. Things it will do:
     *  - Delete rootless nodes (ones not connected to the true root)
     *  - Purge loops (purge!)
     */
    bool validateTree(u32* state)
    {
        /* Future improvement:
         *  - Remove degenerate nodes (looping nodes)
         *  - Remove rootless nodes and their ancestors
         *  - Save traversed paths and stop when they are detected
         */

        Node* ref;
        bool bad_flag;
        bool found_root = false;

        /* Iterate through all nodes. Might do some graph theory on this later */
        /* TODO: Optimize this process, might halt on slow systems */
        for(auto k : m_container)
        {
            /* We start with the node itself */
            ref = &k.second;
            bad_flag = false;
            while(ref != nullptr)
            {
                /* If we have detected a loop, stop, hammertime */
                /* If a looping error is detected during debug process, you should find it and remove it */
                /* Possible optimization: track which parts of the tree we traversed, and ignore them */
                if(ref->parent == &k.second)
                {
                    (*state) |= LoopingError;
                    ref->parent = nullptr;
                    bad_flag = true;
                }

                /* If all is good, and the current node has no parent, it's the root */
                if(!bad_flag && ref->parent==nullptr)
                {
                    if(found_root)
                    {
                        /* Conflicted root! */
                        (*state) |= InvalidHierarchy;
                        return false;
                    }else
                    {
                        found_root = true;
                        m_root = ref;
                    }
                }

                /* Move on to next node */
                ref = ref->parent;
            }
        }

        /* If no root was determined, we have to tell you */
        if(!m_root)
            (*state) |= RootNotFound;

        return true;
    }

    Node& root()
    {
        return *m_root;
    }
    Node& node(Key k)
    {
        return m_container[k];
    }
};

/*!
 * \brief Get complete translation of this node. It is up to the user to check that the hierarchy does not contain loops.
 * \param node
 * \return This node's complete matrix unless the pointer is NULL
 */
STATICINLINE CMat4 AccumulateTransform(const CNode *node)
{
    if(!node || !node->transform)
        return CMat4();

    return (*node->transform)*AccumulateTransform(node->parent);
}

}
}

#endif
