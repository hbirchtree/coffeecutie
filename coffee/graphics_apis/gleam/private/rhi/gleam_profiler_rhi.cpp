#include <coffee/graphics_apis/gleam/rhi/gleam_profile_rhi.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_framebuffer_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_surface_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

QueryT from_term(ProfilingTerm t)
{
    switch(t)
    {
    case ProfilingTerm::ElapsedTime:
        return QueryT::TimeElapsed;
    case ProfilingTerm::ScreenPrimitives:
        return QueryT::PrimGen;
    case ProfilingTerm::ScreenSamples:
        return QueryT::Samples;
    case ProfilingTerm::TransformedPrimitives:
        return QueryT::XFGen;
    default:
        return QueryT::AnySamples;
    }
}

GLEAM_PrfQuery::GLEAM_PrfQuery(ProfilingTerm term):
    PerfQuery(term),
    GLEAM_Query(from_term(term))
{
}

void GLEAM_PrfQuery::begin()
{
    if(m_handle == 0)
        alloc();

    CGL33::QueryBegin(m_type, m_handle);
}

void GLEAM_PrfQuery::end()
{
    CGL33::QueryEnd(m_type);
}

int64 GLEAM_PrfQuery::resulti()
{
#ifdef COFFEE_GLEAM_DESKTOP
    int64 v;
    CGL33::QueryGetObjecti64v(m_handle,GL_QUERY_RESULT,&v);
#else
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle,GL_QUERY_RESULT,&v);
#endif
    return v;
}

uint64 GLEAM_PrfQuery::resultu()
{
#ifdef COFFEE_GLEAM_DESKTOP
    uint64 v;
    CGL33::QueryGetObjectui64v(m_handle,GL_QUERY_RESULT,&v);
#else
    uint32 v;
    CGL33::QueryGetObjectuiv(m_handle,GL_QUERY_RESULT,&v);
#endif
    return v;
}

#ifndef NDEBUG

static CSize v_size = {1280,720};

static cstring m_shader_vertex_passthrough = {
    "#version 300 es\n"
    "precision lowp float;\n"
    "layout(location=0) in vec3 pos;\n"
    "layout(location=1) in vec2 tex;\n"
    ""
    "out vec2 tex_out;\n"
    "uniform mat4 transform;\n"
    ""
    "void main(){\n"
    "    tex_out = tex;\n"
    "    gl_Position = transform*vec4(pos,1.);\n"
    "}\n"
};
static cstring m_shader_fragment_passthrough = {
    "#version 300 es\n"
    "precision lowp float;\n"
    "uniform sampler2D tex;\n"
    "in vec2 tex_out;\n"
    "layout(location=0) out vec4 out_col;\n"
    "void main(){\n"
    #if !defined(COFFEE_ANDROID) && 0
    "    vec4 comp = texture(tex,tex_out);\n"
    "    out_col.rgb = pow(comp.rgb,vec3(1.0/2.2));\n"
    "    out_col.a = comp.a;\n"
    #else
    "    out_col = texture(tex,tex_out);\n"
    #endif
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

#endif

#ifdef NDEBUG
GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t,DBuffers b)
    : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t,b)
{
#else
GLEAM_DBufQuery::GLEAM_DBufQuery(GLEAM_RenderTarget& t,DBuffers b)
    : GraphicsProfiler::BufferQuery<GLEAM_RenderTarget>(t,b),
      m_size(t.size()),
      m_depth_stencil(PixelFormat::Depth24Stencil8,1),
      m_color(PixelFormat::RGBA8,1)
{
    if(!GL_DEBUG_MODE)
    {
        m_enabled = false;
        return;
    }

    if(CGL33::Tex_SRGB_Supported())
    {
        m_color.dealloc();
        new (&m_color) GLEAM_Surface2D(PixelFormat::SRGB8A8);
    }

    if(GL_CURR_API == GL_4_3)
        m_enabled = CGL43::Debug::InternalFormatSupport(Texture::T2D,PixelFormat::Depth24Stencil8);
    else
        m_enabled = CGL33::Debug::InternalFormatSupport(Texture::T2D,PixelFormat::Depth24Stencil8);

    if(!m_enabled && (GL_CURR_API == GLES_3_0 || GL_CURR_API == GLES_3_2))
    {
        m_enabled = true;
        cVerbose(6,"Opting for DEPTH16 depth buffer format");
        m_depth_stencil.dealloc();
        new (&m_depth_stencil) GLEAM_Surface2D(PixelFormat::Depth16,1);
    }

    if(GL_DEBUG_MODE && !m_enabled)
        cWarning("Cannot enable debugging, unsupported depth-stencil format (DEPTH24_STENCIL8)");

    if(GL_DEBUG_MODE && m_enabled)
    {
        m_dtarget.alloc();

        m_color_sampler.alloc();
        m_depth_stencil_sampler.alloc();

        m_color_sampler.attach(&m_color);
        m_depth_stencil_sampler.attach(&m_depth_stencil);

        resize(m_size);

        m_dtarget.attachSurface(m_color,0,0);
        m_dtarget.attachDepthStencilSurface(m_depth_stencil,0);

        if(!m_dtarget.validate())
        {
            cWarning("Failed to create framebuffer");
            m_enabled = false;

            m_dtarget.dealloc();

            m_color_sampler.dealloc();
            m_depth_stencil_sampler.dealloc();

            return;
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
            m_enabled = false;

            CGL33::ShaderFree(2,shaders);

            return;
        }

        CGL33::ProgramAlloc(1,&m_prg);

        CGL33::ShaderAttach(m_prg,shaders[0]);
        CGL33::ShaderAttach(m_prg,shaders[1]);

        if(!CGL33::ProgramLink(m_prg))
        {
            cWarning("Failed to link passthrough program! Fuck this");
            cDebug("Log: {0}",CGL33::ProgramGetLog(m_prg));
            m_enabled = false;

            CGL33::ShaderDetach(m_prg,shaders[0]);
            CGL33::ShaderDetach(m_prg,shaders[1]);

            CGL33::ShaderFree(2,shaders);

            CGL33::ProgramFree(1,&m_prg);

            return;
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
#endif
}

GLEAM_DBufQuery::~GLEAM_DBufQuery()
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
    {
        m_dtarget.dealloc();
        CGL33::ProgramFree(1,&m_prg);
    }
#endif
}

void GLEAM_DBufQuery::resize(const CSize &s)
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
    {
        cVerbose(5,"New framebuffer dimensions: {0}",s);
        m_color.allocate(s,PixCmp::RGBA);
        m_depth_stencil.allocate(s,PixCmp::Depth);
    }
#endif
}

void GLEAM_DBufQuery::begin()
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
        m_dtarget.bind(FramebufferT::All);
#endif
}

void GLEAM_DBufQuery::end()
{
#ifndef NDEBUG
    if(GL_DEBUG_MODE && m_enabled)
    {
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
#endif
}

}
}
}
