#pragma once

#ifdef GL_NV_command_list
#include "../enums/CommandOpcodesNV.h"

namespace gl::nv::command_list {
using gl::group::command_opcodes_nv;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \return void
 */
STATICINLINE void call_command_list(
    u32 list, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CallCommandListNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCallCommandListNV, list);
#endif
    glCallCommandListNV(list);
    detail::error_check("CallCommandListNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \param segments GLuint
 * \return void
 */
STATICINLINE void command_list_segments(
    u32 list, u32 segments, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CommandListSegmentsNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCommandListSegmentsNV, list, segments);
#endif
    glCommandListSegmentsNV(list, segments);
    detail::error_check("CommandListSegmentsNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \return void
 */
STATICINLINE void compile_command_list(
    u32 list, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileCommandListNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCompileCommandListNV, list);
#endif
    glCompileCommandListNV(list);
    detail::error_check("CompileCommandListNV"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param n GLsizei
 * \param lists GLuint *
 * \return void
 */
STATICINLINE void create_command_lists(
    span_u32 lists, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateCommandListsNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glCreateCommandListsNV,
        gsl::span<char>(
            reinterpret_cast<char*>(lists.data()), lists.size_bytes()));
#endif
    glCreateCommandListsNV(
        lists.size(),
        lists.size() ? reinterpret_cast<GLuint*>(lists.data()) : nullptr);
    detail::error_check("CreateCommandListsNV"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param n GLsizei
 * \param states GLuint *
 * \return void
 */
STATICINLINE void create_states(
    span_u32 states, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateStatesNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glCreateStatesNV,
        gsl::span<char>(
            reinterpret_cast<char*>(states.data()), states.size_bytes()));
#endif
    glCreateStatesNV(
        states.size(),
        states.size() ? reinterpret_cast<GLuint*>(states.data()) : nullptr);
    detail::error_check("CreateStatesNV"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param n GLsizei
 * \param lists const GLuint *
 * \return void
 */
STATICINLINE void delete_command_lists(
    span_const_u32 const& lists, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteCommandListsNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glDeleteCommandListsNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(lists.data()), lists.size_bytes()));
#endif
    glDeleteCommandListsNV(
        lists.size(),
        lists.size() ? reinterpret_cast<const GLuint*>(lists.data()) : nullptr);
    detail::error_check("DeleteCommandListsNV"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param n GLsizei
 * \param states const GLuint *
 * \return void
 */
STATICINLINE void delete_states(
    span_const_u32 const& states, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteStatesNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glDeleteStatesNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(states.data()), states.size_bytes()));
#endif
    glDeleteStatesNV(
        states.size(),
        states.size() ? reinterpret_cast<const GLuint*>(states.data())
                      : nullptr);
    detail::error_check("DeleteStatesNV"sv, check_errors);
}

template<class span_const_i32, class span_const_u64>
requires(
    concepts::span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>> &&
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param primitiveMode GLenum
 * \param indirects const GLuint64 *
 * \param sizes const GLsizei *
 * \param count GLuint
 * \return void
 */
STATICINLINE void draw_commands_address(
    GLenum                primitiveMode,
    span_const_u64 const& indirects,
    span_const_i32 const& sizes,
    u32                   count,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsAddressNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawCommandsAddressNV,
        primitiveMode,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indirects.data()),
            indirects.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(sizes.data()), sizes.size_bytes()),
        count);
#endif
    glDrawCommandsAddressNV(
        primitiveMode,
        indirects.size() ? reinterpret_cast<const GLuint64*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsAddressNV"sv, check_errors);
}

template<class span_const_GLintptr, class span_const_i32>
requires(
    concepts::span<span_const_GLintptr> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLintptr::value_type>,
        std::decay_t<GLintptr>> &&
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param primitiveMode GLenum
 * \param buffer GLuint
 * \param indirects const GLintptr *
 * \param sizes const GLsizei *
 * \param count GLuint
 * \return void
 */
STATICINLINE void draw_commands(
    GLenum                     primitiveMode,
    u32                        buffer,
    span_const_GLintptr const& indirects,
    span_const_i32 const&      sizes,
    u32                        count,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawCommandsNV,
        primitiveMode,
        buffer,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indirects.data()),
            indirects.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(sizes.data()), sizes.size_bytes()),
        count);
#endif
    glDrawCommandsNV(
        primitiveMode,
        buffer,
        indirects.size() ? reinterpret_cast<const GLintptr*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsNV"sv, check_errors);
}

template<class span_const_i32, class span_const_u32, class span_const_u64>
requires(
    concepts::span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>> &&
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param indirects const GLuint64 *
 * \param sizes const GLsizei *
 * \param states const GLuint *
 * \param fbos const GLuint *
 * \param count GLuint
 * \return void
 */
STATICINLINE void draw_commands_states_address(
    span_const_u64 const& indirects,
    span_const_i32 const& sizes,
    span_const_u32 const& states,
    span_const_u32 const& fbos,
    u32                   count,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsStatesAddressNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawCommandsStatesAddressNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indirects.data()),
            indirects.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(sizes.data()), sizes.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(states.data()), states.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(fbos.data()), fbos.size_bytes()),
        count);
#endif
    glDrawCommandsStatesAddressNV(
        indirects.size() ? reinterpret_cast<const GLuint64*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        states.size() ? reinterpret_cast<const GLuint*>(states.data())
                      : nullptr,
        fbos.size() ? reinterpret_cast<const GLuint*>(fbos.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsStatesAddressNV"sv, check_errors);
}

template<class span_const_GLintptr, class span_const_i32, class span_const_u32>
requires(
    concepts::span<span_const_GLintptr> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLintptr::value_type>,
        std::decay_t<GLintptr>> &&
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param buffer GLuint
 * \param indirects const GLintptr *
 * \param sizes const GLsizei *
 * \param states const GLuint *
 * \param fbos const GLuint *
 * \param count GLuint
 * \return void
 */
STATICINLINE void draw_commands_states(
    u32                        buffer,
    span_const_GLintptr const& indirects,
    span_const_i32 const&      sizes,
    span_const_u32 const&      states,
    span_const_u32 const&      fbos,
    u32                        count,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsStatesNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawCommandsStatesNV,
        buffer,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indirects.data()),
            indirects.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(sizes.data()), sizes.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(states.data()), states.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(fbos.data()), fbos.size_bytes()),
        count);
#endif
    glDrawCommandsStatesNV(
        buffer,
        indirects.size() ? reinterpret_cast<const GLintptr*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        states.size() ? reinterpret_cast<const GLuint*>(states.data())
                      : nullptr,
        fbos.size() ? reinterpret_cast<const GLuint*>(fbos.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsStatesNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param tokenID GLenum
 * \param size GLuint
 * \return GLuint
 */
STATICINLINE GLuint get_command_header(
    group::command_opcodes_nv tokenID,
    u32                       size,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCommandHeaderNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetCommandHeaderNV, tokenID, size);
#endif
    auto out = glGetCommandHeaderNV(static_cast<GLenum>(tokenID), size);
    detail::error_check("GetCommandHeaderNV"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_NV_command_list
 * \param shadertype GLenum
 * \return GLushort
 */
STATICINLINE GLushort get_stage_index(
    group::shader_type shadertype, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetStageIndexNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetStageIndexNV, shadertype);
#endif
    auto out = glGetStageIndexNV(static_cast<GLenum>(shadertype));
    detail::error_check("GetStageIndexNV"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_command_list(u32 list, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsCommandListNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsCommandListNV, list);
#endif
    auto out = glIsCommandListNV(list);
    detail::error_check("IsCommandListNV"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_NV_command_list
 * \param state GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_state(u32 state, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsStateNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsStateNV, state);
#endif
    auto out = glIsStateNV(state);
    detail::error_check("IsStateNV"sv, check_errors);
    return out;
}

template<class span_const_i32, class span_const_u32, class span_const_void>
requires(
    concepts::span<span_const_void> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \param segment GLuint
 * \param indirects const void **
 * \param sizes const GLsizei *
 * \param states const GLuint *
 * \param fbos const GLuint *
 * \param count GLuint
 * \return void
 */
STATICINLINE void list_draw_commands_states_client(
    u32                    list,
    u32                    segment,
    span_const_void const& indirects,
    span_const_i32 const&  sizes,
    span_const_u32 const&  states,
    span_const_u32 const&  fbos,
    u32                    count,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ListDrawCommandsStatesClientNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glListDrawCommandsStatesClientNV,
        list,
        segment,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indirects.data()),
            indirects.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(sizes.data()), sizes.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(states.data()), states.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(fbos.data()), fbos.size_bytes()),
        count);
#endif
    glListDrawCommandsStatesClientNV(
        list,
        segment,
        indirects.size() ? reinterpret_cast<const void**>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        states.size() ? reinterpret_cast<const GLuint*>(states.data())
                      : nullptr,
        fbos.size() ? reinterpret_cast<const GLuint*>(fbos.data()) : nullptr,
        count);
    detail::error_check("ListDrawCommandsStatesClientNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param state GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void state_capture(
    u32 state, GLenum mode, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StateCaptureNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glStateCaptureNV, state, mode);
#endif
    glStateCaptureNV(state, mode);
    detail::error_check("StateCaptureNV"sv, check_errors);
}

} // namespace gl::nv::command_list
#endif // GL_NV_command_list
namespace gl::nv::command_list {
constexpr auto name = "GL_NV_command_list";
}
