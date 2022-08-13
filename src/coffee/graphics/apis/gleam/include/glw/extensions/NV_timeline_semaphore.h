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
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void create_semaphores(span_u32 semaphores)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateSemaphoresNV)
    }
    glCreateSemaphoresNV(
        semaphores.size(),
        semaphores.size() ? reinterpret_cast<GLuint*>(semaphores.data())
                          : nullptr);
    detail::error_check("CreateSemaphoresNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_semaphore_parameter(
        u32 semaphore, group::semaphore_parameter_name pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSemaphoreParameterivNV)
    }
    glGetSemaphoreParameterivNV(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSemaphoreParameterivNV"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void semaphore_parameter(
        u32                             semaphore,
        group::semaphore_parameter_name pname,
        span_const_i32 const&           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SemaphoreParameterivNV)
    }
    glSemaphoreParameterivNV(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("SemaphoreParameterivNV"sv);
}

} // namespace gl::nv::timeline_semaphore
#endif // GL_NV_timeline_semaphore
namespace gl::nv::timeline_semaphore {
constexpr auto name = "GL_NV_timeline_semaphore";
}