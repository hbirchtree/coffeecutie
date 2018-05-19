#pragma once

#include "cgraphics_api_basic.h"
#include "cgraphics_api_thread.h"

#include <coffee/core/types/cdef/funtypes.h>
#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/cdef/pixtypes.h>

#include <coffee/core/types/vector_types.h>

#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/core/types/edef/colorenum.h>
#include <coffee/core/types/edef/graphicsenum.h>
#include <coffee/core/types/edef/logicenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

#include "cgraphics_pixops.h"

namespace Coffee{

struct HWDeviceInfo;
struct SWVersionInfo;

namespace Display{
struct CDProperties;
}
namespace RHI{

struct GraphicsAPI : GraphicsAPI_Base, GraphicsAPI_Threading
{
    /* Enumerations */
    enum TextureFlags
    {
        TextureDMABuffered      = 0x1,
        /*!< Use DMA transfer for texture if available on hardware */
        TextureAutoMipmapped    = 0x2,
        /*!< Generate automatic mipmaps on texture */
        TextureImmutable        = 0x4,
        /*!< Create an immutable texture */

    };
    enum AttributeFlags
    {
        AttributePacked         = 0x1,
        /*!< For integer types, loads them through the
         *  VertexAttribPointer() function */
        AttributeNormalization  = 0x2,
        /*!< For integers loaded with VertexAttribPointer(),
         *  apply normalization. Essentially converts to
         *  floating-point [-1.0, 1.0] */
    };

    struct GraphicsDevice
    {
    };

    struct GraphicsContext
    {
    };

    struct GraphicsThreadContext
    {
    };

    static void LoadAPI(bool UNUSED_PARAM debug_mode)
    {
    }

    static bool UnloadAPI()
    {
        return true;
    }

    static CString GetAPIName(GraphicsDevice const&)
    {
        return {};
    }
    static bool GetAPIVersion(GraphicsDevice const&, SWVersionInfo*)
    {
        return false;
    }

    static bool GetRendererInfo(GraphicsDevice const&, HWDeviceInfo*)
    {
        return false;
    }
    static bool GetRendererDriverInfo(GraphicsDevice const&, SWVersionInfo*)
    {
        return false;
    }
    static CString GetShaderLanguageName(GraphicsContext const&)
    {
        return {};
    }
    static bool GetShaderLanguageVersion(GraphicsContext const&, SWVersionInfo*)
    {
        return false;
    }

    /*!
     * \brief Contains state such as whether it should output to screen, culling, etc.
     */
    struct RasterizerState
    {
        RasterizerState():
            m_culling(0),
            m_colOp(LogicOp::NOOP),
            m_colMask({1,1,1,1, 0}),
            m_discard(false),
            m_wireframe(false),
            m_polysmooth(false),
            m_dither(false),
            m_doCull(false)
        {
        }

        bool discard()const{return m_discard;}
        uint32 culling()const{return m_culling;}
        bool wireframeRender()const{return m_wireframe;}
        bool polygonSmooth()const{return m_polysmooth;}
        bool dither()const{return m_dither;}
        bool doCull()const{return m_doCull;}

        LogicOp colorOp()const{return m_colOp;}
        CColorMask colorMask()const{return m_colMask;}

        uint32 m_culling;
        LogicOp m_colOp;
        CColorMask m_colMask;
        bool m_discard;
        bool m_wireframe;
        bool m_polysmooth;
        bool m_dither;
        bool m_doCull;
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
        ViewportState():
            ViewportState(1)
        {
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
            m_doBlend(false),
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
            m_mask(true),
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
        StencilFunc func()const{return m_func;}
        StencilOp op()const{return m_op;}

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
        VertexBuffer(ResourceAccess access,szptr size=0):m_access(access),m_size(size){}

        /*!
         * \brief Pointer to data, access restricted to original settings
         * \return
         */
        void* data(){return nullptr;}

        void alloc(){}
        void dealloc(){}

        void commit(szptr,c_cptr){}

        void* map(szptr = 0, szptr = 0){return nullptr;}
        void unmap(){}

    protected:
        ResourceAccess m_access;
        szptr m_size;
    };

