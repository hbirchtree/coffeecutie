#pragma once

#ifdef GL_NV_depth_buffer_float
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::nv::depth_buffer_float {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 float_32_unsigned_int_24_8_rev = 0x8DAD;
constexpr libc_types::u32 depth_buffer_float_mode        = 0x8DAF;
} // namespace values
STATICINLINE void clear_depthd(f64 depth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthdNV)
    }
    glClearDepthdNV(depth);
    detail::error_check("ClearDepthdNV"sv);
}

STATICINLINE void depth_boundsd(f64 zmin, f64 zmax)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthBoundsdNV)
    }
    glDepthBoundsdNV(zmin, zmax);
    detail::error_check("DepthBoundsdNV"sv);
}

STATICINLINE void depth_ranged(f64 zNear, f64 zFar)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangedNV)
    }
    glDepthRangedNV(zNear, zFar);
    detail::error_check("DepthRangedNV"sv);
}

} // namespace gl::nv::depth_buffer_float
#endif // GL_NV_depth_buffer_float
namespace gl::nv::depth_buffer_float {
constexpr auto name = "GL_NV_depth_buffer_float";
}
