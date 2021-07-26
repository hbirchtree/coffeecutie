#pragma once

#ifdef GL_AMD_interleaved_elements
#include "../enums/CombinerComponentUsageNV.h"
#include "../enums/CombinerPortionNV.h"
#include "../enums/InternalFormat.h"
#include "../enums/PathColorFormat.h"
#include "../enums/PixelFormat.h"
#include "../enums/SizedInternalFormat.h"
#include "../enums/TextureSwizzle.h"
namespace gl::amd::interleaved_elements {
using gl::group::combiner_component_usage_nv;
using gl::group::combiner_portion_nv;
using gl::group::internal_format;
using gl::group::path_color_format;
using gl::group::pixel_format;
using gl::group::sized_internal_format;
using gl::group::texture_swizzle;
namespace values {
constexpr libc_types::u32 vertex_element_swizzle = 0x91A4;
constexpr libc_types::u32 vertex_id_swizzle      = 0x91A5;
} // namespace values
STATICINLINE void vertex_attrib_parameter(u32 index, GLenum pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribParameteriAMD)
    }
    glVertexAttribParameteriAMD(index, pname, param);
    detail::error_check("VertexAttribParameteriAMD"sv);
}

} // namespace gl::amd::interleaved_elements
#endif // GL_AMD_interleaved_elements
namespace gl::amd::interleaved_elements {
constexpr auto name = "GL_AMD_interleaved_elements";
}
