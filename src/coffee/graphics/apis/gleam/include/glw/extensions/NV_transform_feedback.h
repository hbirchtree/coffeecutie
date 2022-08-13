#pragma once

#ifdef GL_NV_transform_feedback
#include "../enums/TransformFeedbackTokenNV.h"
namespace gl::nv::transform_feedback {
using gl::group::transform_feedback_token_nv;
namespace values {
constexpr libc_types::u32 back_primary_color                         = 0x8C77;
constexpr libc_types::u32 back_secondary_color                       = 0x8C78;
constexpr libc_types::u32 texture_coord                              = 0x8C79;
constexpr libc_types::u32 clip_distance                              = 0x8C7A;
constexpr libc_types::u32 vertex_id                                  = 0x8C7B;
constexpr libc_types::u32 primitive_id                               = 0x8C7C;
constexpr libc_types::u32 generic_attrib                             = 0x8C7D;
constexpr libc_types::u32 transform_feedback_attribs                 = 0x8C7E;
constexpr libc_types::u32 transform_feedback_buffer_mode             = 0x8C7F;
constexpr libc_types::u32 max_transform_feedback_separate_components = 0x8C80;
constexpr libc_types::u32 active_varyings                            = 0x8C81;
constexpr libc_types::u32 active_varying_max_length                  = 0x8C82;
constexpr libc_types::u32 transform_feedback_varyings                = 0x8C83;
constexpr libc_types::u32 transform_feedback_buffer_start            = 0x8C84;
constexpr libc_types::u32 transform_feedback_buffer_size             = 0x8C85;
constexpr libc_types::u32 transform_feedback_record                  = 0x8C86;
constexpr libc_types::u32 primitives_generated                       = 0x8C87;
constexpr libc_types::u32 transform_feedback_primitives_written      = 0x8C88;
constexpr libc_types::u32 rasterizer_discard                         = 0x8C89;
constexpr libc_types::u32 max_transform_feedback_interleaved_components =
    0x8C8A;
constexpr libc_types::u32 max_transform_feedback_separate_attribs = 0x8C8B;
constexpr libc_types::u32 interleaved_attribs                     = 0x8C8C;
constexpr libc_types::u32 separate_attribs                        = 0x8C8D;
constexpr libc_types::u32 transform_feedback_buffer               = 0x8C8E;
constexpr libc_types::u32 transform_feedback_buffer_binding       = 0x8C8F;
constexpr libc_types::u32 layer                                   = 0x8DAA;
} // namespace values
STATICINLINE void active_varying(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveVaryingNV)
        glIsProgram(program);
    }
    glActiveVaryingNV(program, name.data());
    detail::error_check("ActiveVaryingNV"sv);
}

STATICINLINE void begin_transform_feedback(group::primitive_type primitiveMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginTransformFeedbackNV)
    }
    glBeginTransformFeedbackNV(static_cast<GLenum>(primitiveMode));
    detail::error_check("BeginTransformFeedbackNV"sv);
}

STATICINLINE void bind_buffer_base(
    group::buffer_target_arb target, u32 index, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferBaseNV)
        glIsBuffer(buffer);
    }
    glBindBufferBaseNV(static_cast<GLenum>(target), index, buffer);
    detail::error_check("BindBufferBaseNV"sv);
}

STATICINLINE void bind_buffer_offset(
    group::buffer_target_arb target, u32 index, u32 buffer, GLintptr offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferOffsetNV)
        glIsBuffer(buffer);
    }
    glBindBufferOffsetNV(static_cast<GLenum>(target), index, buffer, offset);
    detail::error_check("BindBufferOffsetNV"sv);
}

STATICINLINE void bind_buffer_range(
    group::buffer_target_arb target,
    u32                      index,
    u32                      buffer,
    GLintptr                 offset,
    GLsizeiptr               size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferRangeNV)
        glIsBuffer(buffer);
    }
    glBindBufferRangeNV(
        static_cast<GLenum>(target), index, buffer, offset, size);
    detail::error_check("BindBufferRangeNV"sv);
}

STATICINLINE void end_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndTransformFeedbackNV)
    }
    glEndTransformFeedbackNV();
    detail::error_check("EndTransformFeedbackNV"sv);
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>) STATICINLINE
    void get_active_varying(
        u32         program,
        u32         index,
        i32&        length,
        i32&        size,
        GLenum&     type,
        span_GLchar name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveVaryingNV)
        glIsProgram(program);
    }
    glGetActiveVaryingNV(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetActiveVaryingNV"sv);
}

STATICINLINE void get_transform_feedback_varying(
    u32 program, u32 index, i32& location)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbackVaryingNV)
        glIsProgram(program);
    }
    glGetTransformFeedbackVaryingNV(program, index, &location);
    detail::error_check("GetTransformFeedbackVaryingNV"sv);
}

STATICINLINE GLint
get_varying_location(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVaryingLocationNV)
        glIsProgram(program);
    }
    auto out = glGetVaryingLocationNV(program, name.data());
    detail::error_check("GetVaryingLocationNV"sv);
    return out;
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void transform_feedback_attribs(
        i32 count, span_const_i32 const& attribs, GLenum bufferMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackAttribsNV)
    }
    glTransformFeedbackAttribsNV(
        count,
        attribs.size() ? reinterpret_cast<const GLint*>(attribs.data())
                       : nullptr,
        bufferMode);
    detail::error_check("TransformFeedbackAttribsNV"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void transform_feedback_varyings(
        u32 program, span_const_i32 const& locations, GLenum bufferMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackVaryingsNV)
        glIsProgram(program);
    }
    glTransformFeedbackVaryingsNV(
        program,
        locations.size(),
        locations.size() ? reinterpret_cast<const GLint*>(locations.data())
                         : nullptr,
        bufferMode);
    detail::error_check("TransformFeedbackVaryingsNV"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void transform_feedback_stream_attribs(
        span_const_i32 const& attribs,
        span_const_i32 const& bufstreams,
        GLenum                bufferMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackStreamAttribsNV)
    }
    glTransformFeedbackStreamAttribsNV(
        attribs.size(),
        attribs.size() ? reinterpret_cast<const GLint*>(attribs.data())
                       : nullptr,
        bufstreams.size(),
        bufstreams.size() ? reinterpret_cast<const GLint*>(bufstreams.data())
                          : nullptr,
        bufferMode);
    detail::error_check("TransformFeedbackStreamAttribsNV"sv);
}

} // namespace gl::nv::transform_feedback
#endif // GL_NV_transform_feedback
namespace gl::nv::transform_feedback {
constexpr auto name = "GL_NV_transform_feedback";
}