    /*!
     * \brief Contains mesh indices for ordering of vertices
     */
    struct ElementBuffer : VertexBuffer
    {
        ElementBuffer(ResourceAccess access, szptr size)
            :VertexBuffer(access,size){}
    };
    /*!
     * \brief Contains data which will be read by a shader
     */
    struct UniformBuffer : VertexBuffer
    {
        UniformBuffer(ResourceAccess access, uint32 stride, szptr size)
            :VertexBuffer(access,size),m_stride(stride){}
    protected:
        uint32 m_stride;
    };
    /*!
     * \brief Contains data which will be modified by a shader, SSBO, transformed data, compute data
     */
    struct ShaderBuffer : VertexBuffer
    {
        ShaderBuffer(ResourceAccess access, uint32 stride, szptr size)
            :VertexBuffer(access,size),m_stride(stride){}
    protected:
        uint32 m_stride;
    };

    /*!
     * \brief Contains a packed struct of parameters. Not applicable to GL3.3
     */
    struct IndirectBuffer : VertexBuffer
    {
        IndirectBuffer(
                ResourceAccess access, uint32 flags,
                szptr stride, szptr size)
            :VertexBuffer(access,size),m_flags(flags),m_stride(stride){}
    protected:
        uint32 m_flags;
        uint32 m_stride;
    };

    /*!
     * \brief Describes a buffer used to upload pixel data
     */
    struct PixelBuffer : VertexBuffer
    {
        PixelBuffer(ResourceAccess access, szptr size)
            :VertexBuffer(access,size)
        {}
    };

    /*!
     * \brief Describes a singular vertex attribute
     */
    struct VertexAttribute
    {
        VertexAttribute():
            m_boffset(0),
            m_bassoc(0),
            m_idx(0),
            m_size(0),
            m_off(0),
            m_stride(sizeof(scalar)),
            m_flags(0),
            m_type(TypeEnum::Scalar),
            m_instanced(false)
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
        uint32 m_flags;
        TypeEnum m_type;
        bool m_instanced;
    };

    /*!
     * \brief Describes the association of a vertex buffer to a vertex attribute location
     */
    struct VertexBufferBinding
    {
        VertexBufferBinding(VertexBuffer& buf, VertexAttribute& desc):m_buffer(buf),m_descr(desc){}
    protected:
        VertexBuffer& m_buffer;
        VertexAttribute& m_descr;
    };

    /*!
     * \brief Describes a vertex buffer structure, intra-buffer and/or across buffers
     */
    struct VertexDescriptor
    {
        VertexDescriptor()
        {
        }

        void alloc(){}
        void dealloc(){}

        void bind(){}
        void bindBuffer(uint32,VertexBuffer&){}

        void addAttribute(VertexAttribute const&){}
        void setIndexBuffer(ElementBuffer const*){}
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

        int32 index() const {return m_idx;}
        cstring name() const {return m_name.c_str();}
        uint32 flags() const {return m_flags;}

        CString m_name;
        uint32 m_flags;
        int32 m_idx;

        int32 m_blkIdx;

        union{
            struct{
                int32 m_offset;
            };
            struct{
                int32 m_arrStride;
                int32 m_arrSize;
            };
        };
    };

    /*!
     * \brief Describes data to go into uniform value
     */
    struct UniformValue
    {
        UniformValue()
        {}
        Bytes const* data;
        uint32 flags;
    };

    /*!
     * \brief Describes vertex inputs and outputs as well as fragment shader outputs
     */
    struct ProgramParameter
    {
        CString m_name;
        uint32 m_flags;
        uint16 m_idx;
    };

    /*!
     * \brief Contains a single shader, fragment and etc.
     * On GL3.3 this will be a shader object
     * On GL4.3+ this will be a separable shaderprogram object
     */
    struct Shader
    {
        Shader():Shader(0){}
        Shader(uint32 flags):m_flags(flags){}

