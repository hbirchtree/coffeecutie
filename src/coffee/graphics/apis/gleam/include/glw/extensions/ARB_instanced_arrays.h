#pragma once

#ifdef GL_ARB_instanced_arrays
namespace gl::arb::instanced_arrays {
namespace values {
constexpr libc_types::u32 vertex_attrib_array_divisor = 0x88FE;
} // namespace values
STATICINLINE void vertex_attrib_divisor(u32 index, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribDivisorARB)
    }
    glVertexAttribDivisorARB(index, divisor);
    detail::error_check("VertexAttribDivisorARB"sv);
}

} // namespace gl::arb::instanced_arrays
#endif // GL_ARB_instanced_arrays
namespace gl::arb::instanced_arrays {
constexpr auto name = "GL_ARB_instanced_arrays";
}
