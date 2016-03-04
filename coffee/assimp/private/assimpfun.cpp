#include "assimpfun.h"

#include <coffee/core/CDebug>
#include <coffee/core/CThreading>

namespace Coffee{
namespace CResourceTypes{
namespace CAssimp{

szptr _assimp_face_transform(const aiFace &v, byte_t *d);
szptr _assimp_vec_transform(const aiVector3D &v, byte_t *d);
szptr _assimp_col_transform(const aiColor4D &v, byte_t *d);

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
    byte_t* buffer;

    {
        bufsize = coffee_assimp_mesh_approx_size(meshdata,&bufferCount);
        bufsize+=sizeof(CAssimpMesh);
        bufsize+=bufferCount*sizeof(assimp_reflexive)-sizeof(assimp_reflexive);
        bufsize+=strlen(meshdata->mName.C_Str())+1;

        buffer = (byte_t*)Calloc(sizeof(byte_t),bufsize);
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
    MemCpy(&buffer[offset],mname,mesh->name.size);
    offset+=mesh->name.size;

    coffee_assimp_mesh_get_offsets(meshdata,bufferArray,offset);

    szptr numUVs = 0,numCols = 0;

    assimp_reflexive* buf;
    for(szptr i=0;i<bufferCount;i++)
    {
        buf = &bufferArray[i];
        switch(buf->type)
        {
        case CAssimpMesh::PositionType:{
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mVertices,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
            break;
        }
        case CAssimpMesh::NormalType:{
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mNormals,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
            break;
        }
        case CAssimpMesh::TangentType:{
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mTangents,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
            break;
        }
        case CAssimpMesh::BitanType:{
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mBitangents,meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
            break;
        }
        case CAssimpMesh::IndexType:{
                coffee_assimp_mesh_attribute_process<aiFace>(
                            meshdata->mFaces,meshdata->mNumFaces,
                            &buffer[buf->offset],_assimp_face_transform);
            break;
        }
        case CAssimpMesh::VColorType:{
                coffee_assimp_mesh_attribute_process<aiColor4D>(
                            meshdata->mColors[numCols],meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_col_transform);
            numCols++;
            break;
        }
        case CAssimpMesh::TextCoordType:{
                coffee_assimp_mesh_attribute_process<aiVector3D>(
                            meshdata->mTextureCoords[numUVs],meshdata->mNumVertices,
                            &buffer[buf->offset],_assimp_vec_transform);
            numUVs++;
            break;
        }
        }
    }

    return mesh;
}

szptr _assimp_face_transform(const aiFace &v, byte_t *d)
{
    MemCpy(d,v.mIndices,sizeof(v.mIndices[0])*v.mNumIndices);
    return sizeof(v.mIndices[0])*v.mNumIndices;
}

szptr _assimp_vec_transform(const aiVector3D &v, byte_t *d)
{
    CVec3* o = (CVec3*)d;
    o->x() = v.x;
    o->y() = v.y;
    o->z() = v.z;
    return sizeof(CVec3);
}

szptr _assimp_col_transform(const aiColor4D &v, byte_t *d)
{
    CVec4* o = (CVec4*)d;
    o->r() = v.r;
    o->g() = v.g;
    o->b() = v.b;
    o->a() = v.a;
    return sizeof(CVec4);
}

}
}
}
