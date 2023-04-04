#pragma once

#include "common.h"

namespace gl::group {

// SubgroupSupportedFeatures
enum class subgroup_supported_features : u32
{
#ifdef GL_SUBGROUP_FEATURE_ARITHMETIC_BIT_KHR
    subgroup_feature_arithmetic_bit_khr =
        GL_SUBGROUP_FEATURE_ARITHMETIC_BIT_KHR,
#endif
#ifdef GL_SUBGROUP_FEATURE_BALLOT_BIT_KHR
    subgroup_feature_ballot_bit_khr = GL_SUBGROUP_FEATURE_BALLOT_BIT_KHR,
#endif
#ifdef GL_SUBGROUP_FEATURE_BASIC_BIT_KHR
    subgroup_feature_basic_bit_khr = GL_SUBGROUP_FEATURE_BASIC_BIT_KHR,
#endif
#ifdef GL_SUBGROUP_FEATURE_CLUSTERED_BIT_KHR
    subgroup_feature_clustered_bit_khr = GL_SUBGROUP_FEATURE_CLUSTERED_BIT_KHR,
#endif
#ifdef GL_SUBGROUP_FEATURE_PARTITIONED_BIT_NV
    subgroup_feature_partitioned_bit_nv =
        GL_SUBGROUP_FEATURE_PARTITIONED_BIT_NV,
#endif
#ifdef GL_SUBGROUP_FEATURE_QUAD_BIT_KHR
    subgroup_feature_quad_bit_khr = GL_SUBGROUP_FEATURE_QUAD_BIT_KHR,
#endif
#ifdef GL_SUBGROUP_FEATURE_SHUFFLE_BIT_KHR
    subgroup_feature_shuffle_bit_khr = GL_SUBGROUP_FEATURE_SHUFFLE_BIT_KHR,
#endif
#ifdef GL_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT_KHR
    subgroup_feature_shuffle_relative_bit_khr =
        GL_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT_KHR,
#endif
#ifdef GL_SUBGROUP_FEATURE_VOTE_BIT_KHR
    subgroup_feature_vote_bit_khr = GL_SUBGROUP_FEATURE_VOTE_BIT_KHR,
#endif
}; // enum class subgroup_supported_features
C_FLAGS(subgroup_supported_features, ::libc_types::u32);

} // namespace gl::group
