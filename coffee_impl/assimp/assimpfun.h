#ifndef ASSIMPFUN
#define ASSIMPFUN

#include <thread>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/anim.h"
#include "assimp/matrix4x4.h"
#include "assimp/vector3.h"
#include "assimp/color4.h"

#include "cassimptypes.h"
#include "stdint.h"
#include "coffee.h"

namespace Coffee{
namespace CResourceTypes{
namespace CAssimp{

struct assimp_mesh_intermediate
{
    szptr bufferSize	= 0;
    szptr vertices	= 0;
    szptr faces		= 0;

    uint32 bufferCnt	= 0;
};

extern  CAssimpMesh*         importMesh(aiMesh* meshdata);
extern  CAssimpMaterial*     importMaterial(aiMaterial* matdata);
extern  CAssimpLight*        importLight(aiLight* ldata);
extern  CAssimpCamera*       importCamera(aiCamera* camdata);
extern  CAssimpTexture*      importTexture(aiTexture* texdata);
extern  CAssimpAnimation*    importAnimation(aiAnimation* anidata);

template<typename Element>
static szptr coffee_assimp_create_attribute(
        assimp_reflexive* reflex,
        Element* src,
        szptr numElements,
        byte* basePtr)
{

}

/*!
 * \brief Tiny functions that transform Assimp data into Coffee. Returns an offset in the output data buffer which is accumulated to store the data. Don't destroy the stack!
 */
template<typename Element,typename OutType>
using AssimpElementPredicate = szptr(*)(const Element&,OutType*);

static szptr coffee_assimp_convert(const aiVector3D& vec,CVec3* buffer)
{
    buffer->x = vec.x;
    buffer->y = vec.y;
    buffer->z = vec.z;
    return 1;
}
}

extern bool coffee_assimp_dump_mesh(
        CAssimp::CAssimpMesh* mesh, CResources::CResource* resource);

extern void coffee_assimp_free(CAssimp::CAssimpData* data);

}
}

#endif // ASSIMPFUN