        bool compile(ShaderStage,Bytes&){return true;}
        void dealloc(){}
    protected:
        uint32 m_flags;
    };
    /*!
     * \brief Contains programs for a rendering pipeline, eg. vertex, fragment, compute shader (for GL3.3, just slap a program in there and put tighter restrictions on attaching)
     * On GL3.3, this will be a program object
     * On GL4.3+ this will be a pipeline object with separable shaderprogram objects
     */
    struct Pipeline
    {
        Pipeline():Pipeline(0){}
        Pipeline(uint32 flags):m_flags(flags){}
        void bind(){}
        void unbind(){}

        bool assemble(){return true;}
        void attach(Shader&,ShaderStage){}
    protected:
        uint32 m_flags;
    };

    /*!
     * \brief Use compute shaders when applicable,
     *  transform feedback + geometry shader otherwise (GL3.3).
     * Data-specification is depending on implementation.
     */
    struct ComputePipeline
    {
        void attach(Pipeline&){}

        void dispatch(){}
    };

    /*!
     * \brief Can be included in a drawcall to determine whether or
     *  not to render. Calls begin() before rendering occlusion shapes
     */
    template<typename T>
    struct OccludeQuery
    {
        OccludeQuery(){}
        OccludeQuery(T){}

        void begin(){}
        void end(){}

        /* How to kill perf: */
        int64 resulti(){return 0;}
        uint64 resultu(){return 0;}
    };

    /*!
     * \brief Rendering surface, can be rendered to by RenderTarget
     *  or used as texture.
     * Should support PBO upload for OpenGL
     */
    template<typename CSizeT, typename PointT>
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

        /*!
         * \brief Allocates any data necessary for this surface to exist.
         */
        void allocate(CSizeT const&,PixCmp){}
        void dealloc(){}

        void upload(BitFormat,PixCmp,CSizeT const&, c_cptr,
                    PointT const& = {}, u32 = 0)
        {
        }

        /*!
         * \brief A safe variant of `upload()`, can check
         *  bounds before copying a texture from memory.
         * The version of `upload()` taking a pointer has no
         *  ability to check this.
         * Please use this one for sanity.
         */
        void upload(BitFmt,PixCmp,CSizeT const&, Bytes const&,
                    PointT const& = {}, u32 = 0)
        {
        }

        uint32 size() const {return 0;}
        bool isArray() const {return b_array;}
        uint32 arraySize() const {return m_arrsize;}
        uint32 mipmaps() const {return m_mips;}
        PixelFormat format() const {return m_pixfmt;}

        u8& glTexHandle() {static u8 m_; return m_;}

        CSize texSize() const {return {};}

        PixelFormat m_pixfmt;
        bool b_array;
        uint32 m_arrsize;
        uint32 m_mips;
        uint32 m_flags;
        ResourceAccess m_access;
    };

    using Surface2D = Surface<CSize, CPoint> /* Simple 2D texture */;
    using Surface3D = Surface<CSize3, CPoint3>/* Simple 3D texture */;
    using SurfaceCube = Surface<CSize, CPoint> /* Cubemap texture */;

    using Surface2DArray = Surface<CSize3, CPoint3> /* 2D texture array */;
    using SurfaceCubeArray = Surface<CSize3, CPoint3> /* Cubemap array */;

    /*!
     * \brief Used to address a sampler from the shaders
     */
    struct SamplerHandle
    {
        u8& glTexHandle() {static u8 m_; return m_;}
        u8& glSamplerHandle() {static u8 m_; return m_;}
    };

    /*!
     * \brief Samples from a Surface/texture
     */
    struct Sampler
    {
        Sampler()
        {
        }

        void alloc(){}

        SamplerHandle handle(){return {};}

        template<typename Surface>
        void attach(Surface const*){}

        void bind(uint32){}

        void setFiltering(Filtering,Filtering,
                          Filtering = Filtering::None){}
        void setLODRange(Vecf2 const&) {}
        void setLODBias(scalar) {}
        void setEdgePolicy(u8, WrapPolicy) {}

        void enableShadowSampler(){}
    };

    struct Sampler2D : Sampler
    {
    };
    struct Sampler3D : Sampler
    {
    };
    struct SamplerCube : Sampler
    {
    };

