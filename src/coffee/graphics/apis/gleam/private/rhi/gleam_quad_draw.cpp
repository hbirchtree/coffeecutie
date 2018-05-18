#include <coffee/graphics/apis/gleam/rhi/gleam_quad_draw.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_surface_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_shader_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_buffer_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

using ShaderVersionPair = Pair<APILevel, Pair<cstring, cstring>>;

static ShaderVersionPair quad_shaders[3] = {
    {
        GLES_2_0,
        {
            R"(#version 100
precision lowp int;
precision lowp float;
uniform mat4 transform[64];
uniform int InstanceID;
attribute vec3 pos;
attribute vec2 tex;
varying vec2 fTex;
void main(){fTex = tex; gl_Position = transform[InstanceID] * vec4(pos, 1.0);}
)",
            R"(#version 100
precision lowp float;
uniform sampler2D colorTex;
varying vec2 fTex;
void main(){gl_FragColor = texture2D(colorTex, fTex);}
)"
        }
    },
    {
        GLES_3_0,
        {
            R"(#version 300 es
precision lowp float;
layout(location=0) in vec3 pos;
layout(location=1) in vec2 tex;
out vec2 fTex;
uniform mat4 transform[64];
void main(){fTex = tex; gl_Position = transform[gl_InstanceID] * vec4(pos, 1.0);}
)",
            R"(#version 300 es
uniform sampler2D colorTex;
in vec2 fTex;
out vec4 OutColor;
void main(){OutColor = texture(colorTex, fTex);}
)"
        }
    },
    {
        GL_3_3,
        {
            R"(#version 330
in vec3 pos;
in vec2 tex;
out vec2 fTex;
uniform mat4 transform[64];
void main(){fTex = tex; gl_Position = transform[gl_InstanceID] * vec4(pos, 1.0);}
)",
            R"(#version 330
uniform sampler2D colorTex;
in vec2 fTex;
out vec4 OutColor;
void main(){OutColor = texture(colorTex, fTex);}
)"
        }
    }
};

static cstring find_shader(ShaderStage stage,
                           APILevel version)
{
    ShaderVersionPair* pair = nullptr;

    switch(version)
    {
    case GLES_2_0:
        pair = &quad_shaders[0];
        break;
    default:
    {
        if(APILevelIsOfClass(version, APIClass::GLCore))
        {
            pair = &quad_shaders[2];
        }else
            pair = &quad_shaders[1];
        break;
    }
    }

    if(!pair)
        throw implementation_error("failed to match shader version");

    switch(stage)
    {
    case ShaderStage::Vertex:
        return pair->second.first;
    case ShaderStage::Fragment:
        return pair->second.second;
    default:
        throw implementation_error("chosen shaderstage not supported");
    }
}

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

GLEAM_DrawInstance GLEAM_Quad_Drawer::drawInstance() const
{
    auto inst = GLEAM_DrawInstance{};

    inst.m_verts = 6;

    return inst;
}

GLEAM_DrawCall GLEAM_Quad_Drawer::drawcall() const
{
    return GLEAM_DrawCall(false, false);
}

GLEAM_VertDescriptor &GLEAM_Quad_Drawer::vertDesc()
{
    return m_desc;
}

bool GLEAM_Quad_Drawer::compile_shaders()
{
    GLEAM_API::DBG::SCOPE _(CString(GLM_API) + __FUNCTION__);

    bool status = true;
    GLEAM_Shader vertex;
    GLEAM_Shader fragment;

    Bytes vertex_src = Bytes::CreateString(
                find_shader(ShaderStage::Vertex, GLEAM_API::Level())
                );
    Bytes fragment_src = Bytes::CreateString(
                find_shader(ShaderStage::Fragment, GLEAM_API::Level())
                );

    vertex.compile(ShaderStage::Vertex, vertex_src);
    fragment.compile(ShaderStage::Fragment, fragment_src);

    auto& v_store = m_pip.storeShader(std::move(vertex));
    auto& f_store = m_pip.storeShader(std::move(fragment));

    m_pip.attach(v_store, ShaderStage::Vertex);
    m_pip.attach(f_store, ShaderStage::Fragment);

    status = m_pip.assemble();

    Vector<GLEAM_UniformDescriptor> desc;
    Vector<GLEAM_ProgramParameter> params;
    GLEAM_API::GetShaderUniformState(m_pip, &desc, &params);

    for(auto const& d : desc)
    {
        if(d.m_name == "colortex")
            m_texLoc = d;
        if(d.m_name == "transform")
            m_transformLoc = d;
    }

    return status;
}

void GLEAM_Quad_Drawer::create_vbo_data(u32 pos_, u32 tex_)
{
    GLEAM_API::DBG::SCOPE _(CString(GLM_API) + __FUNCTION__);

    static GLEAM_ArrayBuffer m_buffer(ResourceAccess::WriteOnly,
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
