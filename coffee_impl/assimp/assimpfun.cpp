#include "assimpfun.h"

namespace Coffee{
namespace CResourceTypes{
namespace CAssimp{

szptr coffee_assimp_mesh_get_size(const aiMesh* meshdata, uint32* numBuffers)
{
    *numBuffers = 0;

    szptr bufferSize = sizeof(CAssimpMesh);

    if(meshdata->HasPositions())
    {
        bufferSize+=meshdata->mNumVertices*sizeof(CVec3);
        *numBuffers++;
    }

    if(meshdata->HasNormals())
    {
        bufferSize+=meshdata->mNumVertices*sizeof(CVec3);
        *numBuffers++;
    }

    if(meshdata->HasTangentsAndBitangents())
    {
        bufferSize+=meshdata->mNumVertices*sizeof(CVec3)*2;
        *numBuffers+=2;
    }

    for(unsigned int i=0;i<meshdata->GetNumUVChannels();i++)
    {
        if(meshdata->HasTextureCoords(i))
            bufferSize+=meshdata->mNumUVComponents[i]*sizeof(scalar)*meshdata->mNumVertices;
        *numBuffers++;
    }

    if(meshdata->HasFaces()){
        for(unsigned int i=0;i<meshdata->mNumFaces;i++)
            bufferSize+=meshdata->mFaces[i].mNumIndices*sizeof(unsigned int);
        *numBuffers++;
    }

    for(unsigned int i=0;i<meshdata->GetNumColorChannels();i++)
    {
        if(meshdata->HasVertexColors(i))
            bufferSize+=meshdata->mNumVertices*sizeof(CVec4);
        *numBuffers++;
    }

    return bufferSize;
}

CAssimpMesh *importMesh(aiMesh *meshdata){
    szptr vertices = meshdata->mNumVertices;
    szptr faces    = meshdata->mNumFaces;
    szptr i,j,k;
    szptr offset = 0;

    uint32 bufferCnt;
    szptr bufSize = coffee_assimp_mesh_get_size(meshdata,&bufferCnt);

    byte* buffer = (byte*)(calloc(1,bufSize));

    CAssimpMesh* mesh = (CAssimpMesh*)(&buffer[0]);

    mesh->chunk_size = bufSize;

    //We use bufferSize as a ptr in the buffer now
    offset = sizeof(CAssimpMesh);

    mesh->numBuffers = bufferCnt;

    uint8* typeBuffer = (uint8*)(&buffer[offset]);
    mesh->bufferType = typeBuffer;
    offset+=bufferCnt*sizeof(byte);

    uint8* elBuffer = (uint8*)(&buffer[offset]);
    mesh->elementSizes = elBuffer;
    offset+=bufferCnt*sizeof(byte);

    uint32* sizeBuffer = (uint32*)(&buffer[offset]);
    mesh->bufferSize = sizeBuffer;
    offset+=bufferCnt*sizeof(uint32);

    //bufferCnt is the index for buffers
    bufferCnt = 0;

//    if(meshdata->HasPositions()){
//        //Positions
//        elBuffer[bufferCnt] = sizeof(CVec3);
//        typeBuffer[bufferCnt] = CAssimpMesh::PositionType;
//        sizeBuffer[bufferCnt] = vertices;
//        CVec3* list = (CVec3*)(&buffer[offset]);

//        for(i=0;i<vertices;i++){
//            aiVector3D* vec = &meshdata->mVertices[i];
//            list[i].x = vec->x;
//            list[i].y = vec->y;
//            list[i].z = vec->z;
//        }

//        offset+=vertices*sizeof(CVec3);
//        bufferCnt++;
//    }
//    if(meshdata->HasNormals()){
//        //Normals
//        elBuffer[bufferCnt] = sizeof(CVec3);
//        typeBuffer[bufferCnt] = CAssimpMesh::NormalType;
//        sizeBuffer[bufferCnt] = vertices;
//        CVec3* list = (CVec3*)(&buffer[offset]);

//        for(i=0;i<vertices;i++){
//            aiVector3D* vec = &meshdata->mNormals[i];
//            list[i].x = vec->x;
//            list[i].y = vec->y;
//            list[i].z = vec->z;
//        }

//        bufferSize+=vertices*sizeof(CVec3);
//        bufferCnt++;
//    }
//    if(meshdata->HasTangentsAndBitangents()){
//        {
//            //Tangents
//            elBuffer[bufferCnt] = sizeof(CVec3);
//            typeBuffer[bufferCnt] = CAssimpMesh::BitanType;
//            sizeBuffer[bufferCnt] = vertices;
//            CVec3* list = (CVec3*)(&buffer[offset]);

//            for(i=0;i<vertices;i++){
//                aiVector3D* vec = &meshdata->mTangents[i];
//                list[i].x = vec->x;
//                list[i].y = vec->y;
//                list[i].z = vec->z;
//            }

//            bufferSize+=vertices*sizeof(CVec3);
//            bufferCnt++;
//        }

//        {
//            //Bitangents
//            elBuffer[bufferCnt] = sizeof(CVec3);
//            typeBuffer[bufferCnt] = CAssimpMesh::TangentType;
//            sizeBuffer[bufferCnt] = vertices;
//            CVec3* list = (CVec3*)(&buffer[offset]);

//            for(i=0;i<vertices;i++){
//                aiVector3D* vec = &meshdata->mBitangents[i];
//                list[i].x = vec->x;
//                list[i].y = vec->y;
//                list[i].z = vec->z;
//            }

//            offset+=vertices*sizeof(CVec3);
//            bufferCnt++;
//        }
//    }
//    if(meshdata->HasFaces()){
//        //Face indices
//        elBuffer[bufferCnt] = sizeof(uint32);
//        typeBuffer[bufferCnt] = CAssimpMesh::IndexType;
//        uint32* list = (uint32*)(&buffer[offset]);

//        k = 0;
//        for(i=0;i<faces;i++)
//            for(j=0;j<meshdata->mFaces[i].mNumIndices;j++){
//                list[k]=meshdata->mFaces[i].mIndices[j];
//                k++;
//            }

//        sizeBuffer[bufferCnt] = k;

//        offset+=k*sizeof(uint32);
//        bufferCnt++;
//    }
//    if(meshdata->HasTextureCoords(0)){
//        //Texture coordinates
//        elBuffer[bufferCnt] = sizeof(CVec2);
//        typeBuffer[bufferCnt] = CAssimpMesh::TextCoordType;
//        sizeBuffer[bufferCnt] = vertices;
//        CVec2* list = (CVec2*)(&buffer[offset]);

//        for(i=0;i<vertices;i++){
//            aiVector3D* vec = &meshdata->mTextureCoords[0][i];
//            list[i].x = vec->x;
//            list[i].y = 1.f-vec->y;
//        }

//        offset+=vertices*sizeof(CVec2);
//        bufferCnt++;
//    }
//    if(meshdata->HasVertexColors(0)){
//        //Vertex colors
//        elBuffer[bufferCnt] = sizeof(CVec3);
//        typeBuffer[bufferCnt] = CAssimpMesh::VColorType;
//        sizeBuffer[bufferCnt] = vertices;
//        CVec4* list = (CVec4*)(listBuffer[bufferCnt]);

//        for(i=0;i<vertices;i++){
//            aiColor4D* vec = &meshdata->mColors[0][i];
//            list[i].x = vec->r;
//            list[i].y = vec->g;
//            list[i].z = vec->b;
//            list[i].w = vec->a;
//        }

//        bufferSize+=vertices*sizeof(CVec4);
//        bufferCnt++;
//    }

    mesh->name = coffee_cpy_string(meshdata->mName.C_Str());

    return mesh;
}
}

bool coffee_assimp_dump_mesh(CAssimp::CAssimpMesh *mesh, CResources::CResource *resource)
{
    bool success = false;

    resource->free_data();

    resource->size = mesh->chunk_size;
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
