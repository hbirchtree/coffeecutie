#pragma once

#ifdef GL_NV_draw_instanced
namespace gl::nv::draw_instanced {
namespace values {
} // namespace values
STATICINLINE void draw_arrays_instanced(
    group::primitive_type mode, i32 first, i32 count, i32 primcount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstancedNV)
    }
    glDrawArraysInstancedNV(static_cast<GLenum>(mode), first, count, primcount);
    detail::error_check("DrawArraysInstancedNV"sv);
}

STATICINLINE void draw_elements_instanced(
    group::primitive_type mode,
    i32                   count,
    group::primitive_type type,
    ptroff                indices,
    i32                   primcount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedNV)
    }
    glDrawElementsInstancedNV(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        primcount);
    detail::error_check("DrawElementsInstancedNV"sv);
}

} // namespace gl::nv::draw_instanced
#endif // GL_NV_draw_instanced
namespace gl::nv::draw_instanced {
constexpr auto name = "GL_NV_draw_instanced";
}
