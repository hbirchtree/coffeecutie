#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Quad_Drawer
{
    void create();
    void draw(Matf4 const& xf, GLEAM_Sampler2D& sampler);
    void cleanup();

protected:
    bool compile_shaders();
    void create_vbo_data();
    void set_vao_attributes();

    CGhnd m_vbo;
    CGhnd m_vao;
    CGhnd m_prg;

    int32 m_tex_size_unif;
    int32 m_trans_unif;
    int32 m_tex_unif;
};

}
}
}
