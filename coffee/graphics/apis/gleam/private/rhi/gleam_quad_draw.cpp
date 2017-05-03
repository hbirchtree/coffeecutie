#include <coffee/graphics/apis/gleam/rhi/gleam_quad_draw.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

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

void GLEAM_Quad_Drawer::create()
{
    compile_shaders();
    create_vbo_data();
#if !defined(COFFEE_ONLY_GLES20)
    set_vao_attributes();
#endif
}

void GLEAM_Quad_Drawer::draw(const Matf4 &xf, GLEAM_Sampler2D &sampler)
{
    constexpr DrwMd mode = {Prim::Triangle, PrimCre::Explicit};
/*
    CGL33::BufBind(BufType::ArrayData,m_vbo);
    CGL33::ProgramUse(m_prg);
    CGL33::Disable(Feature::DepthTest);

    CGL33::Uniformi(m_tex_unif,0);

#if !defined(COFFEE_ONLY_GLES20)
    CGL33::VAOBind(m_vao);
#else
    set_vao_attributes();
#endif

    sampler.bind(0);
    CGL33::Uniformfv(m_trans_unif,1,false,&xf);

//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(scalar) * 5, 0);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(scalar) * 5, (c_ptr)(sizeof(scalar) * 3));
    CGL33::VAOEnableAttrib(0);
    CGL33::VAOEnableAttrib(1);
    CGL33::VAOAttribPointer(0,3,TypeEnum::Scalar,false,sizeof(scalar)*5,0);
    CGL33::VAOAttribPointer(1,2,TypeEnum::Scalar,false,sizeof(scalar)*5,sizeof(scalar)*3);

//    glDrawArrays(GL_TRIANGLES, 0, 6);
    CGL33::DrawArrays(mode,0,6);
    */
}

void GLEAM_Quad_Drawer::cleanup()
{
    /*
    CGL33::ProgramFree(1,&m_prg);
    */
}

bool GLEAM_Quad_Drawer::compile_shaders()
{
    GLEAM_Shader vertex;
    GLEAM_Shader fragment;

    Bytes vertex_src = {};
    Bytes fragment_src = {};

    vertex.compile(ShaderStage::Vertex, vertex_src);
    fragment.compile(ShaderStage::Fragment, fragment_src);

    GLEAM_Pipeline m_pip;
//    m_pip.
    /*
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

        CGL33::ShaderFree(2,shaders);

        return false;
    }

    CGL33::ProgramAlloc(1,&m_prg);

    CGL33::ShaderAttach(m_prg,shaders[0]);
    CGL33::ShaderAttach(m_prg,shaders[1]);

    if(!CGL33::ProgramLink(m_prg))
    {
        cWarning("Failed to link passthrough program! Fuck this");
        cDebug("Log: {0}",CGL33::ProgramGetLog(m_prg));

        CGL33::ShaderDetach(m_prg,shaders[0]);
        CGL33::ShaderDetach(m_prg,shaders[1]);

        CGL33::ShaderFree(2,shaders);

        CGL33::ProgramFree(1,&m_prg);

        return false;
    }

    CGL33::ShaderDetach(m_prg,shaders[0]);
    CGL33::ShaderDetach(m_prg,shaders[1]);

    CGL33::ShaderFree(2,shaders);

    m_tex_unif = CGL33::ProgramUnifGetLoc(m_prg,"tex");
    m_trans_unif = CGL33::ProgramUnifGetLoc(m_prg,"transform");

    return true;
    */
}

void GLEAM_Quad_Drawer::create_vbo_data()
{
    /*
    CGL33::BufAlloc(1,&m_vbo);
#if !defined(COFFEE_ONLY_GLES20)
    CGL33::VAOAlloc(1,&m_vao);
#endif

    CGL33::BufBind(BufType::ArrayData,m_vbo);
    CGL33::BufData(BufType::ArrayData,sizeof(m_vertex_quad_data),
                   m_vertex_quad_data,ResourceAccess::ReadOnly);
    CGL33::BufBind(BufType::ArrayData,0);
    */
}

void GLEAM_Quad_Drawer::set_vao_attributes()
{
//    glBindVertexArray(m_vao);
//#if !defined(COFFEE_ONLY_GLES20)
//    CGL33::VAOBind(m_vao);
//#else
//    CGL33::BufBind(BufType::ArrayData,m_vbo);
//#endif
//    CGL33::VAOEnableAttrib(0);
//    CGL33::VAOEnableAttrib(1);
//    CGL33::VAOAttribPointer(0,3,TypeEnum::Scalar,false,sizeof(scalar)*5,0);
//    CGL33::VAOAttribPointer(1,2,TypeEnum::Scalar,false,sizeof(scalar)*5,sizeof(scalar)*3);
}

}
}
}
