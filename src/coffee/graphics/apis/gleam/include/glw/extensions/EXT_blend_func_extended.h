#pragma once

#ifdef GL_EXT_blend_func_extended
namespace gl::ext::blend_func_extended {
namespace values {
constexpr libc_types::u32 src_alpha_saturate           = 0x0308;
constexpr libc_types::u32 src1_alpha                   = 0x8589;
constexpr libc_types::u32 src1_color                   = 0x88F9;
constexpr libc_types::u32 one_minus_src1_color         = 0x88FA;
constexpr libc_types::u32 one_minus_src1_alpha         = 0x88FB;
constexpr libc_types::u32 max_dual_source_draw_buffers = 0x88FC;
constexpr libc_types::u32 location_index               = 0x930F;
} // namespace values
STATICINLINE void bind_frag_data_location(
    u32 program, u32 color, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationEXT)
        glIsProgram(program);
    }
    glBindFragDataLocationEXT(program, color, name.data());
    detail::error_check("BindFragDataLocationEXT"sv);
}

STATICINLINE void bind_frag_data_location_indexed(
    u32 program, u32 colorNumber, u32 index, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationIndexedEXT)
        glIsProgram(program);
    }
    glBindFragDataLocationIndexedEXT(program, colorNumber, index, name.data());
    detail::error_check("BindFragDataLocationIndexedEXT"sv);
}

STATICINLINE GLint
get_frag_data_index(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataIndexEXT)
        glIsProgram(program);
    }
    auto out = glGetFragDataIndexEXT(program, name.data());
    detail::error_check("GetFragDataIndexEXT"sv);
    return out;
}

STATICINLINE GLint get_program_resource_location_index(
    u32                      program,
    group::program_interface programInterface,
    std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceLocationIndexEXT)
        glIsProgram(program);
    }
    auto out = glGetProgramResourceLocationIndexEXT(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocationIndexEXT"sv);
    return out;
}

} // namespace gl::ext::blend_func_extended
#endif // GL_EXT_blend_func_extended
namespace gl::ext::blend_func_extended {
constexpr auto name = "GL_EXT_blend_func_extended";
}
