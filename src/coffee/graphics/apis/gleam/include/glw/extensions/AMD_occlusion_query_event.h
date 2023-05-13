#pragma once

#ifdef GL_AMD_occlusion_query_event
#include "../enums/OcclusionQueryEventMaskAMD.h"
namespace gl::amd::occlusion_query_event {
using gl::group::occlusion_query_event_mask_amd;
namespace values {
constexpr u32 occlusion_query_event_mask = 0x874F;
} // namespace values
/*!
 * \brief Part of GL_AMD_occlusion_query_event
 * \param target GLenum
 * \param id GLuint
 * \param pname GLenum
 * \param param GLuint
 * \return void
 */
STATICINLINE void query_object_parameter(
    group::query_target target, u32 id, GLenum pname, u32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryObjectParameteruiAMD)
    }
    glQueryObjectParameteruiAMD(static_cast<GLenum>(target), id, pname, param);
    detail::error_check("QueryObjectParameteruiAMD"sv);
}

} // namespace gl::amd::occlusion_query_event
#endif // GL_AMD_occlusion_query_event
namespace gl::amd::occlusion_query_event {
constexpr auto name = "GL_AMD_occlusion_query_event";
}
