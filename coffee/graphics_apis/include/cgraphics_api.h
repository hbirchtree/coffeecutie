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
        bool rasterize(){}
        bool culling(){}
        bool wireframeRender(){}
        bool polygonSmooth(){}
        bool dither(){}
        bool clampDepth(){}
        bool testScissor(){}
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
        bool swapEndiannes(){}
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
        static void addAttribute(VertexDescription& desc,uint32 offset,uint32 stride,TypeEnum type,bool normalized){}
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

    };
    /*!
     * \brief Contains a packed struct of parameters
     */
    struct IndirectBuffer : public VertexBuffer
    {

    };
    /*!
     * \brief Describes a particular uniform value, either inside a uniform block or separately
     */
    struct UniformDescriptor
    {
    };

    /*!
     * \brief Contains programs for a rendering pipeline, eg. vertex, fragment, compute shader
     */
    struct Pipeline
    {
    };
    /*!
     * \brief Contains a single shader, fragment and etc.
     */
    struct Shader
    {

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
        Surface(PixelFormat fmt, bool isArray, uint32 arraySize, uint32 mips, int32 flags, ResourceAccess cl, const CBitmap& bitm);
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
