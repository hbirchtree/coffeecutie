#ifndef COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H
#define COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H

#include <coffee/core/CTypes>

namespace Coffee{
namespace CRHI{

struct GraphicsProfiler
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

struct GraphicsAPI
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
        bool discard(){return 0;}
        bool culling(){return 0;}
        bool wireframeRender(){return 0;}
        bool polygonSmooth(){return 0;}
        bool dither(){return 0;}
        bool clampDepth(){return 0;}
        bool testScissor(){return 0;}

        LogicOp colorOp(){}
        CColorMask colorMask(){}
        bool depthMask(){}
    };

    struct TessellatorState
    {
        uint32 patchCount(){return 0;}
    };

    /*!
     * \brief Set viewport arrays. We will use this for VR rendering later. (Geometry shader way)
     */
    struct ViewportState
    {
        bool multiview(){return 0;}
        uint32 viewCount(){return 0;}
        CRect view(uint32){}
        CZField64 depth(uint32){}
        CRect scissor(uint32){}
    };

    struct BlendState
    {
        bool blend(){return 0;}
        bool additive(){return 0;}
        bool sampleAlphaCoverage(){return 0;}
    };

    struct DepthStencilState
    {
        bool testDepth(){return 0;}
        bool testStencil(){return 0;}
    };

    struct PixelProcessState
    {
        bool swapEndianness(){return 0;}
        bool lsbFirst(){return 0;}
        uint32 rowLength(){return 0;}
        uint32 imgHeight(){return 0;}
        uint32 alignment(){return 0;}
    };

    struct DebugMessage
    {
        DebugComponent component(){}
        DebugType type(){}
        Severity severity(){}
        CString message(){}
    };

    struct VertexAttribute
    {
        bool interleaved(){return 0;}
        int32 offset(){return 0;}
        int32 stride(){return 0;}
        TypeEnum type(){return 0;}
    };

    /*!
     * \brief Base of all buffers, can contain vertices and etc.
     */
    struct VertexBuffer
    {
        VertexBuffer(ResourceAccess access, uint32 size):m_access(access),m_size(size){}

        void* data();
        void commitMemory();
    protected:
        ResourceAccess m_access;
        uint32 m_size;
    };

    struct VertexBufferBinding
    {
        VertexBufferBinding(VertexBuffer* buf, VertexAttribute* desc):m_buffer(buf),m_descr(desc){}
    protected:
        VertexBuffer* m_buffer;
        VertexAttribute* m_descr;
    };

    /*!
     * \brief Contains mesh indices for ordering of vertices
     */
    struct IndexBuffer : public VertexBuffer
    {
        IndexBuffer(ResourceAccess access, TypeEnum itype, uint32 size):VertexBuffer(access,size),m_itype(itype){}
    protected:
        TypeEnum m_itype;
    };
    /*!
     * \brief Contains data which will be read by a shader
     */
    struct UniformBuffer : public VertexBuffer
    {
        UniformBuffer(ResourceAccess access, uint32 stride, uint32 size):VertexBuffer(access,size),m_stride(stride){}
    protected:
        uint32 m_stride;
    };
    /*!
     * \brief Contains data which will be modified by a shader, SSBO, transformed data, compute data
     */
    struct ShaderBuffer : public VertexBuffer
    {
        ShaderBuffer(ResourceAccess access, uint32 stride, uint32 size):VertexBuffer(access,size),m_stride(stride){}
    protected:
        uint32 m_stride;
    };
    /*!
     * \brief Contains a packed struct of parameters. Not applicable to GL3.3
     */
    struct IndirectBuffer : public VertexBuffer
    {
        IndirectBuffer(ResourceAccess access, uint32 flags, uint32 stride, uint32 size):VertexBuffer(access,size),m_flags(flags),m_stride(stride){}
    protected:
        uint32 m_flags;
        uint32 m_stride;
    };
    /*!
     * \brief Describes a particular uniform value, either inside a uniform block or separately
     */
    struct UniformDescriptor
    {
        uint32 index(){return 0;}
        cstring name(){return 0;}
        uint32 flags(){return 0;}
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
     * On GL3.3, this will be a program object
     * On GL4.3+ this will be a pipeline object with separable shaderprogram objects
     */
    struct Pipeline
    {
        Pipeline(uint32 flags):m_flags(flags){}
        void begin(){}
        void end(){}
    protected:
        uint32 m_flags;
    };
    /*!
     * \brief Contains a single shader, fragment and etc.
     * On GL3.3 this will be a shader object
     * On GL4.3+ this will be a separable shaderprogram object
     */
    struct Shader
    {
        Shader(uint32 flags):m_flags(flags){}
    protected:
        uint32 m_flags;
    };

    /*!
     * \brief Can be included in a drawcall to determine whether or not to render. Calls begin() before rendering occlusion shapes
     */
    struct OccludeQuery
    {
        void begin(){}
        void end(){}

        int64 getResult(){return 0;}
    };

    /*!
     * \brief Rendering surface, can be rendered to by RenderTarget or used as texture. Should support PBO upload for OpenGL
     */
    struct Surface
    {
        Surface(PixelFormat fmt, bool isArray = false, uint32 arraySize = 0,
                uint32 mips = 1, uint32 flags = 0,
                ResourceAccess cl = ResourceAccess::ReadOnly)
            :m_pixfmt(fmt),
              b_array(isArray),
              m_arrsize(arraySize),
              m_mips(mips),
              m_flags(flags),
              m_access(cl)
        {}

        uint32 size() const {return 0;}
        bool isArray() const {return 0;}
        uint32 arraySize() const {return 0;}
        uint32 mipmaps() const {return 0;}
        PixelFormat format() const {return (PixelFormat)0;}

    private:
        PixelFormat m_pixfmt;
        bool b_array;
        uint32 m_arrsize;
        uint32 m_mips;
        uint32 m_flags;
        ResourceAccess m_access;
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

    struct DrawCall
    {
        bool indexed(){return 0;}
        bool instanced(){return 0;}

        uint32 vertices(){return 0;}
        uint32 elements(){return 0;}
        uint32 instances(){return 0;}

        int32 vertexOffset(){return 0;}
        int32 indexOffset(){return 0;}
        int32 instanceOffset(){return 0;}
    };

    static void Draw(DrawCall const&);

    struct Util
    {
        static void DumpTexture(Surface const& s);
    };
};

}
}

#endif
