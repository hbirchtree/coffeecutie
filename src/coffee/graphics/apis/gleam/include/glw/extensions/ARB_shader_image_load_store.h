#pragma once

#ifdef GL_ARB_shader_image_load_store
#include "../enums/AttributeType.h"
#include "../enums/InternalFormatPName.h"
#include "../enums/MemoryBarrierMask.h"
namespace gl::arb::shader_image_load_store {
using gl::group::attribute_type;
using gl::group::internal_format_prop;
using gl::group::memory_barrier_mask;
namespace values {
constexpr libc_types::u32 max_image_units = 0x8F38;
constexpr libc_types::u32 max_combined_image_units_and_fragment_outputs =
    0x8F39;
constexpr libc_types::u32 image_binding_name                  = 0x8F3A;
constexpr libc_types::u32 image_binding_level                 = 0x8F3B;
constexpr libc_types::u32 image_binding_layered               = 0x8F3C;
constexpr libc_types::u32 image_binding_layer                 = 0x8F3D;
constexpr libc_types::u32 image_binding_access                = 0x8F3E;
constexpr libc_types::u32 max_image_samples                   = 0x906D;
constexpr libc_types::u32 image_binding_format                = 0x906E;
constexpr libc_types::u32 image_format_compatibility_by_size  = 0x90C8;
constexpr libc_types::u32 image_format_compatibility_by_class = 0x90C9;
constexpr libc_types::u32 max_vertex_image_uniforms           = 0x90CA;
constexpr libc_types::u32 max_tess_control_image_uniforms     = 0x90CB;
constexpr libc_types::u32 max_tess_evaluation_image_uniforms  = 0x90CC;
constexpr libc_types::u32 max_geometry_image_uniforms         = 0x90CD;
constexpr libc_types::u32 max_fragment_image_uniforms         = 0x90CE;
constexpr libc_types::u32 max_combined_image_uniforms         = 0x90CF;
} // namespace values
/*!
 * \brief Part of GL_ARB_shader_image_load_store
 * \param unit GLuint
 * \param texture GLuint
 * \param level GLint
 * \param layered GLboolean
 * \param layer GLint
 * \param access GLenum
 * \param format GLenum
 * \return void
 */
STATICINLINE void bind_image_texture(
    u32                      unit,
    u32                      texture,
    i32                      level,
    bool                     layered,
    i32                      layer,
    group::buffer_access_arb access,
    group::internal_format   format)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindImageTexture)
        glIsTexture(texture);
    }
    glBindImageTexture(
        unit,
        texture,
        level,
        layered,
        layer,
        static_cast<GLenum>(access),
        static_cast<GLenum>(format));
    detail::error_check("BindImageTexture"sv);
}

/*!
 * \brief Part of GL_ARB_shader_image_load_store
 * \param barriers GLbitfield
 * \return void
 */
STATICINLINE void memory_barrier(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrier)
    }
    glMemoryBarrier(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrier"sv);
}

} // namespace gl::arb::shader_image_load_store
#endif // GL_ARB_shader_image_load_store
namespace gl::arb::shader_image_load_store {
constexpr auto name = "GL_ARB_shader_image_load_store";
}
