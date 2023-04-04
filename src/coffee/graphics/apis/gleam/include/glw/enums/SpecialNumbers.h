#pragma once

#include "common.h"

namespace gl::group {

// SpecialNumbers
enum class special_numbers : u32
{
#ifdef GL_ALL_PIXELS_AMD
    all_pixels_amd = GL_ALL_PIXELS_AMD,
#endif
#ifdef GL_FALSE
    false_ = GL_FALSE,
#endif
#ifdef GL_INVALID_INDEX
    invalid_index = GL_INVALID_INDEX,
#endif
#ifdef GL_LUID_SIZE_EXT
    luid_size_ext = GL_LUID_SIZE_EXT,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_NONE_OES
    none_oes = GL_NONE_OES,
#endif
#ifdef GL_NO_ERROR
    no_error = GL_NO_ERROR,
#endif
#ifdef GL_ONE
    one = GL_ONE,
#endif
#ifdef GL_TRUE
    true_ = GL_TRUE,
#endif
#ifdef GL_UUID_SIZE_EXT
    uuid_size_ext = GL_UUID_SIZE_EXT,
#endif
#ifdef GL_VERSION_ES_CL_1_0
    version_es_cl_1_0 = GL_VERSION_ES_CL_1_0,
#endif
#ifdef GL_VERSION_ES_CL_1_1
    version_es_cl_1_1 = GL_VERSION_ES_CL_1_1,
#endif
#ifdef GL_VERSION_ES_CM_1_1
    version_es_cm_1_1 = GL_VERSION_ES_CM_1_1,
#endif
#ifdef GL_ZERO
    zero = GL_ZERO,
#endif
}; // enum class special_numbers
#ifdef GL_TIMEOUT_IGNORED
constexpr auto special_numbers_timeout_ignored = GL_TIMEOUT_IGNORED;
#endif
#ifdef GL_TIMEOUT_IGNORED_APPLE
constexpr auto special_numbers_timeout_ignored_apple = GL_TIMEOUT_IGNORED_APPLE;
#endif

} // namespace gl::group
