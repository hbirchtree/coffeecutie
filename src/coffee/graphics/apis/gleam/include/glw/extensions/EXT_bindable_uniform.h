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
/*!
 * \brief Part of GL_EXT_bindable_uniform
 * \param program GLuint
 * \param location GLint
 * \return GLint
 */
STATICINLINE GLint get_uniform_buffer_size(u32 program, i32 location)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformBufferSizeEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetUniformBufferSizeEXT(program, location);
    detail::error_check("GetUniformBufferSizeEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_bindable_uniform
 * \param program GLuint
 * \param location GLint
 * \return BufferOffset
 */
STATICINLINE GLintptr get_uniform_offset(u32 program, i32 location)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformOffsetEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetUniformOffsetEXT(program, location);
    detail::error_check("GetUniformOffsetEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_bindable_uniform
 * \param program GLuint
 * \param location GLint
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void uniform_buffer(u32 program, i32 location, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformBufferEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glUniformBufferEXT(program, location, buffer);
    detail::error_check("UniformBufferEXT"sv);
}

} // namespace gl::ext::bindable_uniform
#endif // GL_EXT_bindable_uniform
namespace gl::ext::bindable_uniform {
constexpr auto name = "GL_EXT_bindable_uniform";
}
