#include <coffee/graphics_apis/gleam/rhi/gleam_profile_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_framebuffer_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

static CSize v_size = {1280,720};

static cstring m_shader_vertex_passthrough = {
    "#version 300 es\n"
    "precision lowp float;\n"
    "layout(location=0) in vec3 pos;\n"
    "layout(location=1) in vec2 tex;\n"
    ""
    "out vec2 tex_out;\n"
    "uniform mat4 transform;\n"
    "void main(){\n"
    "    tex_out = tex;\n"
    "    gl_Position = transform*vec4(pos,1.);\n"
    "}\n"
};
static cstring m_shader_fragment_passthrough = {
    "#version 300 es\n"
    "precision lowp float;\n"
    "uniform sampler2D tex;\n"
    "uniform vec2 tex_size;\n"
    "in vec2 tex_out;\n"
    "layout(location=0) out vec4 out_col;\n"
    "void main(){\n"
    "    out_col = texture(tex,tex_out);\n"
    "}\n"
};

static const scalar m_vertex_quad_data[] = {
    -1.f, -1.f, 0.f, 0.f, 0.f,
     1.f, -1.f, 0.f, 1.f, 0.f,
    -1.f,  1.f, 0.f, 0.f, 1.f,

    -1.f,  1.f, 0.f, 0.f, 1.f,
     1.f,  1.f, 0.f, 1.f, 1.f,
     1.f, -1.f, 0.f, 1.f, 0.f,
};

GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t,DBuffers b)
    : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t,b),
      m_size(t.size()),
      m_depth_stencil(PixelFormat::Depth24Stencil8,1),
      m_color(PixelFormat::RGBA8,1)
{
    m_enabled = CGL33::Debug::InternalFormatSupport(Texture::T2D,PixelFormat::Depth24Stencil8);

    if(GL_DEBUG_MODE && !m_enabled)
        cWarning("Cannot enable debugging, unsupported depth-stencil format (DEPTH24_STENCIL8)");

    if(GL_DEBUG_MODE && m_enabled)
    {
        m_debug_target.alloc();

        m_color_sampler.alloc();
        m_depth_stencil_sampler.alloc();

        m_color_sampler.attach(&m_color);
        m_depth_stencil_sampler.attach(&m_depth_stencil);

        resize(m_size);

        m_debug_target.attachSurface(m_color,0,0);
        m_debug_target.attachDepthStencilSurface(m_depth_stencil,0);

        if(!m_debug_target.validate())
        {
            cWarning("Failed to create framebuffer");
        }

        CGhnd shaders[2];

        CGL33::ShaderAlloc(1,ShaderStage::Vertex,&shaders[0]);
        CGL33::ShaderAlloc(1,ShaderStage::Fragment,&shaders[1]);

        CGL33::ShaderSource(shaders[0],1,&m_shader_vertex_passthrough);
        CGL33::ShaderSource(shaders[1],1,&m_shader_fragment_passthrough);

        if(!CGL33::ShaderCompile(shaders[0]) || !CGL33::ShaderCompile(shaders[1]))
        {
            cWarning("Failed to compile passthrough shaders! Fuck this");
            cDebug("Vertex: {0}",CGL33::ShaderGetLog(shaders[0]));
            cDebug("Fragment: {0}",CGL33::ShaderGetLog(shaders[1]));
        }

        CGL33::ProgramAlloc(1,&m_prg);

        CGL33::ShaderAttach(m_prg,shaders[0]);
        CGL33::ShaderAttach(m_prg,shaders[1]);

        if(!CGL33::ProgramLink(m_prg))
        {
            cWarning("Failed to link passthrough program! Fuck this");
            cDebug("Log: {0}",CGL33::ProgramGetLog(m_prg));
        }

        CGL33::ShaderDetach(m_prg,shaders[0]);
        CGL33::ShaderDetach(m_prg,shaders[1]);

        CGL33::ShaderFree(2,shaders);

        CGL33::BufAlloc(1,&m_vbo);
        CGL33::VAOAlloc(1,&m_vao);

        CGL33::BufBind(BufType::ArrayData,m_vbo);
        CGL33::BufData(BufType::ArrayData,sizeof(m_vertex_quad_data),
                       m_vertex_quad_data,ResourceAccess::ReadOnly);

        CGL33::VAOBind(m_vao);
        CGL33::VAOEnableAttrib(0);
        CGL33::VAOEnableAttrib(1);
        CGL33::VAOAttribPointer(0,3,TypeEnum::Scalar,false,sizeof(scalar)*5,0);
        CGL33::VAOAttribPointer(1,2,TypeEnum::Scalar,false,sizeof(scalar)*5,sizeof(scalar)*3);

        CGL33::BufBind(BufType::ArrayData,0);

        m_tex_size_unif = CGL33::ProgramUnifGetLoc(m_prg,"tex_size");
        m_tex_unif = CGL33::ProgramUnifGetLoc(m_prg,"tex");
        m_trans_unif = CGL33::ProgramUnifGetLoc(m_prg,"transform");

    }
}

