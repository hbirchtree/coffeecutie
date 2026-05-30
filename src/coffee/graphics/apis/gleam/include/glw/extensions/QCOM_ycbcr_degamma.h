#pragma once

#ifdef GL_QCOM_ycbcr_degamma
#include "../enums/GetTextureParameter.h"
#include "../enums/TextureParameterName.h"

namespace gl::qcom::ycbcr_degamma {
using gl::group::get_texture_parameter;
using gl::group::texture_parameter_name;

namespace values {
} // namespace values
} // namespace gl::qcom::ycbcr_degamma
#endif // GL_QCOM_ycbcr_degamma
namespace gl::qcom::ycbcr_degamma {
constexpr auto name = "GL_QCOM_ycbcr_degamma";
}
