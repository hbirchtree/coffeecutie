#ifndef ASSIMPFUN
#define ASSIMPFUN

#include <thread>
#include <stdint.h>

#include <coffee/core/coffee.h>
#include <coffee/assimp/cassimptypes.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/color4.h>

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

extern CAssimpMesh* importMesh(
        const aiMesh *meshdata);

/*!
 * \brief Transforms data of type Element to type OutType, returning the size of data written
 */
template<typename Element>
using AssimpTransformFun = szptr(const Element&,byte_t*);

template<typename Element>
void coffee_assimp_mesh_attribute_process(
        Element* input, szptr numElements, byte_t* basePtr,
        AssimpTransformFun<Element> fun)
{
    szptr offset = 0;
    for(szptr i=0;i<numElements;i++)
        offset+=fun(input[i],&basePtr[offset]);
}

}



}
}

#endif // ASSIMPFUN
