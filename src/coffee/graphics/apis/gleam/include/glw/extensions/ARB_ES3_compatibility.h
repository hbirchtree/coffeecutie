#pragma once

#ifdef GL_ARB_ES3_compatibility
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/InternalFormat.h"
#include "../enums/QueryTarget.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::arb::es3_compatibility {
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::internal_format;
using gl::group::query_target;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::arb::es3_compatibility
#endif // GL_ARB_ES3_compatibility
namespace gl::arb::es3_compatibility {
constexpr auto name = "GL_ARB_ES3_compatibility";
}
