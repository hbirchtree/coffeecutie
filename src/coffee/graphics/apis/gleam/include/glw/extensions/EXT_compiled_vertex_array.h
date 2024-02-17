#pragma once

#ifdef GL_EXT_compiled_vertex_array
namespace gl::ext::compiled_vertex_array {
namespace values {
constexpr u32 array_element_lock_first = 0x81A8;
constexpr u32 array_element_lock_count = 0x81A9;
} // namespace values

/*!
 * \brief Part of GL_EXT_compiled_vertex_array
 * \param first GLint
 * \param count GLsizei
 * \return void
 */
STATICINLINE void lock_arrays(i32 first, i32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LockArraysEXT)
    }
    glLockArraysEXT(first, count);
    detail::error_check("LockArraysEXT"sv);
}

/*!
 * \brief Part of GL_EXT_compiled_vertex_array

 * \return void
 */
STATICINLINE void unlock_arrays()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnlockArraysEXT)
    }
    glUnlockArraysEXT();
    detail::error_check("UnlockArraysEXT"sv);
}

} // namespace gl::ext::compiled_vertex_array
#endif // GL_EXT_compiled_vertex_array
namespace gl::ext::compiled_vertex_array {
constexpr auto name = "GL_EXT_compiled_vertex_array";
}
