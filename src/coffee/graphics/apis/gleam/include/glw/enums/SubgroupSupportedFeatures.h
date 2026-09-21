#pragma once

#include "common.h"

namespace gl::group {

// SubgroupSupportedFeatures
enum class subgroup_supported_features : u32
{
    subgroup_feature_arithmetic_bit_khr =
        0x00000004, // GL_SUBGROUP_FEATURE_ARITHMETIC_BIT_KHR
    subgroup_feature_ballot_bit_khr =
        0x00000008, // GL_SUBGROUP_FEATURE_BALLOT_BIT_KHR
    subgroup_feature_basic_bit_khr =
        0x00000001, // GL_SUBGROUP_FEATURE_BASIC_BIT_KHR
    subgroup_feature_clustered_bit_khr =
        0x00000040, // GL_SUBGROUP_FEATURE_CLUSTERED_BIT_KHR
    subgroup_feature_partitioned_bit_nv =
        0x00000100, // GL_SUBGROUP_FEATURE_PARTITIONED_BIT_NV
    subgroup_feature_quad_bit_khr =
        0x00000080, // GL_SUBGROUP_FEATURE_QUAD_BIT_KHR
    subgroup_feature_shuffle_bit_khr =
        0x00000010, // GL_SUBGROUP_FEATURE_SHUFFLE_BIT_KHR
    subgroup_feature_shuffle_relative_bit_khr =
        0x00000020, // GL_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT_KHR
    subgroup_feature_vote_bit_khr =
        0x00000002, // GL_SUBGROUP_FEATURE_VOTE_BIT_KHR
}; // enum class subgroup_supported_features
C_FLAGS(subgroup_supported_features, u32);

} // namespace gl::group
