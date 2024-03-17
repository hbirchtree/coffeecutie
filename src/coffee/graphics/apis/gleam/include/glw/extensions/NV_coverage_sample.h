#pragma once

#ifdef GL_NV_coverage_sample
#include "../enums/ClearBufferMask.h"

namespace gl::nv::coverage_sample {
using gl::group::clear_buffer_mask;

namespace values {
constexpr u32 coverage_component      = 0x8ED0;
constexpr u32 coverage_component4     = 0x8ED1;
constexpr u32 coverage_attachment     = 0x8ED2;
constexpr u32 coverage_buffers        = 0x8ED3;
constexpr u32 coverage_samples        = 0x8ED4;
constexpr u32 coverage_all_fragments  = 0x8ED5;
constexpr u32 coverage_edge_fragments = 0x8ED6;
constexpr u32 coverage_automatic      = 0x8ED7;
} // namespace values

/*!
 * \brief Part of GL_NV_coverage_sample
 * \param mask GLboolean
 * \return void
 */
STATICINLINE void coverage_mask(
    bool mask, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverageMaskNV)
    }
    glCoverageMaskNV(mask);
    detail::error_check("CoverageMaskNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_coverage_sample
 * \param operation GLenum
 * \return void
 */
STATICINLINE void coverage_operation(
    GLenum operation, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverageOperationNV)
    }
    glCoverageOperationNV(operation);
    detail::error_check("CoverageOperationNV"sv, check_errors);
}

} // namespace gl::nv::coverage_sample
#endif // GL_NV_coverage_sample
namespace gl::nv::coverage_sample {
constexpr auto name = "GL_NV_coverage_sample";
}
