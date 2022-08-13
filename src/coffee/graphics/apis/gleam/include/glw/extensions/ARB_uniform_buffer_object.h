#pragma once

#ifdef GL_ARB_uniform_buffer_object
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/GetPName.h"
#include "../enums/ProgramPropertyARB.h"
#include "../enums/SubroutineParameterName.h"
#include "../enums/UniformBlockPName.h"
#include "../enums/UniformPName.h"
namespace gl::arb::uniform_buffer_object {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::get_prop;
using gl::group::program_property_arb;
using gl::group::subroutine_parameter_name;
using gl::group::uniform_block_prop;
using gl::group::uniform_prop;
namespace values {
constexpr libc_types::u32 invalid_index = 0xFFFFFFFF;
} // namespace values
STATICINLINE void bind_buffer_base(
    group::buffer_target_arb target, u32 index, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferBase)
        glIsBuffer(buffer);
    }
    glBindBufferBase(static_cast<GLenum>(target), index, buffer);
    detail::error_check("BindBufferBase"sv);
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
        GLW_FPTR_CHECK(BindBufferRange)
        glIsBuffer(buffer);
    }
    glBindBufferRange(static_cast<GLenum>(target), index, buffer, offset, size);
    detail::error_check("BindBufferRange"sv);
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>) STATICINLINE
    void get_active_uniform_block_name(
        u32         program,
        u32         uniformBlockIndex,
        i32&        length,
        span_GLchar uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockName)
        glIsProgram(program);
    }
    glGetActiveUniformBlockName(
        program,
        uniformBlockIndex,
        uniformBlockName.size(),
        &length,
        uniformBlockName.data());
    detail::error_check("GetActiveUniformBlockName"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_active_uniform_blockiv(
        u32                       program,
        u32                       uniformBlockIndex,
        group::uniform_block_prop pname,
        span_i32                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockiv)
        glIsProgram(program);
    }
    glGetActiveUniformBlockiv(
        program,
        uniformBlockIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformBlockiv"sv);
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>) STATICINLINE
    void get_active_uniform_name(
        u32 program, u32 uniformIndex, i32& length, span_GLchar uniformName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformName)
        glIsProgram(program);
    }
    glGetActiveUniformName(
        program, uniformIndex, uniformName.size(), &length, uniformName.data());
    detail::error_check("GetActiveUniformName"sv);
}

template<class span_const_u32, class span_i32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_i32>&&
                              std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_active_uniformsiv(
        u32                   program,
        span_const_u32 const& uniformIndices,
        group::uniform_prop   pname,
        span_i32              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformsiv)
        glIsProgram(program);
    }
    glGetActiveUniformsiv(
        program,
        uniformIndices.size(),
        uniformIndices.size()
            ? reinterpret_cast<const GLuint*>(uniformIndices.data())
            : nullptr,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformsiv"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_integeri_v(group::get_prop target, u32 index, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegeri_v)
    }
    glGetIntegeri_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegeri_v"sv);
}

STATICINLINE GLuint
get_uniform_block_index(u32 program, std::string_view const& uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformBlockIndex)
        glIsProgram(program);
    }
    auto out = glGetUniformBlockIndex(program, uniformBlockName.data());
    detail::error_check("GetUniformBlockIndex"sv);
    return out;
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void get_uniform_indices(
        u32                           program,
        i32                           uniformCount,
        std::vector<std::string_view> uniformNames,
        span_u32                      uniformIndices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformIndices)
        glIsProgram(program);
    }
    auto [uniformNames_lens, uniformNames_cstr, uniformNames_store] =
        detail::transform_strings(uniformNames);
    glGetUniformIndices(
        program,
        uniformCount,
        uniformNames_cstr.data(),
        uniformIndices.size() ? reinterpret_cast<GLuint*>(uniformIndices.data())
                              : nullptr);
    detail::error_check("GetUniformIndices"sv);
}

STATICINLINE void uniform_block_binding(
    u32 program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformBlockBinding)
        glIsProgram(program);
    }
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    detail::error_check("UniformBlockBinding"sv);
}

} // namespace gl::arb::uniform_buffer_object
#endif // GL_ARB_uniform_buffer_object
namespace gl::arb::uniform_buffer_object {
constexpr auto name = "GL_ARB_uniform_buffer_object";
}