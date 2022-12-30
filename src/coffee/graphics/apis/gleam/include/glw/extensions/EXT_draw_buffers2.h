#pragma once

#ifdef GL_EXT_draw_buffers2
namespace gl::ext::draw_buffers2 {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_EXT_draw_buffers2
 * \param index GLuint
 * \param r GLboolean
 * \param g GLboolean
 * \param b GLboolean
 * \param a GLboolean
 * \return void
 */
STATICINLINE void color_mask_indexed(u32 index, bool r, bool g, bool b, bool a)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMaskIndexedEXT)
    }
    glColorMaskIndexedEXT(index, r, g, b, a);
    detail::error_check("ColorMaskIndexedEXT"sv);
}

/*!
 * \brief Part of GL_EXT_draw_buffers2
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void disable_indexed(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableIndexedEXT)
    }
    glDisableIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("DisableIndexedEXT"sv);
}

/*!
 * \brief Part of GL_EXT_draw_buffers2
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void enable_indexed(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableIndexedEXT)
    }
    glEnableIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("EnableIndexedEXT"sv);
}

template<class span_bool>
requires(
    semantic::concepts::Span<span_bool> &&
    std::is_same_v<
        std::decay_t<typename span_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Part of GL_EXT_draw_buffers2
 * \param target GLenum
 * \param index GLuint
 * \param data GLboolean *
 * \return void
 */
STATICINLINE void get_boolean_indexedv(
    group::buffer_target_arb target, u32 index, span_bool data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBooleanIndexedvEXT)
    }
    glGetBooleanIndexedvEXT(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetBooleanIndexedvEXT"sv);
}

template<class span_i32>
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_draw_buffers2
 * \param target GLenum
 * \param index GLuint
 * \param data GLint *
 * \return void
 */
STATICINLINE
    void get_integer_indexedv(group::get_prop target, u32 index, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegerIndexedvEXT)
    }
    glGetIntegerIndexedvEXT(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegerIndexedvEXT"sv);
}

/*!
 * \brief Part of GL_EXT_draw_buffers2
 * \param target GLenum
 * \param index GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_enabled_indexed(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabledIndexedEXT)
    }
    auto out = glIsEnabledIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("IsEnabledIndexedEXT"sv);
    return out;
}

} // namespace gl::ext::draw_buffers2
#endif // GL_EXT_draw_buffers2
namespace gl::ext::draw_buffers2 {
constexpr auto name = "GL_EXT_draw_buffers2";
}
