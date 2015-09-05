#ifndef CASSIMPTYPES
#define CASSIMPTYPES

#include <stdint.h>

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

//These types are allocated in single blocks of memory
//The thought being that they are free()'d in a single call
//Disadvantage: Difficult to move them once created

struct CAssimpMaterial
{
    const char*     name          = nullptr;
    uint16_t        numProperties = 0;
    uint32_t**      propertySizes = nullptr;
    const char*     propertyData  = nullptr;
    //User-defined types, differentiates bool, bytes, int and float
    uint8_t*        propertyTypes = nullptr;
};

struct CAssimpMesh
        //Vertex data
        //Should be allocated in a single block for storage purposes
        //Struct at offset=0
        //free() is called on the CAssimpMesh struct ptr
{
    const char*   name       = nullptr;

    uint8_t       numBuffers = 0;
    //Applies to the below lists
    const char**  buffers    = nullptr;
    const uint8_t*bufferType = nullptr;

    enum BufferType
    {
        PositionType    = 0x1,
        NormalType      = 0x2,
        TextCoordType   = 0x3,
        BitanType       = 0x4,
        TangentType     = 0x5,

        IndexType       = 0x6,

        VColorType      = 0x7,
    };
};

struct CAssimpCamera
{
    const char*   name       = nullptr;
};

struct CAssimpLight
{
    const char*   name       = nullptr;
};

struct CAssimpTexture
{
    const char*   name       = nullptr;
};

struct CAssimpAnimation
{
    const char*   name       = nullptr;
};

struct CAssimpModel //Material + Mesh
{
    const char*         name     = nullptr;

    CAssimpMesh*        mesh     = nullptr;
    CAssimpMaterial*    material = nullptr;
};

struct CAssimpData
{
    uint32_t            numMeshes       = 0;
    uint32_t            numMaterials    = 0;
    uint32_t            numCameras      = 0;
    uint32_t            numLights       = 0;
    uint32_t            numTextures     = 0;
    uint32_t            numAnimations   = 0;

    CAssimpMesh*        meshes      = nullptr;
    CAssimpMaterial*    materials   = nullptr;
    CAssimpCamera*      cameras     = nullptr;
    CAssimpLight*       lights      = nullptr;
    CAssimpTexture*     textures    = nullptr;
    CAssimpAnimation*   animations  = nullptr;
};

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // CASSIMPTYPES

