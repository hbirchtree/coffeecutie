#pragma once

#ifdef GL_NV_conditional_render
namespace gl::nv::conditional_render {
namespace values {
constexpr libc_types::u32 query_wait              = 0x8E13;
constexpr libc_types::u32 query_no_wait           = 0x8E14;
constexpr libc_types::u32 query_by_region_wait    = 0x8E15;
constexpr libc_types::u32 query_by_region_no_wait = 0x8E16;
} // namespace values
/*!
 * \brief Part of GL_NV_conditional_render
 * \param id GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void begin_conditional_render(
    u32 id, group::conditional_render_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginConditionalRenderNV)
    }
    glBeginConditionalRenderNV(id, static_cast<GLenum>(mode));
    detail::error_check("BeginConditionalRenderNV"sv);
}

/*!
 * \brief Part of GL_NV_conditional_render

 * \return void
 */
STATICINLINE void end_conditional_render()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndConditionalRenderNV)
    }
    glEndConditionalRenderNV();
    detail::error_check("EndConditionalRenderNV"sv);
}

} // namespace gl::nv::conditional_render
#endif // GL_NV_conditional_render
namespace gl::nv::conditional_render {
constexpr auto name = "GL_NV_conditional_render";
}
