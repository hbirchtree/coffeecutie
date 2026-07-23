#pragma once

#include "caching.h"
#include "selected_version.h"

#include <blam/volta/blam_antr.h>

#include <chrono>
#include <optional>
#include <peripherals/stl/json.h>

/* Applies a canned pose (JSON array of {"name", "rotation":[x,y,z,w]}) onto
 * model_id's bone_matrices, mirroring ModelCache<V>::apply_animation's DFS
 * parent-chain walk (caching.cpp:663-775) but sourcing rotations from JSON
 * instead of antr frame data. Bones not named in bones_json keep their bind
 * pose. Clears anim_frame_count so tick_animations (caching.cpp:780) doesn't
 * clobber the injected pose on the next frame. */
void apply_pose(
    ModelCache<halo_version>& cache,
    generation_idx_t          model_id,
    nlohmann::json const&     bones_json);

/* Exact-name animation lookup, shared by spawn_static_biped's initial pick
 * and pose_demo_main.cpp's play/loop-animation dummy-plug handlers. */
std::optional<u32> find_animation_by_name(
    blam::antr::header const* antr_hdr,
    blam::map_ptr const&      magic,
    std::string_view          name);

/* Animation playback override state, read by apply_pose's animation-seed
 * step in place of item.anim_idx. Set by pose_demo_main.cpp's
 * "play_animation" (one-shot, plays through once then falls back to the
 * loop) and "loop_animation" (replaces the base loop) dummy-plug events. */
extern std::optional<u32>                    g_pose_demo_oneshot_anim_idx;
extern std::chrono::steady_clock::time_point g_pose_demo_oneshot_start;
extern std::optional<u32>                    g_pose_demo_loop_anim_idx;
