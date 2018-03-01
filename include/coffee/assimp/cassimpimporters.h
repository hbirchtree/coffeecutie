#ifndef COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
#define COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H

#include <coffee/core/coffee.h>
#include <coffee/core/CFiles>
#include "cassimptypes.h"

#include <coffee/core/types/vector_types.h>
#include <coffee/core/CObject>

namespace Coffee {

struct _cbasic_mesh;
using Mesh = _cbasic_mesh;

namespace CResourceTypes {
namespace CAssimp {

using namespace Coffee::CResources;

class CAssimpImporters
{
private:
    CAssimpImporters();
public:
    static CAssimpData* importResource(CResources::Resource* source,
                                       cstring hint = nullptr);
};

extern void coffee_assimp_free(CAssimpData* data);

extern cstring assimp_reflexive_string_get(
        const void *basePtr,
        const assimp_reflexive& ref);

extern bool coffee_assimp_dump_mesh(
	CAssimp::CAssimpMesh* mesh, CResources::Resource* resource);

extern byte_t* coffee_assimp_get_reflexive_ptr(void* baseptr, const assimp_reflexive* ref);

} // namespace CAssimp
} // namespace CResourceTypes

namespace CGraphicsData{
struct InplaceNode;
}

namespace ASSIMP {

struct AssimpData;
using AssimpPtr = UqPtr<AssimpData>;

struct Node : public MultiParentObject
{
    Node(Node* parent)
    {
        if(parent)
            parent->addChild(this);
    }

    Matf4 transform;
    i32 mesh;
    u32 flags;
};

struct NodeList : public LinkList<Node>
{
};


extern bool LoadScene(UqPtr<AssimpData> &target,
                      CResources::Resource* source,
                      cstring hint = nullptr);

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

    const cstring objectName;
    const ObjectType type;
};

struct Object
{
    ObjectDesc::ObjectType type;
    union{

    };
};

extern bool GetSceneObjects(UqPtr<AssimpData> const& scene,
                            Vector<ObjectDesc>& objects);

extern bool GetSceneRoot(UqPtr<AssimpData> const& scene,
                         Node** root,
                         NodeList& nodes);

extern bool GetMeshData(UqPtr<AssimpData> const& scene,
                        i32 node,
                        Mesh& output_mesh);

extern bool GetMeshCount(UqPtr<AssimpData> const& scene,
                         i32& count);

}

} // namespace Coffee

#endif // COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