    struct Sampler2DArray : Sampler
    {
    };
    struct SamplerCubeArray : Sampler
    {
    };

    /*!
     * \brief Images that may be read and written to by shaders
     * On GL 4.2+ this will be using the actual shader image functionality
     *
     * On GL 4.1 and lower, this might be implemented with a fragment shader
     *  and a separate read/write texture, using fragment location outputs for
     *  redirection to textures.
     * Copies back to original texture will be done by CopyTexSubImage*
     */
    struct ShaderImage
    {
        void bind(uint32){}
        template<typename Surface>
        void attach(Surface*,PixFmt,ResourceAccess,uint32,uint32 = 0){}
    };

    /*!
     * \brief RenderTargets may use these for GPU-side storage that is not retrievable by the CPU
     */
    struct RenderDummy
    {
        void allocate(PixelFormat,DBuffers,uint32,CSize){}
        void deallocate(){}
    };

    /*!
     * \brief Contains framebuffer and viewport information for resizing,
     *  used for screen rendering and shadow maps
     */
    struct RenderTarget
    {
        constexpr RenderTarget(){}

        /*!
     * \brief Do a framebuffer blit  to another framebuffer
     */
        void blit(CRect64 const&,RenderTarget&,CRect64 const&,DBuffers) const{}

        /*!
         * \brief Attach a surface to this framebuffer
     */
        template<typename Surface>
        void attachSurface(Surface const&, uint32, uint32 = 0){}
        void attachSurface(RenderDummy const&){}

        template<typename Surface>
        void attachDepthStencilSurface(Surface const&, uint32){}
        template<typename Surface>
        void attachDepthSurface(Surface const&, uint32){}

        void resize(uint32,CRect64 const&){}

        void clear(uint32, Vecf4 const&){}
        void clear(bigscalar){}
        void clear(bigscalar, int32){}
        void clear(uint32, Vecf4 const&, bigscalar){}
        void clear(uint32, Vecf4 const&, bigscalar, int32){}

        void use(FramebufferT){}
    };

    static void SetRenderTarget(RenderTarget const&);

    struct DrawCall
    {
        DrawCall(bool indexed, bool instanced):
            m_idxd(indexed), m_inst(instanced)
        {
            setPrim(Prim::Triangle);
            setCreat(PrimCre::Explicit);
        }
        DrawCall(bool indexed, bool instanced,
                 Prim p, PrimCre c):
            DrawCall(indexed, instanced)
        {
            setPrim(p);
            setCreat(c);
        }
        DrawCall(bool indexed, bool instanced,
                 Prim p):
            DrawCall(indexed, instanced, p,
                     PrimCre::Explicit)
        {
        }
        DrawCall():
            DrawCall(false, false)
        {
        }

        DrawCall(DrawCall const& other):
            m_notouch(other.m_notouch)
        {
        }

        FORCEDINLINE bool indexed()const{return m_idxd;}
        FORCEDINLINE bool instanced()const{return m_inst;}
        FORCEDINLINE Prim primitive()const
        {
            /* m_primitive is restricted to 0-3 range */
            return static_cast<Prim>(m_primitive);
        }
        FORCEDINLINE PrimCre primitiveMode()const
        {
            if(m_pmode < static_cast<u32>(PrimCre::MaxValue))
                return static_cast<PrimCre>(m_pmode);
            else
                return PrimCre::Explicit;
        }

        FORCEDINLINE void setPrim(Prim e)
        {
            m_primitive = C_CAST<u8>(e);
        }
        FORCEDINLINE void setCreat(PrimCre e)
        {
            m_pmode = C_CAST<u8>(e);
        }

        union{
            struct {
                bool m_idxd:1;
                bool m_inst:1;
                u8 m_primitive:2;
                u8 m_pmode:3;
                bool m_unused:1;
            };
            u8 m_notouch = 0x0;
        };
    };

