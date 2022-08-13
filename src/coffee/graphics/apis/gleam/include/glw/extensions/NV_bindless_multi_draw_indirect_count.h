#pragma once

#ifdef GL_NV_bindless_multi_draw_indirect_count
namespace gl::nv::bindless_multi_draw_indirect_count {
namespace values {
} // namespace values
STATICINLINE void multi_draw_arrays_indirect_bindless_count(
    group::primitive_type mode,
    ptroff                indirect,
    i32                   drawCount,
    i32                   maxDrawCount,
    i32                   stride,
    i32                   vertexBufferCount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectBindlessCountNV)
    }
    glMultiDrawArraysIndirectBindlessCountNV(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawCount,
        maxDrawCount,
        stride,
        vertexBufferCount);
    detail::error_check("MultiDrawArraysIndirectBindlessCountNV"sv);
}

STATICINLINE void multi_draw_elements_indirect_bindless_count(
    group::primitive_type     mode,
    group::draw_elements_type type,
    ptroff                    indirect,
    i32                       drawCount,
    i32                       maxDrawCount,
    i32                       stride,
    i32                       vertexBufferCount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectBindlessCountNV)
    }
    glMultiDrawElementsIndirectBindlessCountNV(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawCount,
        maxDrawCount,
        stride,
        vertexBufferCount);
    detail::error_check("MultiDrawElementsIndirectBindlessCountNV"sv);
}

} // namespace gl::nv::bindless_multi_draw_indirect_count
#endif // GL_NV_bindless_multi_draw_indirect_count
namespace gl::nv::bindless_multi_draw_indirect_count {
constexpr auto name = "GL_NV_bindless_multi_draw_indirect_count";
}