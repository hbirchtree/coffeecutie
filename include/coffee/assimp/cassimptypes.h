#ifndef CASSIMPTYPES
#define CASSIMPTYPES

#include <coffee/core/coffee.h>
#include <coffee/core/base/files/cfiles.h>

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

//These types are allocated in single blocks of memory
//The thought being that they are free()'d in a single call
//Disadvantage: Difficult to move them once created

struct assimp_reflexive
{
    szptr offset;
    szptr size;
    byte_t  type;
};

/*!
 * \brief Assimp mesh containing vertex data, from positions to bitangents and texture coordinates. Should be allocated in a single, contiguous block to allow simple free'ing of resources.
 */
struct CAssimpMesh
{
    szptr   byteSize = 0; /*!< Size of mesh in memory, used when dumping to file*/
    assimp_reflexive name; /*!< Name of mesh from Assimp*/
    uint8   numBuffers = 0; /*!< Number of buffers contained*/

    assimp_reflexive buffers[1]; /*!< Buffer offsets, sizes and type */

    enum BufferType
    {
        PositionType    = 0x1,
        NormalType      = 0x2,
        TextCoordType   = 0x3,
        TangentType     = 0x4,
        BitanType       = 0x5,

        IndexType       = 0x6,

        VColorType      = 0x7,
    };
};

struct CAssimpData
{
    uint32            numMeshes       = 0;
    uint32            numMaterials    = 0;
    uint32            numCameras      = 0;
    uint32            numLights       = 0;
    uint32            numTextures     = 0;
    uint32            numAnimations   = 0;

    CAssimpMesh**       meshes      = nullptr;
};

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // CASSIMPTYPES

