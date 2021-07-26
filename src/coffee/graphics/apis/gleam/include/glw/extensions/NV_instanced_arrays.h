#pragma once

#ifdef GL_NV_instanced_arrays
namespace gl::nv::instanced_arrays {
namespace values {
constexpr libc_types::u32 vertex_attrib_array_divisor = 0x88FE;
} // namespace values
STATICINLINE void vertex_attrib_divisor(u32 index, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribDivisorNV)
    }
    glVertexAttribDivisorNV(index, divisor);
    detail::error_check("VertexAttribDivisorNV"sv);
}

} // namespace gl::nv::instanced_arrays
#endif // GL_NV_instanced_arrays
namespace gl::nv::instanced_arrays {
constexpr auto name = "GL_NV_instanced_arrays";
}
