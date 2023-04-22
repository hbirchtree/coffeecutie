#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>

#include <coffee/core/CObject>
#include <coffee/core/types/vector_types.h>

namespace Coffee {

struct memory_mesh;
using Mesh = memory_mesh;

namespace ASSIMP {

struct AssimpData;
using AssimpPtr = std::unique_ptr<AssimpData>;

struct Node : public MultiParentObject
{
    Node(Node* parent)
    {
        if(parent)
            parent->addChild(this);
    }

    Matf4 transform; /*!< Transform of the current node, not accumulated */
    i32   mesh;      /*!< Connected mesh ID, -1 if "virtual" */
    u32   flags;     /*!< User flags */
};

struct NodeList : public LinkList<Node>
{
};

extern bool LoadScene(
    std::unique_ptr<AssimpData>& target, Bytes const& source, cstring hint = nullptr);

struct ObjectDesc
{
    enum ObjectType
    {
        Undefined,
        Mesh,
        Light,
        Animation,
        Camera,
    };

    const cstring    objectName;
    const ObjectType type;
};

struct Object
{
    ObjectDesc::ObjectType type;
    union
    {
    };
};

extern bool GetSceneObjects(
    std::unique_ptr<AssimpData> const& scene, Vector<ObjectDesc>& objects);

/*!
 * \brief Get pre-multiplied scene nodes.
 *  It is not necessary to accumulate the scene matrices with
 *   their parents with this method.
 * \param scene
 * \param root destination for the root node. Is also nodes[0] object.
 * \param nodes
 * \return
 */
extern bool GetSceneRoot(
    std::unique_ptr<AssimpData> const& scene, Node** root, NodeList& nodes);

/*!
 * \brief Like GetSceneRoot(), but does not accumulate matrices.
 * \param scene
 * \param root
 * \param nodes
 * \return
 */
extern bool GetRawSceneRoot(
    std::unique_ptr<AssimpData> const& scene, Node** root, NodeList& nodes);

extern bool GetMeshData(
    std::unique_ptr<AssimpData> const& scene, i32 node, Mesh& output_mesh);

extern bool GetMeshCount(std::unique_ptr<AssimpData> const& scene, i32& count);

} // namespace ASSIMP

} // namespace Coffee
