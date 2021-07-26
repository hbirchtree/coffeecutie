#pragma once

#ifdef GL_NVX_conditional_render
namespace gl::nvx::conditional_render {
namespace values {
} // namespace values
STATICINLINE void begin_conditional_render(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginConditionalRenderNVX)
    }
    glBeginConditionalRenderNVX(id);
    detail::error_check("BeginConditionalRenderNVX"sv);
}

STATICINLINE void end_conditional_render()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndConditionalRenderNVX)
    }
    glEndConditionalRenderNVX();
    detail::error_check("EndConditionalRenderNVX"sv);
}

} // namespace gl::nvx::conditional_render
#endif // GL_NVX_conditional_render
namespace gl::nvx::conditional_render {
constexpr auto name = "GL_NVX_conditional_render";
}
