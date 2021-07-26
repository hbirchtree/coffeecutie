#pragma once

#ifdef GL_EXT_multi_draw_indirect
namespace gl::ext::multi_draw_indirect {
namespace values {
} // namespace values
STATICINLINE void multi_draw_arrays_indirect(
    group::primitive_type mode, ptroff indirect, i32 drawcount, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectEXT)
    }
    glMultiDrawArraysIndirectEXT(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawArraysIndirectEXT"sv);
}

STATICINLINE void multi_draw_elements_indirect(
    group::primitive_type     mode,
    group::draw_elements_type type,
    ptroff                    indirect,
    i32                       drawcount,
    i32                       stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectEXT)
    }
    glMultiDrawElementsIndirectEXT(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawElementsIndirectEXT"sv);
}

} // namespace gl::ext::multi_draw_indirect
#endif // GL_EXT_multi_draw_indirect
namespace gl::ext::multi_draw_indirect {
constexpr auto name = "GL_EXT_multi_draw_indirect";
}
