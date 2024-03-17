#pragma once

#ifdef GL_ARB_vertex_attrib_binding
#include "../enums/GetPName.h"
#include "../enums/VertexArrayPName.h"
#include "../enums/VertexAttribPropertyARB.h"

namespace gl::arb::vertex_attrib_binding {
using gl::group::get_prop;
using gl::group::vertex_array_prop;
using gl::group::vertex_attrib_property_arb;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_vertex_attrib_binding
 * \param bindingindex GLuint
 * \param buffer GLuint
 * \param offset GLintptr
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void bind_vertex_buffer(
    u32         bindingindex,
    u32         buffer,
    GLintptr    offset,
    i32         stride,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glBindVertexBuffer(bindingindex, buffer, offset, stride);
    detail::error_check("BindVertexBuffer"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_attrib_binding
 * \param attribindex GLuint
 * \param bindingindex GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_binding(
    u32         attribindex,
    u32         bindingindex,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribBinding)
    }
    glVertexAttribBinding(attribindex, bindingindex);
    detail::error_check("VertexAttribBinding"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_attrib_binding
 * \param attribindex GLuint
 * \param size GLint
 * \param type GLenum
 * \param normalized GLboolean
 * \param relativeoffset GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_format(
    u32                       attribindex,
    i32                       size,
    group::vertex_attrib_type type,
    bool                      normalized,
    u32                       relativeoffset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribFormat)
    }
    glVertexAttribFormat(
        attribindex,
        size,
        static_cast<GLenum>(type),
        normalized,
        relativeoffset);
    detail::error_check("VertexAttribFormat"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_attrib_binding
 * \param attribindex GLuint
 * \param size GLint
 * \param type GLenum
 * \param relativeoffset GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_i_format(
    u32                      attribindex,
    i32                      size,
    group::vertex_attrib_int type,
    u32                      relativeoffset,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIFormat)
    }
    glVertexAttribIFormat(
        attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexAttribIFormat"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_attrib_binding
 * \param attribindex GLuint
 * \param size GLint
 * \param type GLenum
 * \param relativeoffset GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_l_format(
    u32                       attribindex,
    i32                       size,
    group::vertex_attrib_long type,
    u32                       relativeoffset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribLFormat)
    }
    glVertexAttribLFormat(
        attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexAttribLFormat"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_vertex_attrib_binding
 * \param bindingindex GLuint
 * \param divisor GLuint
 * \return void
 */
STATICINLINE void vertex_binding_divisor(
    u32 bindingindex, u32 divisor, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexBindingDivisor)
    }
    glVertexBindingDivisor(bindingindex, divisor);
    detail::error_check("VertexBindingDivisor"sv, check_errors);
}

} // namespace gl::arb::vertex_attrib_binding
#endif // GL_ARB_vertex_attrib_binding
namespace gl::arb::vertex_attrib_binding {
constexpr auto name = "GL_ARB_vertex_attrib_binding";
}
