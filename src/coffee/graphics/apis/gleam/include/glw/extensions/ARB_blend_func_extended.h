#pragma once

#ifdef GL_ARB_blend_func_extended
#include "../enums/BlendingFactor.h"
#include "../enums/GetPName.h"
#include "../enums/TextureEnvParameter.h"

namespace gl::arb::blend_func_extended {
using gl::group::blending_factor;
using gl::group::get_prop;
using gl::group::texture_env_parameter;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_blend_func_extended
 * \param program GLuint
 * \param colorNumber GLuint
 * \param index GLuint
 * \param name const GLchar *
 * \return void
 */
STATICINLINE void bind_frag_data_location_indexed(
    u32                     program,
    u32                     colorNumber,
    u32                     index,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationIndexed)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glBindFragDataLocationIndexed(program, colorNumber, index, name.data());
    detail::error_check("BindFragDataLocationIndexed"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_blend_func_extended
 * \param program GLuint
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_frag_data_index(
    u32                     program,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataIndex)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetFragDataIndex(program, name.data());
    detail::error_check("GetFragDataIndex"sv, check_errors);
    return out;
}

} // namespace gl::arb::blend_func_extended
#endif // GL_ARB_blend_func_extended
namespace gl::arb::blend_func_extended {
constexpr auto name = "GL_ARB_blend_func_extended";
}
