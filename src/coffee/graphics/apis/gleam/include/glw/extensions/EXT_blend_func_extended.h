#pragma once

#ifdef GL_EXT_blend_func_extended
#include "../enums/TextureEnvParameter.h"

namespace gl::ext::blend_func_extended {
using gl::group::texture_env_parameter;

namespace values {
constexpr u32 src_alpha_saturate           = 0x0308;
constexpr u32 src1_color                   = 0x88F9;
constexpr u32 one_minus_src1_color         = 0x88FA;
constexpr u32 one_minus_src1_alpha         = 0x88FB;
constexpr u32 max_dual_source_draw_buffers = 0x88FC;
constexpr u32 location_index               = 0x930F;
} // namespace values

/*!
 * \brief Part of GL_EXT_blend_func_extended
 * \param program GLuint
 * \param color GLuint
 * \param name const GLchar *
 * \return void
 */
STATICINLINE void bind_frag_data_location(
    u32 program, u32 color, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glBindFragDataLocationEXT(program, color, name.data());
    detail::error_check("BindFragDataLocationEXT"sv);
}

/*!
 * \brief Part of GL_EXT_blend_func_extended
 * \param program GLuint
 * \param colorNumber GLuint
 * \param index GLuint
 * \param name const GLchar *
 * \return void
 */
STATICINLINE void bind_frag_data_location_indexed(
    u32 program, u32 colorNumber, u32 index, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationIndexedEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glBindFragDataLocationIndexedEXT(program, colorNumber, index, name.data());
    detail::error_check("BindFragDataLocationIndexedEXT"sv);
}

/*!
 * \brief Part of GL_EXT_blend_func_extended
 * \param program GLuint
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint
get_frag_data_index(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataIndexEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetFragDataIndexEXT(program, name.data());
    detail::error_check("GetFragDataIndexEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_blend_func_extended
 * \param program GLuint
 * \param programInterface GLenum
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_program_resource_location_index(
    u32                      program,
    group::program_interface programInterface,
    std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceLocationIndexEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetProgramResourceLocationIndexEXT(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocationIndexEXT"sv);
    return out;
}

} // namespace gl::ext::blend_func_extended
#endif // GL_EXT_blend_func_extended
namespace gl::ext::blend_func_extended {
constexpr auto name = "GL_EXT_blend_func_extended";
}
