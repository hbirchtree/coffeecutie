#pragma once

#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/graphics/apis/gleam/levels/shared/gl_shared_types.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

using namespace CGL;

enum APILevel
{
    GL_Nothing = 0x0,

    /* Desktop GL versions */
    GL_3_3   = 0x330,
    GL_4_3   = 0x430,
    GL_4_5   = 0x450,

    /* Mobile GL versions / ES */
    /* These do not map directly to desktop versions */
    GLES_2_0 = 0x10200,
    GLES_3_0 = 0x10300,
    GLES_3_2 = 0x10320,
};

enum class APIClass
{
    Undefined,
    GLCore,
    GLES,
};

inline bool APILevelIsOfClass(APILevel l, APIClass c)
{
    switch(l)
    {
    case GL_3_3:
    case GL_4_3:
    case GL_4_5:
        return c == APIClass::GLCore;
    case GLES_2_0:
    case GLES_3_0:
    case GLES_3_2:
        return c == APIClass::GLES;
    case GL_Nothing:
        return c == APIClass::Undefined;
    }
}

struct GLEAM_API;

/* GLEAM-specific data */

struct GLEAM_Instance_Data;
struct GLEAM_DataStore;

/* Profiler structures */
struct GLEAM_PRF;
struct GLEAM_DBufQuery;
struct GLEAM_PrfQuery;
struct GLEAM_PipelineDumper;

/* Buffers */
struct GLEAM_VBuffer;
struct GLEAM_ArrayBuffer;
struct GLEAM_ElementBuffer;

struct GLEAM_UniformBuffer;
struct GLEAM_ShaderBuffer;

struct GLEAM_PixelBuffer;

/* Surfaces */
struct GLEAM_Surface;

struct GLEAM_Surface2D;
struct GLEAM_Surface3D;
struct GLEAM_SurfaceCube;

struct GLEAM_Surface2DArray;
struct GLEAM_SurfaceCubeArray;

/* Samplers */
struct GLEAM_SamplerHandle;

struct GLEAM_Sampler2D;
struct GLEAM_Sampler3D;
struct GLEAM_SamplerCube;

struct GLEAM_Sampler2DArray;
struct GLEAM_SamplerCubeArray;

/* Shaders */
struct GLEAM_Shader;
struct GLEAM_Pipeline;

/* Vertex data */
struct GLEAM_VertAttribute;
struct GLEAM_VertDescriptor;

/* Uniform data */
struct GLEAM_UniformDescriptor;
struct GLEAM_ShaderUniformState;

struct GLEAM_ProgramParameter;

struct GLEAM_RenderTarget;
struct GLEAM_RenderDummy;

struct GLEAM_ShaderImage;

/* Queries */
struct GLEAM_OccludeQuery;

}
}
}
