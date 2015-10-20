#include "assimpfun.h"

namespace Coffee{
namespace CResourceTypes{
namespace CAssimp{

szptr coffee_assimp_mesh_approx_size(const aiMesh* mesh, szptr* numBuffers)
{
    szptr size = 0;
    *numBuffers = 0;
    size+=mesh->mNumVertices
            *mesh->HasPositions()*sizeof(CVec3);
    *numBuffers+= mesh->HasPositions();

    size+=mesh->mNumVertices
            *mesh->HasNormals()*sizeof(CVec3);
    *numBuffers+= mesh->HasNormals();

    size+=mesh->mNumVertices
            *mesh->HasTangentsAndBitangents()*sizeof(CVec3)*2;
    *numBuffers+= mesh->HasTangentsAndBitangents()*2;

    size+=mesh->mNumVertices*mesh->GetNumColorChannels()*sizeof(CVec4);
    *numBuffers+= mesh->GetNumColorChannels();

    unsigned int i,j;

    for(i=0;i<mesh->GetNumUVChannels();i++)
        size+=mesh->mNumVertices*mesh->mNumUVComponents[i]*sizeof(scalar);
    *numBuffers+=mesh->GetNumUVChannels();

    return size;
}

CAssimpMesh *importMesh(aiMesh *meshdata){
    szptr bufferCount;
    szptr offset = 0;
    byte* buffer;

    {
        szptr bufsize = coffee_assimp_mesh_approx_size(meshdata,&bufferCount);
        bufsize+=sizeof(CAssimpMesh);
        bufsize+=bufferCount*sizeof(assimp_reflexive);

        buffer = (byte*)calloc(
                    sizeof(byte),
                    bufsize);
    }


    CAssimpMesh* mesh = (CAssimpMesh*)(&buffer[0]);
    offset += sizeof(CAssimpMesh);
    mesh->numBuffers = bufferCount;
    mesh->name = coffee_cpy_string(meshdata->mName.C_Str());

    assimp_reflexive* bufferArray = (assimp_reflexive*)&buffer[offset];


    return mesh;
}
}

bool coffee_assimp_dump_mesh(CAssimp::CAssimpMesh *mesh, CResources::CResource *resource)
{
    bool success = false;

    resource->free_data();

    resource->size = mesh->byteSize;
    resource->data = malloc(resource->size);

    memcpy(resource->data,mesh,resource->size);

    if(!resource->save_data())
        cWarning("Failed to store mesh data");
    else success = true;

    resource->free_data();

    return success;
}

void coffee_assimp_free(CAssimp::CAssimpData *data)
{
    szptr i;
    for(i=0;i<data->numMeshes;i++)
        free(data->meshes[i]);
    free(data->meshes);
    for(i=0;i<data->numMaterials;i++)
        free(data->materials[i]);
    free(data->materials);
    for(i=0;i<data->numCameras;i++)
        free(data->cameras[i]);
    free(data->cameras);
    for(i=0;i<data->numLights;i++)
        free(data->lights[i]);
    free(data->lights);
    for(i=0;i<data->numTextures;i++)
        free(data->textures[i]);
    free(data->textures);
    for(i=0;i<data->numAnimations;i++)
        free(data->animations[i]);
    free(data->animations);
}

}
}
