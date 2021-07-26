#pragma once

#ifdef GL_EXT_shadow_samplers
#include "../enums/AttributeType.h"
namespace gl::ext::shadow_samplers {
using gl::group::attribute_type;
namespace values {
constexpr libc_types::u32 texture_compare_mode   = 0x884C;
constexpr libc_types::u32 texture_compare_func   = 0x884D;
constexpr libc_types::u32 compare_ref_to_texture = 0x884E;
} // namespace values
} // namespace gl::ext::shadow_samplers
#endif // GL_EXT_shadow_samplers
namespace gl::ext::shadow_samplers {
constexpr auto name = "GL_EXT_shadow_samplers";
}
