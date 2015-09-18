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

static  CAssimpMesh*        importMesh(aiMesh* meshdata){
    szptr vertices = meshdata->mNumVertices;
    szptr faces    = meshdata->mNumFaces;
    szptr i,j,k;

    szptr bufferSize = sizeof(CAssimpMesh);
    uint bufferCnt    = 0;

    if(meshdata->HasPositions()){
        bufferCnt++;
        bufferSize+=vertices*sizeof(CVec3);
    }
    if(meshdata->HasNormals()){
        bufferCnt++;
        bufferSize+=vertices*sizeof(CVec3);
    }
    if(meshdata->HasTangentsAndBitangents()){
        bufferCnt+=2;
        bufferSize+=vertices*sizeof(CVec3)*2;
    }
    if(meshdata->HasFaces()){
        bufferCnt++;
        j=0;
        for(i=0;i<faces;i++)
            j+=meshdata->mFaces[i].mNumIndices;
        bufferSize+=j*sizeof(uint32_t);
    }
    if(meshdata->HasTextureCoords(0)){
        bufferCnt++;
        bufferSize+=vertices*sizeof(CVec2);
    }
    if(meshdata->HasVertexColors(0)){
        bufferCnt++;
        bufferSize+=vertices*sizeof(CVec4);
    }

    bufferSize+=bufferCnt*sizeof(uint8_t);
    bufferSize+=bufferCnt*sizeof(uint8_t);
    bufferSize+=bufferCnt*sizeof(uint32_t);
    bufferSize+=bufferCnt*sizeof(char*);

    char* buffer = reinterpret_cast<char*>(calloc(1,bufferSize));

    CAssimpMesh* mesh = reinterpret_cast<CAssimpMesh*>(&buffer[0]);

    //We use bufferSize as a ptr in the buffer now
    bufferSize = sizeof(CAssimpMesh);

    mesh->numBuffers = bufferCnt;

    uint8_t* typeBuffer = reinterpret_cast<uint8_t*>(&buffer[bufferSize]);
    mesh->bufferType = typeBuffer;
    bufferSize+=bufferCnt*sizeof(uint8_t);

    uint8_t* elBuffer = reinterpret_cast<uint8_t*>(&buffer[bufferSize]);
    mesh->elementSizes = elBuffer;
    bufferSize+=bufferCnt*sizeof(uint8_t);

    uint32_t* sizeBuffer = reinterpret_cast<uint32_t*>(&buffer[bufferSize]);
    mesh->bufferSize = sizeBuffer;
    bufferSize+=bufferCnt*sizeof(uint32_t);

    char** listBuffer = reinterpret_cast<char**>(&buffer[bufferSize]);
    mesh->buffers = listBuffer;
    bufferSize+=bufferCnt*sizeof(char*);

    //bufferCnt is the index for buffers
    bufferCnt = 0;


    if(meshdata->HasPositions()){
        //Positions
        elBuffer[bufferCnt] = sizeof(CVec3);
        typeBuffer[bufferCnt] = CAssimpMesh::PositionType;
        sizeBuffer[bufferCnt] = vertices;
        listBuffer[bufferCnt] = &buffer[bufferSize];
        CVec3* list = reinterpret_cast<CVec3*>(listBuffer[bufferCnt]);

        for(i=0;i<vertices;i++){
            aiVector3D* vec = &meshdata->mVertices[i];
            list[i].x = vec->x;
            list[i].y = vec->y;
            list[i].z = vec->z;
        }

        bufferSize+=vertices*sizeof(CVec3);
        bufferCnt++;
    }
    if(meshdata->HasNormals()){
        //Normals
        elBuffer[bufferCnt] = sizeof(CVec3);
        typeBuffer[bufferCnt] = CAssimpMesh::NormalType;
        sizeBuffer[bufferCnt] = vertices;
        listBuffer[bufferCnt] = &buffer[bufferSize];
        CVec3* list = reinterpret_cast<CVec3*>(listBuffer[bufferCnt]);

        for(i=0;i<vertices;i++){
            aiVector3D* vec = &meshdata->mNormals[i];
            list[i].x = vec->x;
            list[i].y = vec->y;
            list[i].z = vec->z;
        }

        bufferSize+=vertices*sizeof(CVec3);
        bufferCnt++;
    }
    if(meshdata->HasTangentsAndBitangents()){
        {
            //Tangents
            elBuffer[bufferCnt] = sizeof(CVec3);
            typeBuffer[bufferCnt] = CAssimpMesh::BitanType;
            sizeBuffer[bufferCnt] = vertices;
            listBuffer[bufferCnt] = &buffer[bufferSize];
            CVec3* list = reinterpret_cast<CVec3*>(listBuffer[bufferCnt]);

            for(i=0;i<vertices;i++){
                aiVector3D* vec = &meshdata->mTangents[i];
                list[i].x = vec->x;
                list[i].y = vec->y;
                list[i].z = vec->z;
            }

            bufferSize+=vertices*sizeof(CVec3);
            bufferCnt++;
        }

        {
            //Bitangents
            elBuffer[bufferCnt] = sizeof(CVec3);
            typeBuffer[bufferCnt] = CAssimpMesh::TangentType;
            sizeBuffer[bufferCnt] = vertices;
            listBuffer[bufferCnt] = &buffer[bufferSize];
            CVec3* list = reinterpret_cast<CVec3*>(listBuffer[bufferCnt]);

            for(i=0;i<vertices;i++){
                aiVector3D* vec = &meshdata->mBitangents[i];
                list[i].x = vec->x;
                list[i].y = vec->y;
                list[i].z = vec->z;
            }

            bufferSize+=vertices*sizeof(CVec3);
            bufferCnt++;
        }
    }
    if(meshdata->HasFaces()){
        //Face indices
        elBuffer[bufferCnt] = sizeof(uint32_t);
        typeBuffer[bufferCnt] = CAssimpMesh::IndexType;
        listBuffer[bufferCnt] = &buffer[bufferSize];
        uint32_t* list = reinterpret_cast<uint32_t*>(listBuffer[bufferCnt]);

        k = 0;
        for(i=0;i<faces;i++)
            for(j=0;j<meshdata->mFaces[i].mNumIndices;j++){
                list[k]=meshdata->mFaces[i].mIndices[j];
                k++;
            }

        sizeBuffer[bufferCnt] = k;

        bufferSize+=k*sizeof(uint32_t);
        bufferCnt++;
    }
    if(meshdata->HasTextureCoords(0)){
        //Texture coordinates
        elBuffer[bufferCnt] = sizeof(CVec2);
        typeBuffer[bufferCnt] = CAssimpMesh::TextCoordType;
        sizeBuffer[bufferCnt] = vertices;
        listBuffer[bufferCnt] = &buffer[bufferSize];
        CVec2* list = reinterpret_cast<CVec2*>(listBuffer[bufferCnt]);

        for(i=0;i<vertices;i++){
            aiVector3D* vec = &meshdata->mTextureCoords[0][i];
            list[i].x = vec->x;
            list[i].y = 1.f-vec->y;
        }

        bufferSize+=vertices*sizeof(CVec2);
        bufferCnt++;
    }
    if(meshdata->HasVertexColors(0)){
        //Vertex colors
        elBuffer[bufferCnt] = sizeof(CVec3);
        typeBuffer[bufferCnt] = CAssimpMesh::VColorType;
        sizeBuffer[bufferCnt] = vertices;
        listBuffer[bufferCnt] = &buffer[bufferSize];
        CVec4* list = reinterpret_cast<CVec4*>(listBuffer[bufferCnt]);

        for(i=0;i<vertices;i++){
            aiColor4D* vec = &meshdata->mColors[0][i];
            list[i].x = vec->r;
            list[i].y = vec->g;
            list[i].z = vec->b;
            list[i].w = vec->a;
        }

        bufferSize+=vertices*sizeof(CVec4);
        bufferCnt++;
    }

    mesh->name = coffee_cpy_string(meshdata->mName.C_Str());

    return mesh;
}
static  CAssimpMaterial*     importMaterial(aiMaterial* matdata);
static  CAssimpLight*        importLight(aiLight* ldata);
static  CAssimpCamera*       importCamera(aiCamera* camdata);
static  CAssimpTexture*      importTexture(aiTexture* texdata);
static  CAssimpAnimation*    importAnimation(aiAnimation* anidata);

}
}
}

#endif // ASSIMPFUN
