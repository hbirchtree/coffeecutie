#pragma once

#ifdef GL_QCOM_shading_rate
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/ShadingRateQCOM.h"
namespace gl::qcom::shading_rate {
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::shading_rate_qcom;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_QCOM_shading_rate
 * \param rate GLenum
 * \return void
 */
STATICINLINE void shading_rate(group::shading_rate_qcom rate)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShadingRateQCOM)
    }
    glShadingRateQCOM(static_cast<GLenum>(rate));
    detail::error_check("ShadingRateQCOM"sv);
}

} // namespace gl::qcom::shading_rate
#endif // GL_QCOM_shading_rate
namespace gl::qcom::shading_rate {
constexpr auto name = "GL_QCOM_shading_rate";
}