    /*!
     * \brief DrawInstanceData struct describes a single mesh entity
     *  in an array buffer (+ element buffer)
     * It does this by specifying counts and offsets of
     *  vertices and elements.
     * It also helps describe instances.
     * The use of this information is up to the RHI.
     * Potentially, instance offsets may be implemented as buffer offsets or similar.
     */
    struct DrawInstanceData
    {
        DrawInstanceData(uint32 v = 0, uint32 e = 0, uint32 i = 0):
            m_verts(v),m_elems(e),m_insts(i),
            m_eltype(TypeEnum::UByte),
            m_voff(0),m_eoff(0),m_ioff(0)
        {}

        FORCEDINLINE uint32 vertices()const{return m_verts;}
        FORCEDINLINE uint32 elements()const{return m_elems;}
        FORCEDINLINE TypeEnum elementType()const{return m_eltype;}
        FORCEDINLINE uint32 instances()const{return m_insts;}

        FORCEDINLINE int32 vertexOffset()const{return m_voff;}
        FORCEDINLINE uint32 indexOffset()const{return m_eoff;}
        FORCEDINLINE uint32 instanceOffset()const{return m_ioff;}

        uint32 m_verts;
        uint32 m_elems;
        uint32 m_insts;

        TypeEnum m_eltype;

        int32 m_voff;
        uint32 m_eoff;
        uint32 m_ioff;
    };

    /*!
     * \brief Keeps track of textures, uniforms and miscellaneous buffers
     */
    struct ShaderUniformState
    {
        void setUniform(UniformDescriptor const&,UniformValue*){}
        void setSampler(UniformDescriptor const&,SamplerHandle*){}
    };

    using PipelineState = Map<ShaderStage,const ShaderUniformState &>;

    static void GetShaderUniformState(Pipeline&,Vector<UniformDescriptor>*,
                                      Vector<ProgramParameter>*){}

    /*!
     * \brief Draw primitives regardlessly
     * \param d General drawcall settings
     * \param i Data associated with instance of drawcall
     */
    static void Draw(const Pipeline &,
                     PipelineState const&,
                     VertexDescriptor&,
                     DrawCall const&,
                     DrawInstanceData const&,
                     OccludeQuery<int>* = nullptr){}
    /*!
     * \brief Draw primitives with occlusion query
     * \param d
     * \param i
     * \param c An occlusion query to be considered in the drawcall
     */
    static void DrawConditional(const Pipeline &,
                                PipelineState const&,
                                VertexDescriptor&,
                                DrawCall const&,
                                DrawInstanceData const&,
                                OccludeQuery<int> const&){}

    static void SetRasterizerState(RasterizerState const&){}
    static void SetTessellatorState(TessellatorState const&){}
    static void SetViewportState(ViewportState const&,uint32 = 0){}
    static void SetBlendState(BlendState const&){}
    static void SetDepthState(DepthState<uint32> const&){}
    static void SetStencilState(StencilState<uint32,uint32> const&,uint32){}
    static void SetPixelProcessState(PixelProcessState const&){}
    static void SetShaderUniformState(Pipeline&,ShaderStage,ShaderUniformState const&){}

    /*!
     * \brief Performs operations that might boost performance or otherwise clean up any mess
     *   left by the initialization parts.
     *   In GLM, this releases the shader compiler.
     */
    static void PreDrawCleanup(){}

    struct Util
    {
        template<typename Surface>
        static void DumpTexture(Surface const& s);
    };
};

struct GraphicsProfiler
{
    /*!
     * \brief Queries the API for performance information, begins and ends
     */
    struct PerfQuery
    {
        PerfQuery(ProfilingTerm term):m_term(term){}

        void begin(){}
        void end(){}

        int64 resulti(){return 0;}
        uint64 resultu(){return 0;}

        const ProfilingTerm m_term;
    };

    template<typename RT>
    /*!
     * \brief Will blit color, stencil and depth buffers to a given framebuffer, begins and ends, in which time it captures the data
     * Data that already exists in a framebuffer is included, because it's simpler
     */
    struct BufferQuery
    {
        BufferQuery(RT& t,DBuffers b) : m_rtarget(t),m_dtarget(),m_buffers(b) {}

        bool enabled(){return true;}

        void resize(CSize const&){}

        void begin(){}
        void end(){}

