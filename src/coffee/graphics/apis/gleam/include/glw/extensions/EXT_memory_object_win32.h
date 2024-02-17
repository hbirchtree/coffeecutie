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
    span_void                   handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ImportMemoryWin32HandleEXT)
    }
    glImportMemoryWin32HandleEXT(
        memory,
        size,
        static_cast<GLenum>(handleType),
        handle.size() ? reinterpret_cast<void*>(handle.data()) : nullptr);
    detail::error_check("ImportMemoryWin32HandleEXT"sv);
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
    span_const_void const&      name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ImportMemoryWin32NameEXT)
    }
    glImportMemoryWin32NameEXT(
        memory,
        size,
        static_cast<GLenum>(handleType),
        name.size() ? reinterpret_cast<const void*>(name.data()) : nullptr);
    detail::error_check("ImportMemoryWin32NameEXT"sv);
}

} // namespace gl::ext::memory_object_win32
#endif // GL_EXT_memory_object_win32
namespace gl::ext::memory_object_win32 {
constexpr auto name = "GL_EXT_memory_object_win32";
}
