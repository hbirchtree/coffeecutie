#pragma once

#ifdef GL_ARB_multisample
#include "../enums/AttribMask.h"

namespace gl::arb::multisample {
using gl::group::attrib_mask;

namespace values {
constexpr u32 multisample              = 0x809D;
constexpr u32 sample_alpha_to_coverage = 0x809E;
constexpr u32 sample_alpha_to_one      = 0x809F;
constexpr u32 sample_coverage          = 0x80A0;
constexpr u32 sample_buffers           = 0x80A8;
constexpr u32 samples                  = 0x80A9;
constexpr u32 sample_coverage_value    = 0x80AA;
constexpr u32 sample_coverage_invert   = 0x80AB;
} // namespace values

/*!
 * \brief Part of GL_ARB_multisample
 * \param value GLfloat
 * \param invert GLboolean
 * \return void
 */
STATICINLINE void sample_coverage(f32 value, bool invert)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleCoverageARB)
    }
    glSampleCoverageARB(value, invert);
    detail::error_check("SampleCoverageARB"sv);
}

} // namespace gl::arb::multisample
#endif // GL_ARB_multisample
namespace gl::arb::multisample {
constexpr auto name = "GL_ARB_multisample";
}
