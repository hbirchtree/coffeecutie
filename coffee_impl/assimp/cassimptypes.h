#ifndef CASSIMPTYPES
#define CASSIMPTYPES

#include "coffee.h"

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

struct CAssimpMesh
        //Vertex data
        //Should be allocated in a single block for storage purposes
        //Struct at offset=0
        //free() is called on the CAssimpMesh struct ptr
{
    cstring             name            = nullptr;

    uint8               numBuffers      = 0;
    //Applies to the below lists
    cstring const*      buffers         = nullptr;
    const uint8*        bufferType      = nullptr;
    const uint8*        elementSizes    = nullptr;
    const uint32*       bufferSize      = nullptr;

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

struct CAssimpModel //Material + Mesh
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

    void freeData(){
        szptr i;
        for(i=0;i<numMeshes;i++)
            free(meshes[i]);
        free(meshes);
        for(i=0;i<numMaterials;i++)
            free(materials[i]);
        free(materials);
        for(i=0;i<numCameras;i++)
            free(cameras[i]);
        free(cameras);
        for(i=0;i<numLights;i++)
            free(lights[i]);
        free(lights);
        for(i=0;i<numTextures;i++)
            free(textures[i]);
        free(textures);
        for(i=0;i<numAnimations;i++)
            free(animations[i]);
        free(animations);
    }
};

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // CASSIMPTYPES

