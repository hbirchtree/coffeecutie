#pragma once

#ifdef GL_ARB_shading_language_include
namespace gl::arb::shading_language_include {
namespace values {
constexpr libc_types::u32 shader_include      = 0x8DAE;
constexpr libc_types::u32 named_string_length = 0x8DE9;
constexpr libc_types::u32 named_string_type   = 0x8DEA;
} // namespace values
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_shading_language_include
     * \param shader GLuint
     * \param count GLsizei
     * \param path const GLchar *const*
     * \param length const GLint *
     * \return void
     */
    STATICINLINE void compile_shader_include(
        u32                           shader,
        std::vector<std::string_view> path,
        span_const_i32 const&         length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileShaderIncludeARB)
        glIsShader(shader);
    }
    auto [path_lens, path_cstr, path_store] = detail::transform_strings(path);
    glCompileShaderIncludeARB(
        shader,
        path_cstr.size(),
        path_cstr.data(),
        length.size() ? reinterpret_cast<const GLint*>(length.data())
                      : nullptr);
    detail::error_check("CompileShaderIncludeARB"sv);
}

/*!
 * \brief Part of GL_ARB_shading_language_include
 * \param namelen GLint
 * \param name const GLchar *
 * \return void
 */
STATICINLINE void delete_named_string(std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteNamedStringARB)
    }
    glDeleteNamedStringARB(name.size(), name.data());
    detail::error_check("DeleteNamedStringARB"sv);
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>)
    /*!
     * \brief Part of GL_ARB_shading_language_include
     * \param namelen GLint
     * \param name const GLchar *
     * \param bufSize GLsizei
     * \param stringlen GLint *
     * \param string GLchar *
     * \return void
     */
    STATICINLINE void get_named_string(
        std::string_view const& name, i32& stringlen, span_GLchar string)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedStringARB)
    }
    glGetNamedStringARB(
        name.size(), name.data(), string.size(), &stringlen, string.data());
    detail::error_check("GetNamedStringARB"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_shading_language_include
     * \param namelen GLint
     * \param name const GLchar *
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_named_stringiv(
        std::string_view const& name, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedStringivARB)
    }
    glGetNamedStringivARB(
        name.size(),
        name.data(),
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedStringivARB"sv);
}

/*!
 * \brief Part of GL_ARB_shading_language_include
 * \param namelen GLint
 * \param name const GLchar *
 * \return Boolean
 */
STATICINLINE GLboolean is_named_string(std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsNamedStringARB)
    }
    auto out = glIsNamedStringARB(name.size(), name.data());
    detail::error_check("IsNamedStringARB"sv);
    return out;
}

/*!
 * \brief Part of GL_ARB_shading_language_include
 * \param type GLenum
 * \param namelen GLint
 * \param name const GLchar *
 * \param stringlen GLint
 * \param string const GLchar *
 * \return void
 */
STATICINLINE void named_string(
    GLenum type, std::string_view const& name, std::string_view const& string)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedStringARB)
    }
    glNamedStringARB(
        type, name.size(), name.data(), string.size(), string.data());
    detail::error_check("NamedStringARB"sv);
}

} // namespace gl::arb::shading_language_include
#endif // GL_ARB_shading_language_include
namespace gl::arb::shading_language_include {
constexpr auto name = "GL_ARB_shading_language_include";
}
