#pragma once

#include "common.h"

namespace gl::groups {

// MapQuery
enum class map_query : ::libc_types::u32 {
#ifdef GL_COEFF
    coeff = GL_COEFF,
#endif
#ifdef GL_DOMAIN
    domain = GL_DOMAIN,
#endif
#ifdef GL_ORDER
    order = GL_ORDER,
#endif
}; // enum class map_query

} // namespace gl::groups
