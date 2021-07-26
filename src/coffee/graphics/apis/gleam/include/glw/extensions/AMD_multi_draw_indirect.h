#pragma once

#ifdef GL_AMD_multi_draw_indirect
namespace gl::amd::multi_draw_indirect {
namespace values {
} // namespace values
STATICINLINE void multi_draw_arrays_indirect(
    group::primitive_type mode, ptroff indirect, i32 primcount, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectAMD)
    }
    glMultiDrawArraysIndirectAMD(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        primcount,
        stride);
    detail::error_check("MultiDrawArraysIndirectAMD"sv);
}

STATICINLINE void multi_draw_elements_indirect(
    group::primitive_type     mode,
    group::draw_elements_type type,
    ptroff                    indirect,
    i32                       primcount,
    i32                       stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectAMD)
    }
    glMultiDrawElementsIndirectAMD(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        primcount,
        stride);
    detail::error_check("MultiDrawElementsIndirectAMD"sv);
}

} // namespace gl::amd::multi_draw_indirect
#endif // GL_AMD_multi_draw_indirect
namespace gl::amd::multi_draw_indirect {
constexpr auto name = "GL_AMD_multi_draw_indirect";
}
