#pragma once

#ifdef GL_AMD_name_gen_delete
namespace gl::amd::name_gen_delete {
namespace values {
constexpr u32 data_buffer         = 0x9151;
constexpr u32 performance_monitor = 0x9152;
constexpr u32 query_object        = 0x9153;
constexpr u32 vertex_array_object = 0x9154;
constexpr u32 sampler_object      = 0x9155;
} // namespace values

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_AMD_name_gen_delete
 * \param identifier GLenum
 * \param num GLuint
 * \param names const GLuint *
 * \return void
 */
STATICINLINE void delete_names(
    GLenum                identifier,
    span_const_u32 const& names,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteNamesAMD)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteNamesAMD,
        gsl::span<const char>(
            reinterpret_cast<const char*>(names.data()), names.size_bytes()),
        identifier);
#endif
    glDeleteNamesAMD(
        identifier,
        names.size(),
        names.size() ? reinterpret_cast<const GLuint*>(names.data()) : nullptr);
    detail::error_check("DeleteNamesAMD"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_AMD_name_gen_delete
 * \param identifier GLenum
 * \param num GLuint
 * \param names GLuint *
 * \return void
 */
STATICINLINE void gen_names(
    GLenum      identifier,
    span_u32    names,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenNamesAMD)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenNamesAMD,
        gsl::span<char>(
            reinterpret_cast<char*>(names.data()), names.size_bytes()),
        identifier);
#endif
    glGenNamesAMD(
        identifier,
        names.size(),
        names.size() ? reinterpret_cast<GLuint*>(names.data()) : nullptr);
    detail::error_check("GenNamesAMD"sv, check_errors);
}

/*!
 * \brief Part of GL_AMD_name_gen_delete
 * \param identifier GLenum
 * \param name GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_name(GLenum identifier, u32 name, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsNameAMD)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsNameAMD, identifier, name);
#endif
    auto out = glIsNameAMD(identifier, name);
    detail::error_check("IsNameAMD"sv, check_errors);
    return out;
}

} // namespace gl::amd::name_gen_delete
#endif // GL_AMD_name_gen_delete
namespace gl::amd::name_gen_delete {
constexpr auto name = "GL_AMD_name_gen_delete";
}
