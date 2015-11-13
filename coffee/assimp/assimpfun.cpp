#include "assimpfun.h"

//#define ASSIMP_PARSE_MULTICORE

#include <coffee/core/base/cdebug.h>
#include <coffee/core/base/cthreading.h>

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

    unsigned int i;

    for(i=0;i<mesh->GetNumUVChannels();i++)
        size+=mesh->mNumVertices*mesh->mNumUVComponents[i]*sizeof(scalar);
    *numBuffers+=mesh->GetNumUVChannels();

    for(i=0;i<mesh->mNumFaces;i++)
        size+=mesh->mFaces[i].mNumIndices*sizeof(mesh->mFaces[i].mIndices[0]);
    *numBuffers+=mesh->HasFaces();

    return size;
}

void coffee_assimp_mesh_get_offsets(const aiMesh* mesh, assimp_reflexive* buffers, szptr initOffset)
{
    szptr size = initOffset;
    szptr buf = 0;

    if(mesh->HasPositions())
    {
        buffers[buf].offset = size;
        size+=mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].size = mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].type = CAssimpMesh::PositionType;
        buf++;
    }

    if(mesh->HasNormals())
    {
        buffers[buf].offset = size;
        size+=mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].size = mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].type = CAssimpMesh::NormalType;
        buf++;
    }

    if(mesh->HasTangentsAndBitangents())
    {
        buffers[buf].offset = size;
        size+=mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].size = mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].type = CAssimpMesh::TangentType;
        buf++;

        buffers[buf].offset = size;
        size+=mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].size = mesh->mNumVertices*sizeof(CVec3);
        buffers[buf].type = CAssimpMesh::BitanType;
        buf++;
    }

    unsigned int i;

    for(i=0;i<mesh->GetNumColorChannels();i++)
    {
        buffers[buf].offset = size;
        size+=mesh->mNumVertices*sizeof(CVec4);
        buffers[buf].size = mesh->mNumVertices*sizeof(CVec4);
        buffers[buf].type = CAssimpMesh::VColorType;
        buf++;
    }

    for(i=0;i<mesh->GetNumUVChannels();i++)
    {
        buffers[buf].offset = size;
        size+=mesh->mNumVertices*sizeof(scalar)*mesh->mNumUVComponents[i];
        buffers[buf].size = mesh->mNumVertices*sizeof(scalar)*mesh->mNumUVComponents[i];
        buffers[buf].type = CAssimpMesh::TextCoordType;
        buf++;
    }

    if(mesh->HasFaces()){
        buffers[buf].offset = size;
        for(i=0;i<mesh->mNumFaces;i++)
        {
            buffers[buf].size += mesh->mFaces[i].mNumIndices
                    *sizeof(mesh->mFaces[i].mIndices[0]);
        }
        buffers[buf].type = CAssimpMesh::IndexType;
        buf++;
    }
}

