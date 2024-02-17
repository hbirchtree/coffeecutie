#pragma once

#ifdef GL_EXT_shader_image_load_store
#include "../enums/MemoryBarrierMask.h"

namespace gl::ext::shader_image_load_store {
using gl::group::memory_barrier_mask;

namespace values {
constexpr u32 max_image_units                               = 0x8F38;
constexpr u32 max_combined_image_units_and_fragment_outputs = 0x8F39;
constexpr u32 image_binding_name                            = 0x8F3A;
constexpr u32 image_binding_level                           = 0x8F3B;
constexpr u32 image_binding_layered                         = 0x8F3C;
constexpr u32 image_binding_layer                           = 0x8F3D;
constexpr u32 image_binding_access                          = 0x8F3E;
constexpr u32 image_1d                                      = 0x904C;
constexpr u32 image_2d                                      = 0x904D;
constexpr u32 image_3d                                      = 0x904E;
constexpr u32 image_2d_rect                                 = 0x904F;
constexpr u32 image_cube                                    = 0x9050;
constexpr u32 image_buffer                                  = 0x9051;
constexpr u32 image_1d_array                                = 0x9052;
constexpr u32 image_2d_array                                = 0x9053;
constexpr u32 image_cube_map_array                          = 0x9054;
constexpr u32 image_2d_multisample                          = 0x9055;
constexpr u32 image_2d_multisample_array                    = 0x9056;
constexpr u32 int_image_1d                                  = 0x9057;
constexpr u32 int_image_2d                                  = 0x9058;
constexpr u32 int_image_3d                                  = 0x9059;
constexpr u32 int_image_2d_rect                             = 0x905A;
constexpr u32 int_image_cube                                = 0x905B;
constexpr u32 int_image_buffer                              = 0x905C;
constexpr u32 int_image_1d_array                            = 0x905D;
constexpr u32 int_image_2d_array                            = 0x905E;
constexpr u32 int_image_cube_map_array                      = 0x905F;
constexpr u32 int_image_2d_multisample                      = 0x9060;
constexpr u32 int_image_2d_multisample_array                = 0x9061;
constexpr u32 unsigned_int_image_1d                         = 0x9062;
constexpr u32 unsigned_int_image_2d                         = 0x9063;
constexpr u32 unsigned_int_image_3d                         = 0x9064;
constexpr u32 unsigned_int_image_2d_rect                    = 0x9065;
constexpr u32 unsigned_int_image_cube                       = 0x9066;
constexpr u32 unsigned_int_image_buffer                     = 0x9067;
constexpr u32 unsigned_int_image_1d_array                   = 0x9068;
constexpr u32 unsigned_int_image_2d_array                   = 0x9069;
constexpr u32 unsigned_int_image_cube_map_array             = 0x906A;
constexpr u32 unsigned_int_image_2d_multisample             = 0x906B;
constexpr u32 unsigned_int_image_2d_multisample_array       = 0x906C;
constexpr u32 max_image_samples                             = 0x906D;
constexpr u32 image_binding_format                          = 0x906E;
} // namespace values

/*!
 * \brief Part of GL_EXT_shader_image_load_store
 * \param index GLuint
 * \param texture GLuint
 * \param level GLint
 * \param layered GLboolean
 * \param layer GLint
 * \param access GLenum
 * \param format GLint
 * \return void
 */
STATICINLINE void bind_image_texture(
    u32                      index,
    u32                      texture,
    i32                      level,
    bool                     layered,
    i32                      layer,
    group::buffer_access_arb access,
    i32                      format)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindImageTextureEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindImageTextureEXT(
        index,
        texture,
        level,
        layered,
        layer,
        static_cast<GLenum>(access),
        format);
    detail::error_check("BindImageTextureEXT"sv);
}

/*!
 * \brief Part of GL_EXT_shader_image_load_store
 * \param barriers GLbitfield
 * \return void
 */
STATICINLINE void memory_barrier(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrierEXT)
    }
    glMemoryBarrierEXT(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrierEXT"sv);
}

} // namespace gl::ext::shader_image_load_store
#endif // GL_EXT_shader_image_load_store
namespace gl::ext::shader_image_load_store {
constexpr auto name = "GL_EXT_shader_image_load_store";
}
