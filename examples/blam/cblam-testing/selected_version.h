#pragma once

#include <blam/volta/blam_versions.h>

#ifndef BLAM_HALO_VERSION
#define BLAM_HALO_VERSION pc_version_t
#endif

using halo_version                    = blam::BLAM_HALO_VERSION;
constexpr halo_version halo_version_v = halo_version();
