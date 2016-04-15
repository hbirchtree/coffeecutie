#ifndef COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H
#define COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H

#include <coffee/core/types/tdef/integertypes.h>

#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/cdef/funtypes.h>
#include <coffee/core/types/cdef/pixtypes.h>

#include <coffee/core/types/cdef/timetypes.h>

#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/core/types/edef/colorenum.h>
#include <coffee/core/types/edef/logicenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace RHI{

FORCEDINLINE szptr GetPixSize(BitFormat fmt, PixelComponents comp, szptr pixels)
{
    szptr pxsz = 0;
    switch(fmt)
    {
    case BitFormat::Byte:
    case BitFormat::ByteR:
    case BitFormat::UByte:
    case BitFormat::UByteR:
    case BitFormat::UByte_332:
    case BitFormat::UByte_233R:
        pxsz = 1;
        break;
    case BitFormat::Short:
    case BitFormat::ShortR:
    case BitFormat::UShort:
    case BitFormat::UShortR:
    case BitFormat::UShort_565:
    case BitFormat::UShort_565R:
    case BitFormat::UShort_5551:
    case BitFormat::UShort_1555R:
    case BitFormat::Scalar_16:
        pxsz = 2;
        break;
    case BitFormat::Int:
    case BitFormat::IntR:
    case BitFormat::UInt:
    case BitFormat::UIntR:
    case BitFormat::UInt_5999R:
    case BitFormat::UInt_1010102:
    case BitFormat::UInt_2101010R:
    case BitFormat::Scalar_32:
    case BitFormat::Scalar_11_11_10:
        pxsz = 4;
        break;
    case BitFormat::Scalar_64:
    case BitFormat::Scalar_32_Int_24_8:
        pxsz = 8;
    }
    switch(comp)
    {
    case PixelComponents::R:
    case PixelComponents::G:
    case PixelComponents::B:
    case PixelComponents::Stencil:
    case PixelComponents::Depth:
    case PixelComponents::DepthStencil:
        pxsz *= 1;
        break;
    case PixelComponents::RG:
        pxsz *= 2;
        break;
    case PixelComponents::RGB:
    case PixelComponents::BGR:
        pxsz *= 3;
        break;
    case PixelComponents::RGBA:
    case PixelComponents::BGRA:
        pxsz *= 4;
        break;
    }

    return pxsz*pixels;
}

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
        RasterizerState():
            m_discard(false),
            m_culling(false),
            m_wireframe(false),
            m_polysmooth(false),
            m_dither(false),
            m_colOp(LogicOp::NOOP),
            m_colMask()
        {
        }

        bool discard()const{return m_discard;}
        uint32 culling()const{return m_culling;}
        bool wireframeRender()const{return m_wireframe;}
        bool polygonSmooth()const{return m_polysmooth;}
        bool dither()const{return m_dither;}

        LogicOp colorOp()const{return m_colOp;}
        CColorMask colorMask()const{return m_colMask;}

        bool m_discard;
        uint32 m_culling;
        bool m_wireframe;
        bool m_polysmooth;
        bool m_dither;
        LogicOp m_colOp;
        CColorMask m_colMask;
    };

    struct TessellatorState
    {
        TessellatorState():
            m_patches(1)
        {
        }

        uint32 patchCount()const{return m_patches;}

        uint32 m_patches;
    };

    /*!
     * \brief Set viewport arrays. We will use this for VR rendering later. (Geometry shader way)
     */
    struct ViewportState
    {
        using VRect = CRect;
        using DField = ZField64;
        using SRect = CRect;

        ViewportState(szptr views):
            m_mview(false)
        {
            m_view.resize(views);
            m_depth.resize(views);
            m_scissor.resize(views);
        }

        bool multiview()const{return m_mview;}
        szptr viewCount()const{return m_view.size();}
        VRect const& view(uint32 i)const{return m_view[i];}
        DField const& depth(uint32 i)const{return m_depth[i];}
        VRect const& scissor(uint32 i)const{return m_scissor[i];}

        bool m_mview;
        Vector<VRect> m_view;
        Vector<DField> m_depth;
        Vector<SRect> m_scissor;
    };

    struct BlendState
    {
        BlendState():
            m_doBlend(true),
            m_additive(false),
            m_alphaCoverage(false)
        {
        }

        bool blend()const{return m_doBlend;}
        bool additive()const{return m_additive;}
        bool sampleAlphaCoverage()const{return m_alphaCoverage;}

        bool m_doBlend;
        bool m_additive;
        bool m_alphaCoverage;
    };

    template<typename DepthFun>
    struct DepthState
    {
        DepthState():
            m_test(true),
            m_clamp(false),
            m_mask(false),
            m_func()
        {
        }

        bool testDepth()const{return m_test;}
        bool clampDepth()const{return m_clamp;}
        bool mask()const{return m_mask;}
        DepthFun fun()const{return m_func;}

        bool m_test;
        bool m_clamp;
        bool m_mask;
        DepthFun m_func;
    };

    template<typename StencilFunc,typename StencilOp>
    struct StencilState
    {
        StencilState():
            m_test(false),
            m_mask(false),
            m_func(),
            m_op()
        {
        }

        bool testStencil()const{return 0;}
        uint32 mask()const{return 0;}
        StencilFunc func()const{return StencilFunc();}
        StencilOp op()const{return StencilOp();}

        bool m_test;
        uint32 m_mask;
        StencilFunc m_func;
        StencilOp m_op;
    };

    struct PixelProcessState
    {
        PixelProcessState():
            m_swap(false),
            m_lsbf(false),
            m_rlen(0),
            m_imgh(0),
            m_align(0)
        {
        }

        bool swapEndianness()const{return m_swap;}
        bool lsbFirst()const{return m_lsbf;}
        uint32 rowLength()const{return m_rlen;}
        uint32 imgHeight()const{return m_imgh;}
        uint32 alignment()const{return m_align;}

        bool m_swap;
        bool m_lsbf;
        uint32 m_rlen;
        uint32 m_imgh;
        uint32 m_align;
    };

    struct DebugMessage
    {

        DebugComponent component()const{return m_comp;}
        DebugType type()const{return m_type;}
        Severity severity()const{return m_sev;}
        CString& message(){return m_msg;}
        CString const& message()const{return m_msg;}

        DebugComponent m_comp;
        DebugType m_type;
        Severity m_sev;
        CString m_msg;
    };

    /*!
     * \brief Base of all buffers, can contain vertices and etc.
     */
    struct VertexBuffer
    {
        VertexBuffer(ResourceAccess access, szptr size):m_access(access),m_size(size){}

        /*!
         * \brief Pointer to data, access restricted to original settings
         * \return
         */
        void* data();

    protected:
        ResourceAccess m_access;
        szptr m_size;
    };

    /*!
     * \brief Contains mesh indices for ordering of vertices
     */
    struct IndexBuffer : VertexBuffer
    {
        IndexBuffer(ResourceAccess access, TypeEnum itype, szptr size):VertexBuffer(access,size),m_itype(itype){}
    protected:
        TypeEnum m_itype;
    };
    /*!
     * \brief Contains data which will be read by a shader
     */
    struct UniformBuffer : VertexBuffer
    {
        UniformBuffer(ResourceAccess access, uint32 stride, szptr size):VertexBuffer(access,size),m_stride(stride){}
    protected:
        uint32 m_stride;
    };
    /*!
     * \brief Contains data which will be modified by a shader, SSBO, transformed data, compute data
     */
    struct ShaderBuffer : VertexBuffer
    {
        ShaderBuffer(ResourceAccess access, uint32 stride, szptr size):VertexBuffer(access,size),m_stride(stride){}
    protected:
        uint32 m_stride;
    };

    /*!
     * \brief Contains a packed struct of parameters. Not applicable to GL3.3
     */
    struct IndirectBuffer : VertexBuffer
    {
        IndirectBuffer(ResourceAccess access, uint32 flags, uint32 stride, szptr size):VertexBuffer(access,size),m_flags(flags),m_stride(stride){}
    protected:
        uint32 m_flags;
        uint32 m_stride;
    };

    struct VertexAttribute
    {
        VertexAttribute():
            m_boffset(0),
            m_bassoc(0),
            m_size(0),
            m_off(0),
            m_stride(sizeof(scalar)),
            m_type(TypeEnum::Scalar)
        {
        }

        uint64 bufferOffset()const{return m_boffset;}
        uint32 bufferAssociation()const{return m_bassoc;}
        uint32 index()const{return m_idx;}
        uint32 size()const{return m_size;}
        uint32 offset()const{return m_off;}
        uint32 stride()const{return m_stride;}
        TypeEnum type()const{return m_type;}
        bool instanced()const{return m_instanced;}

        uint64 m_boffset;
        uint32 m_bassoc;
        uint32 m_idx;
        uint32 m_size;
        uint32 m_off;
        uint32 m_stride;
        TypeEnum m_type;
        bool m_instanced;
    };

    struct VertexBufferBinding
    {
        VertexBufferBinding(VertexBuffer& buf, VertexAttribute& desc):m_buffer(buf),m_descr(desc){}
    protected:
        VertexBuffer& m_buffer;
        VertexAttribute& m_descr;
    };

    struct VertexDescriptor
    {
        VertexDescriptor()
        {
        }

        void addAttribute(VertexAttribute const&){}
        void setIndexBuffer(IndexBuffer const&){}
    };

    /*!
     * \brief Describes a particular uniform value, either inside a uniform block or separately
     */
    struct UniformDescriptor
    {
        UniformDescriptor():
            m_name(),
            m_flags(0),
            m_idx(0)
        {
        }

        int32 index(){return m_idx;}
        cstring name(){return m_name.c_str();}
        uint32 flags(){return m_flags;}

        CString m_name;
        uint32 m_flags;
        int32 m_idx;
    };

    /*!
     * \brief Keeps track of textures, uniforms and miscellaneous buffers
     */
    struct ShaderUniformState
    {
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
     * \brief Rendering surface, can be rendered to by RenderTarget or used as texture.
     *  Should support PBO upload for OpenGL
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
        bool isArray() const {return b_array;}
        uint32 arraySize() const {return m_arrsize;}
        uint32 mipmaps() const {return m_mips;}
        PixelFormat format() const {return m_pixfmt;}

        PixelFormat m_pixfmt;
        bool b_array;
        uint32 m_arrsize;
        uint32 m_mips;
        uint32 m_flags;
        ResourceAccess m_access;
    };

    struct Surface2D; /* Simple 2D texture */
    struct Surface3D; /* 3D texture */
    struct SurfaceCube; /* Cube texture */

    struct Surface2DArray; /* Layered 2D texture*/
    struct SurfaceCubeArray; /* Layered cubemap */

    struct Sampler
    {
        Sampler()
        {
        }

        void attach(Surface const&);
    };

    /*!
     * \brief Contains framebuffer and viewport information for resizing,
     *  used for screen rendering and shadow maps
     */
    struct RenderTarget
    {
        void attachSurface(){}
        void attachDepthStencilSurface(){}
    };

    static void SetRenderTarget(RenderTarget const&);

    struct DrawCall
    {
        bool indexed()const{return m_idxd;}
        bool instanced()const{return m_inst;}

        bool m_idxd;
        bool m_inst;
    };

    struct DrawInstanceData
    {
        uint32 vertices()const{return m_verts;}
        uint32 elements()const{return m_elems;}
        TypeEnum elementType()const{return m_eltype;}
        uint32 instances()const{return m_insts;}

        int32 vertexOffset()const{return m_voff;}
        uint32 indexOffset()const{return m_eoff;}
        uint32 instanceOffset()const{return m_ioff;}

        uint32 m_verts;
        uint32 m_elems;
        uint32 m_insts;

        TypeEnum m_eltype;

        int32 m_voff;
        uint32 m_eoff;
        uint32 m_ioff;
    };

    /*!
     * \brief Draw primitives regardlessly
     * \param d General drawcall settings
     * \param i Data associated with instance of drawcall
     */
    static void Draw(DrawCall const& d,DrawInstanceData const& i);
    /*!
     * \brief Draw primitives with occlusion query
     * \param d
     * \param i
     * \param c An occlusion query to be considered in the drawcall
     */
    static void DrawConditional(DrawCall const& d,DrawInstanceData const& i,OccludeQuery const& c);

    static void SetRasterizerState(){}
    static void SetTessellatorState(){}
    static void SetViewportState(){}
    static void SetBlendState(){}
    static void SetDepthState(){}
    static void SetStencilState(){}
    static void SetPixelProcessState(){}
    static void SetShaderUniformState(){}

    struct Util
    {
        static void DumpTexture(Surface const& s);
    };
};

}
}

#endif
