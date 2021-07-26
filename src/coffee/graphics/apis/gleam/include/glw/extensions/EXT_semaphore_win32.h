#pragma once

#ifdef GL_EXT_semaphore_win32
#include "../enums/ExternalHandleType.h"
#include "../enums/GetPName.h"
#include "../enums/SemaphoreParameterName.h"
namespace gl::ext::semaphore_win32 {
using gl::group::external_handle_type;
using gl::group::get_prop;
using gl::group::semaphore_parameter_name;
namespace values {
constexpr libc_types::u32 luid_size = 8;
} // namespace values
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void import_semaphore_win32_handle(
        u32 semaphore, group::external_handle_type handleType, span_void handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ImportSemaphoreWin32HandleEXT)
    }
    glImportSemaphoreWin32HandleEXT(
        semaphore,
        static_cast<GLenum>(handleType),
        handle.size() ? reinterpret_cast<void*>(handle.data()) : nullptr);
    detail::error_check("ImportSemaphoreWin32HandleEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void import_semaphore_win32_name(
        u32                         semaphore,
        group::external_handle_type handleType,
        span_const_void const&      name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ImportSemaphoreWin32NameEXT)
    }
    glImportSemaphoreWin32NameEXT(
        semaphore,
        static_cast<GLenum>(handleType),
        name.size() ? reinterpret_cast<const void*>(name.data()) : nullptr);
    detail::error_check("ImportSemaphoreWin32NameEXT"sv);
}

} // namespace gl::ext::semaphore_win32
#endif // GL_EXT_semaphore_win32
namespace gl::ext::semaphore_win32 {
constexpr auto name = "GL_EXT_semaphore_win32";
}
