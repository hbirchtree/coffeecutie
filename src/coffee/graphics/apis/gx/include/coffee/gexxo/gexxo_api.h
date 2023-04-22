#pragma once

#include <coffee/interfaces/cgraphics_api.h>

#define GEXXO_API "gexxo::"

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
    using parent_type = Coffee::RHI::GraphicsAPI::VertexBuffer;

    VertexBuffer(semantic::RSCA access, libc_types::szptr size) :
        parent_type(access, size)
    {
    }

    FORCEDINLINE void commit(semantic::BytesConst const& data)
    {
        using semantic::RSCA;

        if(enum_helpers::feval(this->data.access() & RSCA::WriteOnly))
            Throw(undefined_behavior(
                GEXXO_API "cannot assign read-only data to writeable region"));

        if(!data.elements || !data.size)
            Throw(undefined_behavior(
                std::string("indexed ") + std::to_string(data.elements)));

        this->data =
            semantic::Bytes::From(const_cast<u8*>(data.data), data.size);
    }
    FORCEDINLINE void commit(szptr size, c_cptr data = nullptr)
    {
        commit(semantic::BytesConst::FromBytes(data, size));
    }

    FORCEDINLINE semantic::Bytes map(szptr offset, szptr size)
    {
        return *data.at(offset, size);
    }

    semantic::Bytes data;
};

struct ElementBuffer : VertexBuffer
{
};

using VertexAttrib = Coffee::RHI::GraphicsAPI::VertexAttribute;

struct VertexDescriptor : Coffee::RHI::GraphicsAPI::VertexDescriptor
{
    void bind();

    void bindBuffer(libc_types::u32 i, VertexBuffer& buf)
    {
        buffers[i] = &buf;
    }
    void addAttribute(VertexAttrib const& attr)
    {
        attribs.push_back(attr);
    }
    void setIndexBuffer(ElementBuffer* buf)
    {
        indexBuffer = buf;
    }

    stl_types::Map<u32, VertexBuffer*> buffers;
    stl_types::Vector<VertexAttrib>    attribs;
    ElementBuffer*                     indexBuffer;
};

struct Shader : Coffee::RHI::GraphicsAPI::Shader
{
    Shader(typing::graphics::ShaderStage stage, cstring variant) :
        stage(stage), variant(variant)
    {
    }

    STATICINLINE Shader MatrixTransform()
    {
        return {typing::graphics::ShaderStage::Vertex, "matrix_transform"};
    }

    STATICINLINE Shader VertexColor()
    {
        return {typing::graphics::ShaderStage::Fragment, "vertex_color"};
    }

    typing::graphics::ShaderStage stage;
    cstring                       variant;
};

struct Pipeline : Coffee::RHI::GraphicsAPI::Pipeline
{
    void bind();

    void attach(Shader& shader, typing::graphics::ShaderStage stage)
    {
        shaders[stage] = &shader;
    }

    Shader& storeShader(Shader&& shader)
    {
        stored_shaders.emplace_back(std::make_unique<Shader>(std::move(shader)));
        return *stored_shaders.back();
    }

    stl_types::Vector<std::unique_ptr<Shader>> stored_shaders;

    stl_types::Map<typing::graphics::ShaderStage, Shader*> shaders;
};

struct GXAPI : Coffee::RHI::GraphicsAPI
{
    using API_CONTEXT = stl_types::Function<bool(bool)>;

    struct OPTS
    {
    };

    static gexxo::RenderTarget* DefaultFramebuffer();
    static API_CONTEXT          GetLoadAPI(OPTS const& = {});

    using RBUF = RenderDummy;

    using S_CubeA = SurfaceCubeArray;

    using SM_2D   = Sampler2D;
    using SM_3D   = Sampler3D;
    using SM_Cube = SamplerCube;

    using SM_2DA   = Sampler2DArray;
    using SM_CubeA = SamplerCubeArray;

    using SHD = gexxo::Shader;
    using PIP = gexxo::Pipeline;

    using V_ATTR = gexxo::VertexAttrib;
    using V_DESC = gexxo::VertexDescriptor;

    using BUF_A = gexxo::VertexBuffer;
    using BUF_E = gexxo::ElementBuffer;

    /* Unsupported */
    using BUF_U    = UniformBuffer;
    using BUF_S    = ShaderBuffer;
    using BUF_P    = PixelBuffer;
    using BUF_DRAW = IndirectBuffer;

    using D_CALL = DrawCall;
    using D_DATA = DrawInstanceData;

    /* Unsupported */
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

    static void Draw(
        PIP const&,
        PSTATE const&,
        V_DESC&       desc,
        D_CALL const& call,
        D_DATA const& data);
};

} // namespace gexxo

#undef GEXXO_API
