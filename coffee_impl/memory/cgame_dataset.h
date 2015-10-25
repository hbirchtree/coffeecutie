#ifndef CGAME_DATASET
#define CGAME_DATASET

#include "cgame_function_binds.h"
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

    CGraphicsQuirks::CFeatureSet*   features;
    game_function_binds             funptrs;

    std::vector<CAssimpMesh*>   meshes;
    std::vector<szptr>          mesh_indices;
};

struct game_shader_program_desc
{
    cstring shader_v    = nullptr;
    cstring shader_g    = nullptr;
    cstring shader_f    = nullptr;

    cstring shader_tes  = nullptr;
    cstring shader_tcs  = nullptr;

    cstring shader_dump = nullptr;
};

}
}

#endif // CGAME_DATASET

