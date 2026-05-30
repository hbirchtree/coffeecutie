#pragma once

#ifdef GL_NV_timeline_semaphore
#include "../enums/GetPName.h"
#include "../enums/SemaphoreParameterName.h"

namespace gl::nv::timeline_semaphore {
using gl::group::get_prop;
using gl::group::semaphore_parameter_name;

namespace values {
} // namespace values

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_timeline_semaphore
 * \param n GLsizei
 * \param semaphores GLuint *
 * \return void
 */
STATICINLINE void create_semaphores(
    span_u32 semaphores, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateSemaphoresNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glCreateSemaphoresNV,
        gsl::span<char>(
            reinterpret_cast<char*>(semaphores.data()),
            semaphores.size_bytes()));
#endif
    glCreateSemaphoresNV(
        semaphores.size(),
        semaphores.size() ? reinterpret_cast<GLuint*>(semaphores.data())
                          : nullptr);
    detail::error_check("CreateSemaphoresNV"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_timeline_semaphore
 * \param semaphore GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_semaphore_parameter(
    u32                             semaphore,
    group::semaphore_parameter_name pname,
    span_i32                        params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSemaphoreParameterivNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetSemaphoreParameterivNV,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        semaphore,
        pname);
#endif
    glGetSemaphoreParameterivNV(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSemaphoreParameterivNV"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_timeline_semaphore
 * \param semaphore GLuint
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void semaphore_parameter(
    u32                             semaphore,
    group::semaphore_parameter_name pname,
    span_const_i32 const&           params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SemaphoreParameterivNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glSemaphoreParameterivNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        semaphore,
        pname);
#endif
    glSemaphoreParameterivNV(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("SemaphoreParameterivNV"sv, check_errors);
}

} // namespace gl::nv::timeline_semaphore
#endif // GL_NV_timeline_semaphore
namespace gl::nv::timeline_semaphore {
constexpr auto name = "GL_NV_timeline_semaphore";
}
