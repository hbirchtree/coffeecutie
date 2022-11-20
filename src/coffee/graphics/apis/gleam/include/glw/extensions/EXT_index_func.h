#pragma once

#ifdef GL_EXT_index_func
namespace gl::ext::index_func {
namespace values {
constexpr libc_types::u32 index_test      = 0x81B5;
constexpr libc_types::u32 index_test_func = 0x81B6;
constexpr libc_types::u32 index_test_ref  = 0x81B7;
} // namespace values
/*!
 * \brief Part of GL_EXT_index_func
 * \param func GLenum
 * \param ref GLclampf
 * \return void
 */
STATICINLINE void index_func(group::index_function_ext func, GLclampf ref)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexFuncEXT)
    }
    glIndexFuncEXT(static_cast<GLenum>(func), ref);
    detail::error_check("IndexFuncEXT"sv);
}

} // namespace gl::ext::index_func
#endif // GL_EXT_index_func
namespace gl::ext::index_func {
constexpr auto name = "GL_EXT_index_func";
}
