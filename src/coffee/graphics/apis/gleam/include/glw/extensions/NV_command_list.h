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
STATICINLINE void call_command_list(u32 list)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CallCommandListNV)
    }
    glCallCommandListNV(list);
    detail::error_check("CallCommandListNV"sv);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \param segments GLuint
 * \return void
 */
STATICINLINE void command_list_segments(u32 list, u32 segments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CommandListSegmentsNV)
    }
    glCommandListSegmentsNV(list, segments);
    detail::error_check("CommandListSegmentsNV"sv);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \return void
 */
STATICINLINE void compile_command_list(u32 list)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileCommandListNV)
    }
    glCompileCommandListNV(list);
    detail::error_check("CompileCommandListNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_command_list
     * \param n GLsizei
     * \param lists GLuint *
     * \return void
     */
    STATICINLINE void create_command_lists(span_u32 lists)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateCommandListsNV)
    }
    glCreateCommandListsNV(
        lists.size(),
        lists.size() ? reinterpret_cast<GLuint*>(lists.data()) : nullptr);
    detail::error_check("CreateCommandListsNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_command_list
     * \param n GLsizei
     * \param states GLuint *
     * \return void
     */
    STATICINLINE void create_states(span_u32 states)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateStatesNV)
    }
    glCreateStatesNV(
        states.size(),
        states.size() ? reinterpret_cast<GLuint*>(states.data()) : nullptr);
    detail::error_check("CreateStatesNV"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_command_list
     * \param n GLsizei
     * \param lists const GLuint *
     * \return void
     */
    STATICINLINE void delete_command_lists(span_const_u32 const& lists)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteCommandListsNV)
    }
    glDeleteCommandListsNV(
        lists.size(),
        lists.size() ? reinterpret_cast<const GLuint*>(lists.data()) : nullptr);
    detail::error_check("DeleteCommandListsNV"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_command_list
     * \param n GLsizei
     * \param states const GLuint *
     * \return void
     */
    STATICINLINE void delete_states(span_const_u32 const& states)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteStatesNV)
    }
    glDeleteStatesNV(
        states.size(),
        states.size() ? reinterpret_cast<const GLuint*>(states.data())
                      : nullptr);
    detail::error_check("DeleteStatesNV"sv);
}

template<class span_const_i32, class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>&& semantic::concepts::Span<span_const_i32>&&
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
        u32                   count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsAddressNV)
    }
    glDrawCommandsAddressNV(
        primitiveMode,
        indirects.size() ? reinterpret_cast<const GLuint64*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsAddressNV"sv);
}

template<class span_const_GLintptr, class span_const_i32>
requires(semantic::concepts::Span<span_const_GLintptr>&& std::is_same_v<
         std::decay_t<typename span_const_GLintptr::value_type>,
         std::decay_t<GLintptr>>&& semantic::concepts::Span<span_const_i32>&&
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
        u32                        count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsNV)
        glIsBuffer(buffer);
    }
    glDrawCommandsNV(
        primitiveMode,
        buffer,
        indirects.size() ? reinterpret_cast<const GLintptr*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsNV"sv);
}

template<class span_const_i32, class span_const_u32, class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>&&         semantic::concepts::Span<span_const_i32>&&
                                      std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>&& semantic::concepts::Span<span_const_u32>&&
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
        u32                   count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsStatesAddressNV)
    }
    glDrawCommandsStatesAddressNV(
        indirects.size() ? reinterpret_cast<const GLuint64*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        states.size() ? reinterpret_cast<const GLuint*>(states.data())
                      : nullptr,
        fbos.size() ? reinterpret_cast<const GLuint*>(fbos.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsStatesAddressNV"sv);
}

template<class span_const_GLintptr, class span_const_i32, class span_const_u32>
requires(semantic::concepts::Span<span_const_GLintptr>&& std::is_same_v<
         std::decay_t<typename span_const_GLintptr::value_type>,
         std::decay_t<GLintptr>>&&    semantic::concepts::Span<span_const_i32>&&
                                      std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>&& semantic::concepts::Span<span_const_u32>&&
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
        u32                        count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawCommandsStatesNV)
        glIsBuffer(buffer);
    }
    glDrawCommandsStatesNV(
        buffer,
        indirects.size() ? reinterpret_cast<const GLintptr*>(indirects.data())
                         : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizei*>(sizes.data()) : nullptr,
        states.size() ? reinterpret_cast<const GLuint*>(states.data())
                      : nullptr,
        fbos.size() ? reinterpret_cast<const GLuint*>(fbos.data()) : nullptr,
        count);
    detail::error_check("DrawCommandsStatesNV"sv);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param tokenID GLenum
 * \param size GLuint
 * \return GLuint
 */
STATICINLINE GLuint
get_command_header(group::command_opcodes_nv tokenID, u32 size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCommandHeaderNV)
    }
    auto out = glGetCommandHeaderNV(static_cast<GLenum>(tokenID), size);
    detail::error_check("GetCommandHeaderNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_command_list
 * \param shadertype GLenum
 * \return GLushort
 */
STATICINLINE GLushort get_stage_index(group::shader_type shadertype)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetStageIndexNV)
    }
    auto out = glGetStageIndexNV(static_cast<GLenum>(shadertype));
    detail::error_check("GetStageIndexNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_command_list
 * \param list GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_command_list(u32 list)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsCommandListNV)
    }
    auto out = glIsCommandListNV(list);
    detail::error_check("IsCommandListNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_command_list
 * \param state GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_state(u32 state)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsStateNV)
    }
    auto out = glIsStateNV(state);
    detail::error_check("IsStateNV"sv);
    return out;
}

template<class span_const_i32, class span_const_u32, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>&& semantic::concepts::Span<span_const_u32>&&
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
        u32                    count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ListDrawCommandsStatesClientNV)
    }
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
    detail::error_check("ListDrawCommandsStatesClientNV"sv);
}

/*!
 * \brief Part of GL_NV_command_list
 * \param state GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void state_capture(u32 state, GLenum mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StateCaptureNV)
    }
    glStateCaptureNV(state, mode);
    detail::error_check("StateCaptureNV"sv);
}

} // namespace gl::nv::command_list
#endif // GL_NV_command_list
namespace gl::nv::command_list {
constexpr auto name = "GL_NV_command_list";
}
