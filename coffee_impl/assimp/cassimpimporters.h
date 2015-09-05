#ifndef COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
#define COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H

#include "coffee.h"
#include "coffee/cdebug.h"
#include "coffee/cfiles.h"

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

using namespace Coffee::CResources;

struct CAssimpMaterial
{
    uint16_t        numProperties = 0;
    uint32_t**      propertySizes = nullptr;
    const char*     propertyData  = nullptr;
    //User-defined types, differentiates bool, bytes, int and float
    uint8_t*        propertyTypes = nullptr;
};

struct CAssimpMesh
        //Vertex data
        //Should be allocated in a single block for storage purposes
        //Mesh data at offset=0
{
    uint8_t       numBuffers = 0;
    const float** buffers    = nullptr;
    uint8_t*      bufferType = nullptr;
};

struct CAssimpCamera
{

};

struct CAssimpLight
{

};

struct CAssimpTexture
{

};

struct CAssimpAnimation
{

};

struct CAssimpModel //Material + Mesh
{
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

class CAssimpImporters
{
private:
    CAssimpImporters();
public:
    static CAssimpData* importResource(CResource* source, CString hint = CString());
};

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
