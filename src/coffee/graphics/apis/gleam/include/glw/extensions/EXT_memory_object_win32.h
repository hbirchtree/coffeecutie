#pragma once

#ifdef GL_EXT_memory_object_win32
#include "../enums/ExternalHandleType.h"
#include "../enums/GetPName.h"
#include "../enums/SpecialNumbers.h"

namespace gl::ext::memory_object_win32 {
using gl::group::external_handle_type;
using gl::group::get_prop;
using gl::group::special_numbers;

namespace values {
} // namespace values

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_memory_object_win32
 * \param memory GLuint
 * \param size GLuint64
 * \param handleType GLenum
 * \param handle void *
 * \return void
 */
STATICINLINE void import_memory_win32_handle(
    u32                         memory,
    u64                         size,
    group::external_handle_type handleType,
    span_void                   handle,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ImportMemoryWin32HandleEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glImportMemoryWin32HandleEXT,
        ::glw::trace::byte_span(handle),
        memory,
        size,
        handleType);
#endif
    glImportMemoryWin32HandleEXT(
        memory,
        size,
        static_cast<GLenum>(handleType),
        handle.size() ? reinterpret_cast<void*>(handle.data()) : nullptr);
    detail::error_check("ImportMemoryWin32HandleEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_memory_object_win32
 * \param memory GLuint
 * \param size GLuint64
 * \param handleType GLenum
 * \param name const void *
 * \return void
 */
STATICINLINE void import_memory_win32_name(
    u32                         memory,
    u64                         size,
    group::external_handle_type handleType,
    span_const_void const&      name,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ImportMemoryWin32NameEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glImportMemoryWin32NameEXT,
        ::glw::trace::byte_span(name),
        memory,
        size,
        handleType);
#endif
    glImportMemoryWin32NameEXT(
        memory,
        size,
        static_cast<GLenum>(handleType),
        name.size() ? reinterpret_cast<const void*>(name.data()) : nullptr);
    detail::error_check("ImportMemoryWin32NameEXT"sv, check_errors);
}

} // namespace gl::ext::memory_object_win32
#endif // GL_EXT_memory_object_win32
namespace gl::ext::memory_object_win32 {
constexpr auto name = "GL_EXT_memory_object_win32";
}
