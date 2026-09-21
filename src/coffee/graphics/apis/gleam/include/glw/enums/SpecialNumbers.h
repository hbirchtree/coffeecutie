#pragma once

#include "common.h"

namespace gl::group {

// SpecialNumbers
enum class special_numbers : u32
{
    false_            = 0,          // GL_FALSE
    invalid_index     = 0xFFFFFFFF, // GL_INVALID_INDEX
    none              = 0,          // GL_NONE
    no_error          = 0,          // GL_NO_ERROR
    one               = 1,          // GL_ONE
    true_             = 1,          // GL_TRUE
    version_es_cl_1_0 = 1,          // GL_VERSION_ES_CL_1_0
    version_es_cl_1_1 = 1,          // GL_VERSION_ES_CL_1_1
    version_es_cm_1_1 = 1,          // GL_VERSION_ES_CM_1_1
    zero              = 0,          // GL_ZERO
    all_pixels_amd    = 0xFFFFFFFF, // GL_ALL_PIXELS_AMD
    luid_size_ext     = 8,          // GL_LUID_SIZE_EXT
    none_oes          = 0,          // GL_NONE_OES
    uuid_size_ext     = 16,         // GL_UUID_SIZE_EXT
}; // enum class special_numbers
constexpr auto special_numbers_timeout_ignored =
    0xFFFFFFFFFFFFFFFFull; // GL_TIMEOUT_IGNORED
constexpr auto special_numbers_timeout_ignored_apple =
    0xFFFFFFFFFFFFFFFFull; // GL_TIMEOUT_IGNORED_APPLE

} // namespace gl::group