GLEAM_DBufQuery::~GLEAM_DBufQuery()
{
    if(GL_DEBUG_MODE && m_enabled)
    {
        m_debug_target.dealloc();
        CGL33::ProgramFree(1,&m_prg);
    }
}

void GLEAM_DBufQuery::resize(const CSize &s)
{
    cVerbose(5,"New framebuffer dimensions: {0}",s);
    m_color.allocate(s,PixCmp::RGBA);
    m_depth_stencil.allocate(s,PixCmp::Depth);
}

void GLEAM_DBufQuery::begin()
{
    if(GL_DEBUG_MODE && m_enabled)
    {
        Vecf4 clear_col(1);
        m_debug_target.clear(0,clear_col,1.f);
        m_debug_target.bind(FramebufferT::All);
    }
}

void GLEAM_DBufQuery::end()
{
    if(GL_DEBUG_MODE && m_enabled)
    {
//        CGL33::FBBind(FramebufferT::Read,m_debug_target.m_handle);
//        CGL33::FBBind(FramebufferT::Draw,m_rtarget.m_handle);
//        CGL33::FBBlit({0,0,v_size.w,v_size.h}
//                      ,{0,0,v_size.w/2,v_size.h/2}
//                      ,DBuffers::Color,Filtering::Linear);
//        CGL33::FBBlit({0,0,v_size.w,v_size.h}
//                      ,{v_size.w/2,0,v_size.w/2,v_size.h/2}
//                      ,DBuffers::Depth,Filtering::Nearest);
        m_rtarget.bind(FramebufferT::All);

        /* Frame data */
        constexpr DrwMd md = {Prim::Triangle,PrimCre::Explicit};
        Vecf2 tex_size(v_size.w,v_size.h);
        Matf4 transform = scale(Matf4(),Vecf3(0.5));

        /* Set up rendering state */
        CGL33::VAOBind(m_vao);
        CGL33::BufBind(BufType::ArrayData,m_vbo);
        CGL33::ProgramUse(m_prg);
        CGL33::Disable(Feature::DepthTest);
        CGL33::Uniformfv(m_tex_size_unif,1,&tex_size);
        CGL33::Uniformi(m_tex_unif,0);

        /* Draw depth buffer view */
        m_depth_stencil_sampler.bind(0);
        transform = translation(transform,Vecf3(-1,-1,0));
        CGL33::Uniformfv(m_trans_unif,1,false,&transform);
        CGL33::DrawArrays(md,0,6);

        /* Draw color buffer view */
        m_color_sampler.bind(0);
        transform = translation(transform,Vecf3(2,0,0));
        CGL33::Uniformfv(m_trans_unif,1,false,&transform);
        CGL33::DrawArrays(md,0,6);

        CGL33::VAOBind(0);
        CGL33::BufBind(BufType::ArrayData,0);
    }
}

}
}
}
