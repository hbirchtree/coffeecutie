#pragma once

#ifdef GL_ARB_provoking_vertex
#include "../enums/GetPName.h"
#include "../enums/VertexProvokingMode.h"
namespace gl::arb::provoking_vertex {
using gl::group::get_prop;
using gl::group::vertex_provoking_mode;
namespace values {
constexpr u32 quads_follow_provoking_vertex_convention = 0x8E4C;
} // namespace values
/*!
 * \brief Part of GL_ARB_provoking_vertex
 * \param mode GLenum
 * \return void
 */
STATICINLINE void provoking_vertex(group::vertex_provoking_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProvokingVertex)
    }
    glProvokingVertex(static_cast<GLenum>(mode));
    detail::error_check("ProvokingVertex"sv);
}

} // namespace gl::arb::provoking_vertex
#endif // GL_ARB_provoking_vertex
namespace gl::arb::provoking_vertex {
constexpr auto name = "GL_ARB_provoking_vertex";
}
