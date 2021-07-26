#pragma once

#ifdef GL_ARM_texture_unnormalized_coordinates
#include "../enums/GetTextureParameter.h"
#include "../enums/SamplerParameterF.h"
#include "../enums/SamplerParameterI.h"
#include "../enums/TextureParameterName.h"
namespace gl::arm::texture_unnormalized_coordinates {
using gl::group::get_texture_parameter;
using gl::group::sampler_parameter_f;
using gl::group::sampler_parameter_i;
using gl::group::texture_parameter_name;
namespace values {
} // namespace values
} // namespace gl::arm::texture_unnormalized_coordinates
#endif // GL_ARM_texture_unnormalized_coordinates
namespace gl::arm::texture_unnormalized_coordinates {
constexpr auto name = "GL_ARM_texture_unnormalized_coordinates";
}
