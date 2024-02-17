#pragma once

#ifdef GL_EXT_debug_marker
namespace gl::ext::debug_marker {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_EXT_debug_marker
 * \param length GLsizei
 * \param marker const GLchar *
 * \return void
 */
STATICINLINE void insert_event_marker(
    i32 length, std::string_view const& marker)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InsertEventMarkerEXT)
    }
    glInsertEventMarkerEXT(length, marker.data());
    detail::error_check("InsertEventMarkerEXT"sv);
}

/*!
 * \brief Part of GL_EXT_debug_marker

 * \return void
 */
STATICINLINE void pop_group_marker()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PopGroupMarkerEXT)
    }
    glPopGroupMarkerEXT();
    detail::error_check("PopGroupMarkerEXT"sv);
}

/*!
 * \brief Part of GL_EXT_debug_marker
 * \param length GLsizei
 * \param marker const GLchar *
 * \return void
 */
STATICINLINE void push_group_marker(i32 length, std::string_view const& marker)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PushGroupMarkerEXT)
    }
    glPushGroupMarkerEXT(length, marker.data());
    detail::error_check("PushGroupMarkerEXT"sv);
}

} // namespace gl::ext::debug_marker
#endif // GL_EXT_debug_marker
namespace gl::ext::debug_marker {
constexpr auto name = "GL_EXT_debug_marker";
}
