#pragma once

#ifdef GL_EXT_bindable_uniform
namespace gl::ext::bindable_uniform {
namespace values {
constexpr libc_types::u32 max_vertex_bindable_uniforms   = 0x8DE2;
constexpr libc_types::u32 max_fragment_bindable_uniforms = 0x8DE3;
constexpr libc_types::u32 max_geometry_bindable_uniforms = 0x8DE4;
constexpr libc_types::u32 max_bindable_uniform_size      = 0x8DED;
constexpr libc_types::u32 uniform_buffer                 = 0x8DEE;
constexpr libc_types::u32 uniform_buffer_binding         = 0x8DEF;
} // namespace values
STATICINLINE GLint get_uniform_buffer_size(u32 program, i32 location)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformBufferSizeEXT)
        glIsProgram(program);
    }
    auto out = glGetUniformBufferSizeEXT(program, location);
    detail::error_check("GetUniformBufferSizeEXT"sv);
    return out;
}

STATICINLINE GLintptr get_uniform_offset(u32 program, i32 location)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformOffsetEXT)
        glIsProgram(program);
    }
    auto out = glGetUniformOffsetEXT(program, location);
    detail::error_check("GetUniformOffsetEXT"sv);
    return out;
}

STATICINLINE void uniform_buffer(u32 program, i32 location, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformBufferEXT)
        glIsProgram(program);
        glIsBuffer(buffer);
    }
    glUniformBufferEXT(program, location, buffer);
    detail::error_check("UniformBufferEXT"sv);
}

} // namespace gl::ext::bindable_uniform
#endif // GL_EXT_bindable_uniform
namespace gl::ext::bindable_uniform {
constexpr auto name = "GL_EXT_bindable_uniform";
}