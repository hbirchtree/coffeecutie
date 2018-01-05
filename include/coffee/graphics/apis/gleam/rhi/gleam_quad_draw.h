#pragma once

#include "gleam_types_rhi.h"
#include "gleam_shader_rhi.h"
#include "gleam_vertex_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Quad_Drawer
{
    void create(u32 pos = 0, u32 tex = 1);
    void draw(Matf4 const& xf, GLEAM_Sampler2D& sampler);
    void cleanup();

    GLEAM_VertDescriptor &vertDesc();

protected:
    bool compile_shaders();
    void create_vbo_data(u32 pos = 0, u32 tex = 1);

    GLEAM_Pipeline m_pip;
    GLEAM_UniformDescriptor m_texLoc;
    GLEAM_UniformDescriptor m_transformLoc;
    GLEAM_VertDescriptor m_desc;
};

}
}
}
