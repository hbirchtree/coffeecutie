#pragma once

#include <coffee/interfaces/cgraphics_api.h>

namespace gexxo {

using namespace typing::vector_types;

using size_2d = typing::geometry::size_2d<libc_types::i32>;

using vec4_u8 = typing::vectors::tvector<libc_types::u8, 4>;

extern void initialize();
extern void infiniteLoop();
extern void swapBuffers();

extern size_2d framebufferSize();

struct RenderTarget : Coffee::RHI::GraphicsAPI::RenderTarget
{
    void clear(u32, Vecf4 const& color);
    void clear(f32 depth);

    FORCEDINLINE void clear(f32 depth, C_UNUSED(i32 stencil))
    {
        clear(depth);
    }
    FORCEDINLINE void clear(u32, Vecf4 const& color, f32 depth)
    {
        clear(0, color);
        clear(depth);
    }
    FORCEDINLINE void clear(
        u32, Vecf4 const& color, f32 depth, C_UNUSED(i32 stencil))
    {
        clear(0, color);
        clear(depth);
    }

    void*   mode;
    void*   framebuffer;
    vec4_u8 color;
    u32     depth;
    u8      depth_bits;
};

struct VertexBuffer : Coffee::RHI::GraphicsAPI::VertexBuffer
{
    FORCEDINLINE void commit(semantic::BytesConst const& data)
    {
        semantic::chunk_ops::MemCpy(data, this->data);
    }
    FORCEDINLINE void commit(szptr size, c_cptr data = nullptr)
    {
        commit(semantic::BytesConst::From(data, size));
    }

    FORCEDINLINE semantic::Bytes map(szptr offset, szptr size)
    {
        return data.at(offset, size);
    }

    semantic::Bytes data;
};

struct ElementBuffer : VertexBuffer
{
};

using VertexAttrib = Coffee::RHI::GraphicsAPI::VertexAttribute;

struct VertexDescriptor
{
    void bind();
    void bindBuffer(libc_types::u32 i, VertexBuffer& buf);

    void addAttribute(VertexAttrib const& attr);
    void setIndexBuffer(ElementBuffer const* buf);

    stl_types::Map<u32, VertexBuffer*> buffers;
    stl_types::Vector<VertexAttrib>    attribs;
    ElementBuffer*                     indexBuffer;
};

struct GXAPI : Coffee::RHI::GraphicsAPI
{
    using API_CONTEXT = stl_types::Function<bool(bool)>;

    struct OPTS
    {
    };

    static gexxo::RenderTarget* DefaultFramebuffer();
    static API_CONTEXT          GetLoadAPI(OPTS const&);

    using RBUF = RenderDummy;

    using S_CubeA = SurfaceCubeArray;

    using SM_2D   = Sampler2D;
    using SM_3D   = Sampler3D;
    using SM_Cube = SamplerCube;

    using SM_2DA   = Sampler2DArray;
    using SM_CubeA = SamplerCubeArray;

    using SHD = Shader;
    using PIP = Pipeline;

    using V_ATTR = VertexAttribute;
    using V_DESC = VertexDescriptor;

    using BUF_A    = VertexBuffer;
    using BUF_E    = ElementBuffer;
    using BUF_U    = UniformBuffer;
    using BUF_S    = ShaderBuffer;
    using BUF_P    = PixelBuffer;
    using BUF_DRAW = IndirectBuffer;

    using D_CALL = DrawCall;
    using D_DATA = DrawInstanceData;

    using UNIFDESC = UniformDescriptor;
    using UNIFVAL  = UniformValue;
    using UNIFSMP  = SamplerHandle;
    using PPARAM   = ProgramParameter;

    using USTATE    = ShaderUniformState;
    using RASTSTATE = RasterizerState;
    using VIEWSTATE = ViewportState;
    using BLNDSTATE = BlendState;
    using DEPTSTATE = DepthState<libc_types::u32>;
    using STENSTATE = StencilState<libc_types::u32, libc_types::u32>;
    using TSLRSTATE = TessellatorState;
    using PIXLSTATE = PixelProcessState;

    using PSTATE = PipelineState;

    using G_CTXT  = GraphicsContext;
    using G_TCTXT = GraphicsThreadContext;
    using G_DEV   = GraphicsDevice;

    using Q_OCC = OccludeQuery<int>;
};

} // namespace gexxo
