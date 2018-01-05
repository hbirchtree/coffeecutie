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
#if !defined(COFFEE_APPLE)
    "#version 300 es\n"
    "precision lowp float;\n"
#else
    "#version 330\n"
#endif
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
#if !defined(COFFEE_APPLE)
    "#version 300 es\n"
    "precision lowp float;\n"
#else
    "#version 330\n"
#endif
    "uniform sampler2D tex;\n"
    "in vec2 tex_out;\n"
    "layout(location=0) out vec4 OutColor;\n"
    "void main(){\n"
#if !defined(COFFEE_ANDROID) && 0
    "    vec4 comp = texture(tex,tex_out);\n"
    "    out_col.rgb = pow(comp.rgb,vec3(1.0/2.2));\n"
    "    out_col.a = comp.a;\n"
    #else
    "    OutColor = texture(tex,tex_out);\n"
#endif
    "}\n"
};

static const i8 m_vertex_quad_data[] = {
    -127, -127, 0,   0,
     127, -127, 127, 0,
    -127,  127, 0,   127,

     127,  127, 127, 127,
    -127,  127, 0,   127,
     127, -127, 127, 0,
};

void GLEAM_Quad_Drawer::create(u32 pos, u32 tex)
{
    compile_shaders();
    create_vbo_data(pos, tex);
}

void GLEAM_Quad_Drawer::draw(const Matf4 &xf, GLEAM_Sampler2D &sampler)
{
    GLEAM_API::DrawInstanceData di;
    di.m_verts = 6;

    GLEAM_ShaderUniformState m_state_v;
    GLEAM_ShaderUniformState m_state_f;

    Bytes xf_data = Bytes::Create(xf);
    GLEAM_UniformValue m_transform = {};
    m_transform.data = &xf_data;
    m_state_v.setUniform(m_transformLoc, &m_transform);

    auto handle = sampler.handle();
    m_state_f.setSampler(m_texLoc, &handle);

    GLEAM_API::Draw(m_pip, {
                        {ShaderStage::Vertex, &m_state_v},
                        {ShaderStage::Fragment, &m_state_f}
                    }, m_desc, {}, di, nullptr);
}

void GLEAM_Quad_Drawer::cleanup()
{
}

GLEAM_VertDescriptor &GLEAM_Quad_Drawer::vertDesc()
{
    return m_desc;
}

bool GLEAM_Quad_Drawer::compile_shaders()
{
    bool status = true;
    GLEAM_Shader vertex;
    GLEAM_Shader fragment;

    Bytes vertex_src = Bytes::CreateString(m_shader_vertex_passthrough);
    Bytes fragment_src = Bytes::CreateString(m_shader_fragment_passthrough);

    vertex.compile(ShaderStage::Vertex, vertex_src);
    fragment.compile(ShaderStage::Fragment, fragment_src);

    m_pip.attach(vertex, ShaderStage::Vertex);
    m_pip.attach(fragment, ShaderStage::Fragment);

    status = m_pip.assemble();

    vertex.dealloc();
    fragment.dealloc();

    Vector<GLEAM_UniformDescriptor> desc;
    Vector<GLEAM_ProgramParameter> params;
    GLEAM_API::GetShaderUniformState(m_pip, &desc, &params);

    for(auto const& d : desc)
    {
        if(d.m_name == "tex")
            m_texLoc = d;
        if(d.m_name == "transform")
            m_transformLoc = d;
    }

    return status;
}

void GLEAM_Quad_Drawer::create_vbo_data(u32 pos_, u32 tex_)
{
    static GLEAM_ArrayBuffer m_buffer(ResourceAccess::ReadOnly,
                                      sizeof(m_vertex_quad_data));

    m_buffer.alloc();
    m_buffer.commit(sizeof(m_vertex_quad_data), m_vertex_quad_data);

    m_desc.alloc();

    GLEAM_VertAttribute pos = {};
    GLEAM_VertAttribute tex = {};

    pos.m_idx = pos_;
    tex.m_idx = tex_;

    pos.m_bassoc = 0;

    pos.m_size = tex.m_size = 2;

    pos.m_type = tex.m_type = TypeEnum::Byte;

    pos.m_flags = tex.m_flags =
            GLEAM_API::AttributeNormalization
            |GLEAM_API::AttributePacked;

    pos.m_stride = tex.m_stride = sizeof(sbyte_t) * 4;
    tex.m_off = sizeof(sbyte_t) * 2;

    m_desc.addAttribute(pos);
    m_desc.addAttribute(tex);

    m_desc.bindBuffer(0, m_buffer);
}

}
}
}
