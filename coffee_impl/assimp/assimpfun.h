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

extern  CAssimpMesh*         importMesh(const aiMesh *meshdata);
extern  CAssimpMaterial*     importMaterial(aiMaterial* matdata);
extern  CAssimpLight*        importLight(aiLight* ldata);
extern  CAssimpCamera*       importCamera(aiCamera* camdata);
extern  CAssimpTexture*      importTexture(aiTexture* texdata);
extern  CAssimpAnimation*    importAnimation(aiAnimation* anidata);

/*!
 * \brief Transforms data of type Element to type OutType, returning the size of data written
 */
template<typename Element>
using AssimpTransformFun = szptr(const Element&,byte*);

extern szptr _assimp_face_transform(const aiFace& v,byte* d);
extern szptr _assimp_vec_transform(const aiVector3D& v,byte* d);
extern szptr _assimp_col_transform(const aiColor4D& v, byte* d);

extern byte* coffee_assimp_get_reflexive_ptr(void* baseptr, const assimp_reflexive* ref);

template<typename Element>
void coffee_assimp_mesh_attribute_process(
        Element* input, szptr numElements, byte* basePtr,
        AssimpTransformFun<Element> fun)
{
    std::cout << "thread:" << std::this_thread::get_id() << "ptr:" << (void*)basePtr << " " << sizeof(Element) << std::endl;
    szptr offset = 0;
    for(szptr i=0;i<numElements;i++)
        offset+=fun(input[i],&basePtr[offset]);
    std::cout << "Finish: thread:" << std::this_thread::get_id() << "ptr:" << (void*)basePtr << std::endl;
}

}

extern bool coffee_assimp_dump_mesh(
        CAssimp::CAssimpMesh* mesh, CResources::CResource* resource);

extern void coffee_assimp_free(CAssimp::CAssimpData* data);

}
}

#endif // ASSIMPFUN
