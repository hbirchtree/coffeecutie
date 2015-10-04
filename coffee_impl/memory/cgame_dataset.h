#ifndef CGAME_DATASET
#define CGAME_DATASET

#include "coffee_impl/graphics/cbuffer.h"
#include "coffee_impl/graphics/ctexture.h"
#include "coffee_impl/rendering/cmultidrawgraph.h"
#include "coffee_impl/assimp/cassimptypes.h"
#include "coffee_impl/graphics/cuniformtypes.h"
#include "coffee_impl/graphics/cgraphics_quirks.h"

namespace Coffee{
using namespace CGraphicsWrappers;
using namespace CRendering;

namespace CMemoryManagement{

struct game_vertex_description
{
    chunk_mem<CVertexFormat>        formats;
    chunk_mem<CVertexArrayObject>   arrays;
    chunk_mem<CVertexBufferBinding> bindings;
};

struct game_vertexdata_chunk
{
    chunk_mem<chunk_mem<byte>>  buffers;
    chunk_mem<CAssimpData*>     data;
    game_vertex_description     descriptor;
};

struct game_shader_manager
{
    chunk_mem<CShader>          shaders;
    chunk_mem<CShaderProgram>   programs;
    chunk_mem<CPipeline>        pipelines;
};

typedef chunk_mem<CTexture> game_texture_storage;

typedef chunk_mem<CResource> game_resource_chunk;

struct game_memory_chunk
{
    chunk_mem<CMultiDrawDataSet>    datasets;

    chunk_mem<CBuffer>              buffers;
    chunk_mem<CSubBuffer>           subbuffers;

    chunk_mem<CUniformBlock>        uniformblocks;
};

struct game_transform_chunk
{
    chunk_mem<CModelTransform>      transforms;
    chunk_mem<CGCamera>             cameras;
};

struct game_context
{
    game_vertexdata_chunk   vertexdata;
    game_shader_manager     shaders;
    game_resource_chunk     resources;
    game_memory_chunk       renderdata;
    game_texture_storage    texstorage;

    game_transform_chunk    transforms;

    CGraphicsQuirks::CFeatureSet *features;

    std::function<void(const CMultiDrawDataSet&)> renderfun;

    std::vector<CAssimpMesh*>   meshes;
    std::vector<szptr>          mesh_indices;
};

struct game_shader_program_desc
{
    cstring shader_v    = nullptr;
    cstring shader_f    = nullptr;

    cstring shader_dump = nullptr;
};

static void coffee_mem_clear(void* start, szptr size){
    memset(start,0,size);
}

template<typename T>
static void coffee_mem_expand_array(chunk_mem<T> *mem, szptr size){
    szptr osize = mem->size;
    mem->size += size;
    mem->d = (T*)realloc(mem->d,sizeof(T)*mem->size);
    coffee_mem_clear(&mem->d[osize],sizeof(T)*(size-osize));
}

}
}

#endif // CGAME_DATASET

