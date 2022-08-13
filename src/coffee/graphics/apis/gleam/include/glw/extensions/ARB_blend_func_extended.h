#pragma once

#ifdef GL_ARB_blend_func_extended
#include "../enums/BlendingFactor.h"
#include "../enums/GetPName.h"
namespace gl::arb::blend_func_extended {
using gl::group::blending_factor;
using gl::group::get_prop;
namespace values {
} // namespace values
STATICINLINE void bind_frag_data_location_indexed(
    u32 program, u32 colorNumber, u32 index, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationIndexed)
        glIsProgram(program);
    }
    glBindFragDataLocationIndexed(program, colorNumber, index, name.data());
    detail::error_check("BindFragDataLocationIndexed"sv);
}

STATICINLINE GLint
get_frag_data_index(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataIndex)
        glIsProgram(program);
    }
    auto out = glGetFragDataIndex(program, name.data());
    detail::error_check("GetFragDataIndex"sv);
    return out;
}

} // namespace gl::arb::blend_func_extended
#endif // GL_ARB_blend_func_extended
namespace gl::arb::blend_func_extended {
constexpr auto name = "GL_ARB_blend_func_extended";
}