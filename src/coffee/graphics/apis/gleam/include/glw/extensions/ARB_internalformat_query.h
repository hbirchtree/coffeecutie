#pragma once

#ifdef GL_ARB_internalformat_query
#include "../enums/InternalFormatPName.h"

namespace gl::arb::internalformat_query {
using gl::group::internal_format_prop;

namespace values {
} // namespace values

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_internalformat_query
 * \param target GLenum
 * \param internalformat GLenum
 * \param pname GLenum
 * \param count GLsizei
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_internalformativ(
    group::texture_target       target,
    group::internal_format      internalformat,
    group::internal_format_prop pname,
    span_i32                    params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInternalformativ)
    }
    glGetInternalformativ(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(pname),
        params.size(),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetInternalformativ"sv, check_errors);
}

} // namespace gl::arb::internalformat_query
#endif // GL_ARB_internalformat_query
namespace gl::arb::internalformat_query {
constexpr auto name = "GL_ARB_internalformat_query";
}
