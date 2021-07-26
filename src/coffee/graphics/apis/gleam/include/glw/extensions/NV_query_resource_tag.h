#pragma once

#ifdef GL_NV_query_resource_tag
namespace gl::nv::query_resource_tag {
namespace values {
} // namespace values
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void delete_query_resource_tag(span_const_i32 const& tagIds)
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
    detail::error_check("DeleteQueryResourceTagNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void gen_query_resource_tag(span_i32 tagIds)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueryResourceTagNV)
    }
    glGenQueryResourceTagNV(
        tagIds.size(),
        tagIds.size() ? reinterpret_cast<GLint*>(tagIds.data()) : nullptr);
    detail::error_check("GenQueryResourceTagNV"sv);
}

STATICINLINE void query_resource_tag(
    i32 tagId, std::string_view const& tagString)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryResourceTagNV)
    }
    glQueryResourceTagNV(tagId, tagString.data());
    detail::error_check("QueryResourceTagNV"sv);
}

} // namespace gl::nv::query_resource_tag
#endif // GL_NV_query_resource_tag
namespace gl::nv::query_resource_tag {
constexpr auto name = "GL_NV_query_resource_tag";
}
