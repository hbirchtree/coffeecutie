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
    size_t vertices = meshdata->mNumVertices;
    size_t faces    = meshdata->mNumFaces;
    size_t i,j,k;

    size_t bufferSize = sizeof(CAssimpMesh);
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
    bufferSize+=bufferCnt*sizeof(char*);

    char* buffer = reinterpret_cast<char*>(malloc(bufferSize));

    CAssimpMesh* mesh = reinterpret_cast<CAssimpMesh*>(&buffer[0]);

    //We use bufferSize as a ptr in the buffer now
    bufferSize = sizeof(CAssimpMesh);

    uint8_t* typeBuffer = reinterpret_cast<uint8_t*>(&buffer[bufferSize]);
    bufferSize+=bufferCnt*sizeof(uint8_t);
    char** listBuffer = reinterpret_cast<char**>(&buffer[bufferSize]);
    bufferSize+=bufferCnt*sizeof(char*);

    //bufferCnt is the index for buffers
    bufferCnt = 0;


    if(meshdata->HasPositions()){
        //Positions
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
        listBuffer[bufferCnt] = &buffer[bufferSize];
        uint32_t* list = reinterpret_cast<uint32_t*>(listBuffer[bufferCnt]);

        k = 0;
        for(i=0;i<faces;i++)
            for(j=0;j<meshdata->mFaces[i].mNumIndices;j++){
                list[k]=meshdata->mFaces[i].mIndices[j];
                k++;
            }

        bufferSize+=k*sizeof(uint32_t);
        bufferCnt++;
    }
    if(meshdata->HasTextureCoords(0)){
        //Texture coordinates
        listBuffer[bufferCnt] = &buffer[bufferSize];
        CVec2* list = reinterpret_cast<CVec2*>(listBuffer[bufferCnt]);

        for(i=0;i<vertices;i++){
            aiVector3D* vec = &meshdata->mTextureCoords[0][i];
            list[i].x = vec->x;
            list[i].y = 1.f-vec->y;
        }

        bufferSize+=vertices*sizeof(CVec3);
        bufferCnt++;
    }
    if(meshdata->HasVertexColors(0)){
        //Normals
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
