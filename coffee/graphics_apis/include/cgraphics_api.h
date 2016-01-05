#ifndef COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H
#define COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H

#include <coffee/core/CTypes>

namespace Coffee{
namespace CRHI{

struct CGraphicsProfiler
{
    /*!
     * \brief Queries the API for performance information, begins and ends
     */
    struct PerfQuery
    {
        PerfQuery(ProfilingTerm term);

        void begin();
        void end();

        int64 getResult();
    };
};

struct CGraphicsAPI
{
    /*!
     * \brief A singular command for the GPU command queue
     */
    struct GPUCommand : _cbasic_threadrunner_command
    {
    };
    /*!
     * \brief A queue which executes commands on the GPU thread
     */
    struct GPUCommandQueue : _cbasic_threadrunner_queue
    {
        void insertCmd(GPUCommand*){}
    };
    /*!
     * \brief Contains state such as whether it should output to screen, culling, etc.
     */
    struct RasterizerState
    {
        bool discard(){}
        bool culling(){}
        bool wireframeRender(){}
        bool polygonSmooth(){}
        bool dither(){}
        bool clampDepth(){}
        bool testScissor(){}
    };

    struct TessellatorState
    {
        int32 patchCount(){}
    };

    /*!
     * \brief Set viewport arrays. We will use this for VR rendering later. (Geometry shader way)
     */
    struct ViewportState
    {
        bool multiview(){}
        int32 viewCount(){}
        CRect view(int32){}
        CZField64 depth(int32){}
        CRect scissor(int32){}
    };

    struct BlendState
    {
        bool blend(){}
        bool additive(){}
        bool sampleAlphaCoverage(){}
    };

    struct DepthStencilState
    {
        bool testDepth(){}
        bool testStencil(){}
    };

    struct PixelProcessState
    {
        bool swapEndianness(){}
        bool lsbFirst(){}
        int32 rowLength(){}
        int32 imgHeight(){}
        int32 alignment(){}
    };

    struct DebugMessage
    {
        DebugComponent component(){}
        DebugType type(){}
        Severity severity(){}
        CString message(){}
    };

    struct VertexDescription
    {
        void addAttribute(uint32 idx,uint32 offset,uint32 stride,uint32 flags,TypeEnum type,bool normalized){}
        bool interleaved(){}
        int32 stride(){}
    };

    /*!
     * \brief Base of all buffers, can contain vertices and etc.
     */
    struct VertexBuffer
    {
        VertexBuffer(ResourceAccess access, uint32 size){}

        void commitMemory();
    };
    /*!
     * \brief Contains mesh indices for ordering of vertices
     */
    struct IndexBuffer : public VertexBuffer
    {
        IndexBuffer(TypeEnum itype, uint32 size){}
    };
    /*!
     * \brief Contains data which will be read by a shader
     */
    struct UniformBuffer : public VertexBuffer
    {
        UniformBuffer(uint32 stride, uint32 size){}
    };
    /*!
     * \brief Contains data which will be modified by a shader, SSBO, transformed data, compute data
     */
    struct ShaderBuffer : public VertexBuffer
    {
        ShaderBuffer(uint32 stride, uint32 size){}
    };
    /*!
     * \brief Contains a packed struct of parameters. Not applicable to GL3.3
     */
    struct IndirectBuffer : public VertexBuffer
    {
        IndirectBuffer(uint32 flags, uint32 stride, uint32 size){}
    };
    /*!
     * \brief Describes a particular uniform value, either inside a uniform block or separately
     */
    struct UniformDescriptor
    {
        uint32 index(){}
        cstring name(){}
        uint32 flags(){}
    };

    /*!
     * \brief Use compute shaders when applicable, transform feedback + geometry shader otherwise (GL3.3). Data-specification is depending on implementation.
     */
    struct ComputePipeline
    {
        void dispatch(){}
    };

    /*!
     * \brief Contains programs for a rendering pipeline, eg. vertex, fragment, compute shader (for GL3.3, just slap a program in there and put tighter restrictions on attaching)
     */
    struct Pipeline
    {
        Pipeline(uint32 flags){}
        void begin(){}
        void end(){}
    };
    /*!
     * \brief Contains a single shader, fragment and etc.
     */
    struct Shader
    {
        Shader(uint32 flags,cstring* src,int32 numSources){}
    };

    /*!
     * \brief Can be included in a drawcall to determine whether or not to render. Calls begin() before rendering occlusion shapes
     */
    struct OccludeQuery
    {
        void begin(){}
        void end(){}

        int64 getResult(){}
    };

    /*!
     * \brief Rendering surface, can be rendered to by RenderTarget or used as texture. Should support PBO upload for OpenGL
     */
    struct Surface
    {
        Surface(PixelFormat fmt, bool isArray, uint32 arraySize, uint32 mips, uint32 flags, ResourceAccess cl, const CByteData& data);

        int32 size(){}
        bool isArray(){}
        uint32 arraySize(){}
        uint32 mipmaps(){}
        PixelFormat format(){}
    };

    struct Texture2D; /* Simple 2D texture */
    struct VolumeTexture; /* 3D texture */
    struct Cubemap; /* Cube texture */

    struct Texture2DArray; /* Layered 2D texture*/
    struct CubemapArray; /* Layered cubemap */

    /*!
     * \brief Contains framebuffer and viewport information for resizing, used for screen rendering and shadow maps
     */
    struct RenderTarget
    {
        void attachSurface(){}
        void attachDepthStencilSurface(){}
    };
};

}
}

#endif
