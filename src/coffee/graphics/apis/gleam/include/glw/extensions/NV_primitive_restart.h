#pragma once

#ifdef GL_NV_primitive_restart
namespace gl::nv::primitive_restart {
namespace values {
constexpr libc_types::u32 primitive_restart       = 0x8558;
constexpr libc_types::u32 primitive_restart_index = 0x8559;
} // namespace values
STATICINLINE void primitive_restart_index(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveRestartIndexNV)
    }
    glPrimitiveRestartIndexNV(index);
    detail::error_check("PrimitiveRestartIndexNV"sv);
}

STATICINLINE void primitive_restart()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveRestartNV)
    }
    glPrimitiveRestartNV();
    detail::error_check("PrimitiveRestartNV"sv);
}

} // namespace gl::nv::primitive_restart
#endif // GL_NV_primitive_restart
namespace gl::nv::primitive_restart {
constexpr auto name = "GL_NV_primitive_restart";
}
