#pragma once

#ifdef GL_EXT_multisample
#include "../enums/AttribMask.h"
#include "../enums/SamplePatternEXT.h"
#include "../enums/SamplePatternSGIS.h"
namespace gl::ext::multisample {
using gl::group::attrib_mask;
using gl::group::sample_pattern_ext;
using gl::group::sample_pattern_sgis;
namespace values {
constexpr libc_types::u32 multisample          = 0x809D;
constexpr libc_types::u32 sample_alpha_to_mask = 0x809E;
constexpr libc_types::u32 sample_alpha_to_one  = 0x809F;
constexpr libc_types::u32 sample_mask          = 0x80A0;
constexpr libc_types::u32 sample_buffers       = 0x80A8;
constexpr libc_types::u32 samples              = 0x80A9;
constexpr libc_types::u32 sample_mask_value    = 0x80AA;
constexpr libc_types::u32 sample_mask_invert   = 0x80AB;
constexpr libc_types::u32 sample_pattern       = 0x80AC;
} // namespace values
/*!
 * \brief Part of GL_EXT_multisample
 * \param value GLclampf
 * \param invert GLboolean
 * \return void
 */
STATICINLINE void sample_mask(GLclampf value, bool invert)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleMaskEXT)
    }
    glSampleMaskEXT(value, invert);
    detail::error_check("SampleMaskEXT"sv);
}

/*!
 * \brief Part of GL_EXT_multisample
 * \param pattern GLenum
 * \return void
 */
STATICINLINE void sample_pattern(group::sample_pattern_ext pattern)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplePatternEXT)
    }
    glSamplePatternEXT(static_cast<GLenum>(pattern));
    detail::error_check("SamplePatternEXT"sv);
}

} // namespace gl::ext::multisample
#endif // GL_EXT_multisample
namespace gl::ext::multisample {
constexpr auto name = "GL_EXT_multisample";
}
