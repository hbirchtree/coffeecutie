#pragma once

#ifdef GL_EXT_provoking_vertex
namespace gl::ext::provoking_vertex {
namespace values {
constexpr libc_types::u32 quads_follow_provoking_vertex_convention = 0x8E4C;
constexpr libc_types::u32 first_vertex_convention                  = 0x8E4D;
constexpr libc_types::u32 last_vertex_convention                   = 0x8E4E;
constexpr libc_types::u32 provoking_vertex                         = 0x8E4F;
} // namespace values
/*!
 * \brief Part of GL_EXT_provoking_vertex
 * \param mode GLenum
 * \return void
 */
STATICINLINE void provoking_vertex(group::vertex_provoking_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProvokingVertexEXT)
    }
    glProvokingVertexEXT(static_cast<GLenum>(mode));
    detail::error_check("ProvokingVertexEXT"sv);
}

} // namespace gl::ext::provoking_vertex
#endif // GL_EXT_provoking_vertex
namespace gl::ext::provoking_vertex {
constexpr auto name = "GL_EXT_provoking_vertex";
}
