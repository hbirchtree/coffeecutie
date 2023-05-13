#pragma once

#ifdef GL_NV_texgen_emboss
namespace gl::nv::texgen_emboss {
namespace values {
constexpr u32 emboss_light    = 0x855D;
constexpr u32 emboss_constant = 0x855E;
constexpr u32 emboss_map      = 0x855F;
} // namespace values
} // namespace gl::nv::texgen_emboss
#endif // GL_NV_texgen_emboss
namespace gl::nv::texgen_emboss {
constexpr auto name = "GL_NV_texgen_emboss";
}
