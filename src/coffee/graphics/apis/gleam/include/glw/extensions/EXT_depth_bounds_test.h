#pragma once

#ifdef GL_EXT_depth_bounds_test
namespace gl::ext::depth_bounds_test {
namespace values {
constexpr libc_types::u32 depth_bounds_test = 0x8890;
constexpr libc_types::u32 depth_bounds      = 0x8891;
} // namespace values
STATICINLINE void depth_bounds(GLclampd zmin, GLclampd zmax)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthBoundsEXT)
    }
    glDepthBoundsEXT(zmin, zmax);
    detail::error_check("DepthBoundsEXT"sv);
}

} // namespace gl::ext::depth_bounds_test
#endif // GL_EXT_depth_bounds_test
namespace gl::ext::depth_bounds_test {
constexpr auto name = "GL_EXT_depth_bounds_test";
}
