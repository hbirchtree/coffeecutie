#pragma once

#ifdef GL_ARB_bindless_texture
#include "../enums/AttributeType.h"
#include "../enums/VertexAttribPointerType.h"
namespace gl::arb::bindless_texture {
using gl::group::attribute_type;
using gl::group::vertex_attrib_pointer_type;
namespace values {
} // namespace values
STATICINLINE GLuint64 get_image_handle(
    u32 texture, i32 level, bool layered, i32 layer, group::pixel_format format)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetImageHandleARB)
        glIsTexture(texture);
    }
    auto out = glGetImageHandleARB(
        texture, level, layered, layer, static_cast<GLenum>(format));
    detail::error_check("GetImageHandleARB"sv);
    return out;
}

STATICINLINE GLuint64 get_texture_handle(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureHandleARB)
        glIsTexture(texture);
    }
    auto out = glGetTextureHandleARB(texture);
    detail::error_check("GetTextureHandleARB"sv);
    return out;
}

STATICINLINE GLuint64 get_texture_sampler_handle(u32 texture, u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureSamplerHandleARB)
        glIsTexture(texture);
        glIsSampler(sampler);
    }
    auto out = glGetTextureSamplerHandleARB(texture, sampler);
    detail::error_check("GetTextureSamplerHandleARB"sv);
    return out;
}

template<class span_GLuint64EXT>
requires(semantic::concepts::Span<span_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>) STATICINLINE
    void get_vertex_attrib_lui64v(
        u32 index, group::vertex_attrib_enum pname, span_GLuint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribLui64vARB)
    }
    glGetVertexAttribLui64vARB(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetVertexAttribLui64vARB"sv);
}

STATICINLINE GLboolean is_image_handle_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsImageHandleResidentARB)
    }
    auto out = glIsImageHandleResidentARB(handle);
    detail::error_check("IsImageHandleResidentARB"sv);
    return out;
}

STATICINLINE GLboolean is_texture_handle_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTextureHandleResidentARB)
    }
    auto out = glIsTextureHandleResidentARB(handle);
    detail::error_check("IsTextureHandleResidentARB"sv);
    return out;
}

STATICINLINE void make_image_handle_non_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeImageHandleNonResidentARB)
    }
    glMakeImageHandleNonResidentARB(handle);
    detail::error_check("MakeImageHandleNonResidentARB"sv);
}

STATICINLINE void make_image_handle_resident(u64 handle, GLenum access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeImageHandleResidentARB)
    }
    glMakeImageHandleResidentARB(handle, access);
    detail::error_check("MakeImageHandleResidentARB"sv);
}

STATICINLINE void make_texture_handle_non_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeTextureHandleNonResidentARB)
    }
    glMakeTextureHandleNonResidentARB(handle);
    detail::error_check("MakeTextureHandleNonResidentARB"sv);
}

STATICINLINE void make_texture_handle_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeTextureHandleResidentARB)
    }
    glMakeTextureHandleResidentARB(handle);
    detail::error_check("MakeTextureHandleResidentARB"sv);
}

STATICINLINE void program_uniform_handle(u32 program, i32 location, u64 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformHandleui64ARB)
        glIsProgram(program);
    }
    glProgramUniformHandleui64ARB(program, location, value);
    detail::error_check("ProgramUniformHandleui64ARB"sv);
}

template<class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void program_uniform_handle(
        u32 program, i32 location, span_const_u64 const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformHandleui64vARB)
        glIsProgram(program);
    }
    glProgramUniformHandleui64vARB(
        program,
        location,
        values.size(),
        values.size() ? reinterpret_cast<const GLuint64*>(values.data())
                      : nullptr);
    detail::error_check("ProgramUniformHandleui64vARB"sv);
}

STATICINLINE void uniform_handle(i32 location, u64 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformHandleui64ARB)
    }
    glUniformHandleui64ARB(location, value);
    detail::error_check("UniformHandleui64ARB"sv);
}

template<class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void uniform_handle(i32 location, span_const_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformHandleui64vARB)
    }
    glUniformHandleui64vARB(
        location,
        value.size(),
        value.size() ? reinterpret_cast<const GLuint64*>(value.data())
                     : nullptr);
    detail::error_check("UniformHandleui64vARB"sv);
}

STATICINLINE void vertex_attrib_l1ui64(u32 index, GLuint64EXT x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1ui64ARB)
    }
    glVertexAttribL1ui64ARB(index, x);
    detail::error_check("VertexAttribL1ui64ARB"sv);
}

template<class span_const_GLuint64EXT>
requires(semantic::concepts::Span<span_const_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_const_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>) STATICINLINE
    void vertex_attrib_l1ui64v(u32 index, span_const_GLuint64EXT const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1ui64vARB)
    }
    glVertexAttribL1ui64vARB(
        index, reinterpret_cast<const GLuint64EXT*>(v.data()));
    detail::error_check("VertexAttribL1ui64vARB"sv);
}

} // namespace gl::arb::bindless_texture
#endif // GL_ARB_bindless_texture
namespace gl::arb::bindless_texture {
constexpr auto name = "GL_ARB_bindless_texture";
}