        RT& output(){return m_rtarget;}
        RT& debugTarget(){return m_dtarget;}

        RT& m_rtarget;
        RT m_dtarget;
        const DBuffers m_buffers;
    };

    template<typename PIP>
    struct PipelineDumper
    {
        PipelineDumper(PIP& p):m_pipeline(p){}

        void dump(cstring){}

        PIP& m_pipeline;
    };
};

struct GraphicsDebug
{
    struct ScopeMarker
    {
        /* Creates debug group */
        ScopeMarker(cstring) {}

        /* Removes debug group */
        ~ScopeMarker() {}
    };

    template<typename T>
    static void Mark(T&, cstring) {}
};

struct NullAPI : GraphicsAPI
{
    using OccludeQuery = GraphicsAPI::OccludeQuery<int>;

    enum DummyValues
    {
        TextureDMABuffered = 0,
    };

    /* Null renderer types, do not inherit from these */
    struct S_2D : Surface2D
    {
        S_2D(PixelFormat fmt,uint32 mips = 1, uint32 flags = 0):
            Surface2D(fmt,false,0,mips,flags)
        {}
        CSize m_size;
    };
    struct S_3D : Surface3D
    {
        S_3D(PixelFormat fmt,uint32 mips = 1, uint32 flags = 0):
            Surface3D(fmt,false,0,mips,flags)
        {}
        CSize3 m_size;
    };
    using S_Cube = SurfaceCube;

    struct S_2DA : Surface2DArray
    {
        S_2DA(PixelFormat fmt,uint32 mips = 1, uint32 flags = 0):
            Surface2DArray(fmt,false,0,mips,flags)
        {}
        CSize3 m_size;
    };

    using API_CONTEXT = Function<bool(bool)>;

    using FB_T = RenderTarget;
    using RBUF = RenderDummy;

    using S_CubeA = SurfaceCubeArray;

    using SM_2D = Sampler2D;
    using SM_3D = Sampler3D;
    using SM_Cube = SamplerCube;

    using SM_2DA = Sampler2DArray;
    using SM_CubeA = SamplerCubeArray;

    using SHD = Shader;
    using PIP = Pipeline;

    using V_ATTR = VertexAttribute;
    using V_DESC = VertexDescriptor;

    using BUF_A = VertexBuffer;
    using BUF_E = ElementBuffer;
    using BUF_U = UniformBuffer;
    using BUF_S = ShaderBuffer;
    using BUF_P = PixelBuffer;
    using BUF_DRAW = IndirectBuffer;

    using D_CALL = DrawCall;
    using D_DATA = DrawInstanceData;

    using UNIFDESC = UniformDescriptor;
    using UNIFVAL = UniformValue;
    using UNIFSMP = SamplerHandle;
    using PPARAM = ProgramParameter;

    using USTATE = ShaderUniformState;
    using RASTSTATE = RasterizerState;
    using VIEWSTATE = ViewportState;
    using BLNDSTATE = BlendState;
    using DEPTSTATE = DepthState<uint32>;
    using STENSTATE = StencilState<uint32,uint32>;
    using TSLRSTATE = TessellatorState;
    using PIXLSTATE = PixelProcessState;

    using PSTATE = PipelineState;

    using G_CTXT = GraphicsContext;
    using G_TCTXT = GraphicsThreadContext;
    using G_DEV = GraphicsDevice;

    using Q_OCC = OccludeQuery;

    static FB_T& DefaultFramebuffer()
    {
        static FB_T m_;
        return m_;
    }

    static API_CONTEXT GetLoadAPI()
    {
        return [](bool)
        {
            return true;
        };
    }

    /* We define a profiler namespace */
    struct PRF
    {
        using QRY_DBUF = GraphicsProfiler::BufferQuery<FB_T>;
        using QRY_PERF = GraphicsProfiler::PerfQuery;
        using QRY_PIPDMP = GraphicsProfiler::PipelineDumper<PIP>;
    };

    struct DBG : GraphicsDebug
    {
        using SCOPE = GraphicsDebug::ScopeMarker;
    };
};

}
}