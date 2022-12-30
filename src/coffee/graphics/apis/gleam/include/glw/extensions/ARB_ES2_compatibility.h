#pragma once

#ifdef GL_ARB_ES2_compatibility
#include "../enums/GetFramebufferParameter.h"
#include "../enums/GetPName.h"
#include "../enums/InternalFormat.h"
#include "../enums/PrecisionType.h"
#include "../enums/SizedInternalFormat.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
namespace gl::arb::es2_compatibility {
using gl::group::get_framebuffer_parameter;
using gl::group::get_prop;
using gl::group::internal_format;
using gl::group::precision_type;
using gl::group::sized_internal_format;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_ES2_compatibility
 * \param d GLfloat
 * \return void
 */
STATICINLINE void clear_depthf(f32 d)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthf)
    }
    glClearDepthf(d);
    detail::error_check("ClearDepthf"sv);
}

/*!
 * \brief Part of GL_ARB_ES2_compatibility
 * \param n GLfloat
 * \param f GLfloat
 * \return void
 */
STATICINLINE void depth_rangef(f32 n, f32 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangef)
    }
    glDepthRangef(n, f);
    detail::error_check("DepthRangef"sv);
}

template<class span_i32>
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_ES2_compatibility
 * \param shadertype GLenum
 * \param precisiontype GLenum
 * \param range GLint *
 * \param precision GLint *
 * \return void
 */
STATICINLINE void get_shader_precision_format(
    group::shader_type    shadertype,
    group::precision_type precisiontype,
    span_i32              range,
    i32&                  precision)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderPrecisionFormat)
    }
    glGetShaderPrecisionFormat(
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(precisiontype),
        range.size() ? reinterpret_cast<GLint*>(range.data()) : nullptr,
        &precision);
    detail::error_check("GetShaderPrecisionFormat"sv);
}

/*!
 * \brief Part of GL_ARB_ES2_compatibility

 * \return void
 */
STATICINLINE void release_shader_compiler()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReleaseShaderCompiler)
    }
    glReleaseShaderCompiler();
    detail::error_check("ReleaseShaderCompiler"sv);
}

template<class span_const_u32, class span_const_void>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_ES2_compatibility
 * \param count GLsizei
 * \param shaders const GLuint *
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLsizei
 * \return void
 */
STATICINLINE void shader_binary(
    span_const_u32 const&       shaders,
    group::shader_binary_format binaryFormat,
    span_const_void const&      binary,
    i32                         length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderBinary)
    }
    glShaderBinary(
        shaders.size(),
        shaders.size() ? reinterpret_cast<const GLuint*>(shaders.data())
                       : nullptr,
        static_cast<GLenum>(binaryFormat),
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ShaderBinary"sv);
}

} // namespace gl::arb::es2_compatibility
#endif // GL_ARB_ES2_compatibility
namespace gl::arb::es2_compatibility {
constexpr auto name = "GL_ARB_ES2_compatibility";
}
