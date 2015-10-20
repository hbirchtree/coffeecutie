#ifndef CASSIMPTYPES
#define CASSIMPTYPES

#include "coffee.h"
#include "coffee/cfiles.h"

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

//These types are allocated in single blocks of memory
//The thought being that they are free()'d in a single call
//Disadvantage: Difficult to move them once created

struct CAssimpMaterial
{
    cstring                 name            = nullptr;
    uint16                  numProperties   = 0;
    const uint32* const*    propertySizes   = nullptr;
    const byte*             propertyData    = nullptr;
    //User-defined types, differentiates bool, bytes, int and float
    const uint8*            propertyTypes   = nullptr;
};

struct assimp_reflexive
{
    byte  type;
    szptr offset;
    szptr size;
};

/*!
 * \brief Assimp mesh containing vertex data, from positions to bitangents and texture coordinates. Should be allocated in a single, contiguous block to allow simple free'ing of resources.
 */
struct CAssimpMesh
{
    szptr   byteSize = 0; /*!< Size of mesh in memory, used when dumping to file*/
    cstring name     = nullptr; /*!< Name of mesh from Assimp*/
    uint8   numBuffers = 0; /*!< Number of buffers contained*/

    const assimp_reflexive* buffers = nullptr; /*!< Buffer offsets, sizes and type */

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



struct CAssimpCamera
{
    cstring       name       = nullptr;
};

struct CAssimpLight
{
    cstring       name       = nullptr;
};

struct CAssimpTexture
{
    cstring       name       = nullptr;
};

struct CAssimpAnimation
{
    cstring       name       = nullptr;
};

/*!
 * \brief A set of Assimp mesh and material
 */
struct CAssimpModel
{
    cstring             name     = nullptr;

    CAssimpMesh*        mesh     = nullptr;
    CAssimpMaterial*    material = nullptr;
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
    CAssimpMaterial**   materials   = nullptr;
    CAssimpCamera**     cameras     = nullptr;
    CAssimpLight**      lights      = nullptr;
    CAssimpTexture**    textures    = nullptr;
    CAssimpAnimation**  animations  = nullptr;
};

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // CASSIMPTYPES