CAssimpMesh *importMesh(const aiMesh *meshdata){
    szptr bufferCount;
    szptr offset = 0;
    szptr bufsize;
    byte* buffer;

    {
        bufsize = coffee_assimp_mesh_approx_size(meshdata,&bufferCount);
        bufsize+=sizeof(CAssimpMesh);
        bufsize+=bufferCount*sizeof(assimp_reflexive)-sizeof(assimp_reflexive);
        bufsize+=strlen(meshdata->mName.C_Str())+1;

        buffer = (byte*)calloc(
                    sizeof(byte),
                    bufsize);
    }

    CAssimpMesh* mesh = (CAssimpMesh*)(&buffer[0]);
    offset += sizeof(CAssimpMesh);
    mesh->numBuffers = bufferCount;
    mesh->byteSize = bufsize;

    assimp_reflexive* bufferArray = (assimp_reflexive*)&mesh->buffers;
    offset+=sizeof(assimp_reflexive)*bufferCount-sizeof(assimp_reflexive);

    cstring mname = meshdata->mName.C_Str();
    mesh->name.offset = offset;
    mesh->name.size = strlen(mname)+1;
    memcpy(&buffer[offset],mname,mesh->name.size);
    offset+=mesh->name.size;

    coffee_assimp_mesh_get_offsets(meshdata,bufferArray,offset);

    std::vector<std::future<void>> futures;

    szptr numUVs = 0,numCols = 0;

    assimp_reflexive* buf;
    for(csize_t i=0;i<bufferCount;i++)
    {
        buf = &bufferArray[i];
        switch(buf->type)
        {
        case CAssimpMesh::PositionType:{
#ifdef ASSIMP_PARSE_MULTICORE
            futures.push_back(CThreading::runAsync<void>(
                                  [=](){
#endif
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mVertices,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
#ifdef ASSIMP_PARSE_MULTICORE
            }));
#endif
            break;
        }
        case CAssimpMesh::NormalType:{
#ifdef ASSIMP_PARSE_MULTICORE
            futures.push_back(CThreading::runAsync<void>(
                                  [=](){
#endif
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mNormals,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
#ifdef ASSIMP_PARSE_MULTICORE
            }));
#endif
            break;
        }
        case CAssimpMesh::TangentType:{
#ifdef ASSIMP_PARSE_MULTICORE
            futures.push_back(CThreading::runAsync<void>(
                                  [=](){
#endif
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mTangents,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
#ifdef ASSIMP_PARSE_MULTICORE
            }));
#endif
            break;
        }
        case CAssimpMesh::BitanType:{
#ifdef ASSIMP_PARSE_MULTICORE
            futures.push_back(CThreading::runAsync<void>(
                                  [=](){
#endif
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mBitangents,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
#ifdef ASSIMP_PARSE_MULTICORE
            }));
#endif
            break;
        }
        case CAssimpMesh::IndexType:{
#ifdef ASSIMP_PARSE_MULTICORE
            futures.push_back(CThreading::runAsync<void>(
                                  [=](){
#endif
                coffee_assimp_mesh_attribute_process<aiFace>(
                            meshdata->mFaces,meshdata->mNumFaces,
                            &buffer[buf->offset],_assimp_face_transform);
#ifdef ASSIMP_PARSE_MULTICORE
            }));
#endif
            break;
        }
        case CAssimpMesh::VColorType:{
#ifdef ASSIMP_PARSE_MULTICORE
            futures.push_back(CThreading::runAsync<void>(
                                  [=](){
#endif
                coffee_assimp_mesh_attribute_process<aiColor4D>(
                            meshdata->mColors[numCols],meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_col_transform);
#ifdef ASSIMP_PARSE_MULTICORE
            }));
#endif
            numCols++;
            break;
        }
        case CAssimpMesh::TextCoordType:{
#ifdef ASSIMP_PARSE_MULTICORE
            futures.push_back(CThreading::runAsync<void>(
                                  [=](){
#endif
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mTextureCoords[numUVs],meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
#ifdef ASSIMP_PARSE_MULTICORE
            }));
#endif
            numUVs++;
            break;
        }
        }
    }

#ifdef ASSIMP_PARSE_MULTICORE
    for(std::future<void>& f : futures)
        f.get();
#endif

    return mesh;
}

szptr _assimp_face_transform(const aiFace &v, byte *d)
{
    memcpy(d,v.mIndices,sizeof(v.mIndices[0])*v.mNumIndices);
    return sizeof(v.mIndices[0])*v.mNumIndices;
}

szptr _assimp_vec_transform(const aiVector3D &v, byte *d)
{
    CVec3* o = (CVec3*)d;
    o->x() = v.x;
    o->y() = v.y;
    o->z() = v.z;
    return sizeof(CVec3);
}

szptr _assimp_col_transform(const aiColor4D &v, byte *d)
{
    CVec4* o = (CVec4*)d;
    o->r() = v.r;
    o->g() = v.g;
    o->b() = v.b;
    o->a() = v.a;
    return sizeof(CVec4);
}

byte *coffee_assimp_get_reflexive_ptr(void *baseptr, const assimp_reflexive *ref)
{
    return &((byte*)baseptr)[ref->offset];
}

cstring assimp_reflexive_string_get(const void* basePtr, const assimp_reflexive &ref)
{
    const byte* b_ptr = (const byte*)basePtr;
    return &b_ptr[ref.offset];
}

}

bool coffee_assimp_dump_mesh(CAssimp::CAssimpMesh *mesh, CResources::CResource *resource)
{
    bool success = false;

    coffee_file_free(resource);

    resource->size = mesh->byteSize;
    resource->data = malloc(resource->size);

    memcpy(resource->data,mesh,resource->size);

    if(!coffee_file_commit(resource))
        cWarning("Failed to store mesh data");
    else success = true;

    coffee_file_free(resource);

    return success;
}

void coffee_assimp_free(CAssimp::CAssimpData *data)
{
    szptr i;
    for(i=0;i<data->numMeshes;i++)
        free(data->meshes[i]);
    free(data->meshes);
}

}
}
