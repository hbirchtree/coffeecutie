#pragma once

#ifdef GL_ARB_occlusion_query2
#include "../enums/QueryTarget.h"
namespace gl::arb::occlusion_query2 {
using gl::group::query_target;
namespace values {
} // namespace values
} // namespace gl::arb::occlusion_query2
#endif // GL_ARB_occlusion_query2
namespace gl::arb::occlusion_query2 {
constexpr auto name = "GL_ARB_occlusion_query2";
}
