#pragma once

#ifdef GL_NV_query_resource_tag
namespace gl::nv::query_resource_tag {
namespace values {
} // namespace values

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_query_resource_tag
 * \param n GLsizei
 * \param tagIds const GLint *
 * \return void
 */
STATICINLINE void delete_query_resource_tag(
    span_const_i32 const& tagIds, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueryResourceTagNV)
    }
    glDeleteQueryResourceTagNV(
        tagIds.size(),
        tagIds.size() ? reinterpret_cast<const GLint*>(tagIds.data())
                      : nullptr);
    detail::error_check("DeleteQueryResourceTagNV"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_query_resource_tag
 * \param n GLsizei
 * \param tagIds GLint *
 * \return void
 */
STATICINLINE void gen_query_resource_tag(
    span_i32 tagIds, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueryResourceTagNV)
    }
    glGenQueryResourceTagNV(
        tagIds.size(),
        tagIds.size() ? reinterpret_cast<GLint*>(tagIds.data()) : nullptr);
    detail::error_check("GenQueryResourceTagNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_query_resource_tag
 * \param tagId GLint
 * \param tagString const GLchar *
 * \return void
 */
STATICINLINE void query_resource_tag(
    i32                     tagId,
    std::string_view const& tagString,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryResourceTagNV)
    }
    glQueryResourceTagNV(tagId, tagString.data());
    detail::error_check("QueryResourceTagNV"sv, check_errors);
}

} // namespace gl::nv::query_resource_tag
#endif // GL_NV_query_resource_tag
namespace gl::nv::query_resource_tag {
constexpr auto name = "GL_NV_query_resource_tag";
}
