#pragma once

#ifdef GL_EXT_debug_label
#include "../enums/BindTransformFeedbackTarget.h"
#include "../enums/ContainerType.h"
#include "../enums/ObjectIdentifier.h"

namespace gl::ext::debug_label {
using gl::group::bind_transform_feedback_target;
using gl::group::container_type;
using gl::group::object_identifier;

namespace values {
constexpr u32 program_pipeline_object = 0x8A4F;
constexpr u32 shader_object           = 0x8B48;
constexpr u32 buffer_object           = 0x9151;
constexpr u32 query_object            = 0x9153;
constexpr u32 vertex_array_object     = 0x9154;
} // namespace values

template<class span_GLchar>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_EXT_debug_label
 * \param type GLenum
 * \param object GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param label GLchar *
 * \return void
 */
STATICINLINE void get_object_label(
    GLenum      type,
    u32         object,
    i32&        length,
    span_GLchar label,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectLabelEXT)
    }
    glGetObjectLabelEXT(type, object, label.size(), &length, label.data());
    detail::error_check("GetObjectLabelEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_debug_label
 * \param type GLenum
 * \param object GLuint
 * \param length GLsizei
 * \param label const GLchar *
 * \return void
 */
STATICINLINE void label_object(
    GLenum                  type,
    u32                     object,
    i32                     length,
    std::string_view const& label,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LabelObjectEXT)
    }
    glLabelObjectEXT(type, object, length, label.data());
    detail::error_check("LabelObjectEXT"sv, check_errors);
}

} // namespace gl::ext::debug_label
#endif // GL_EXT_debug_label
namespace gl::ext::debug_label {
constexpr auto name = "GL_EXT_debug_label";
}
