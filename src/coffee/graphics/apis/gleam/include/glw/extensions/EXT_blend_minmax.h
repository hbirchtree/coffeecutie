#pragma once

#ifdef GL_EXT_blend_minmax
#include "../enums/BlendEquationModeEXT.h"
#include "../enums/GetPName.h"

namespace gl::ext::blend_minmax {
using gl::group::blend_equation_mode_ext;
using gl::group::get_prop;

namespace values {
} // namespace values
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_EXT_blend_minmax
 * \param mode GLenum
 * \return void
 */
STATICINLINE void blend_equation(
    group::blend_equation_mode_ext mode,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendEquationEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendEquationEXT, mode);
#endif
    glBlendEquationEXT(static_cast<GLenum>(mode));
    detail::error_check("BlendEquationEXT"sv, check_errors);
}

#endif
} // namespace gl::ext::blend_minmax
#endif // GL_EXT_blend_minmax
namespace gl::ext::blend_minmax {
constexpr auto name = "GL_EXT_blend_minmax";
}
