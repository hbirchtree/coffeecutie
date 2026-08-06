#pragma once

#include "caching.h"
#include "selected_version.h"

#include <blam/volta/blam_versions.h>

#include <vector>

extern generation_idx_t g_pose_demo_biped_model;
extern u64              g_pose_demo_biped_entity;

/* The biped's transform as spawned, before any per-frame microphone offset.
 * Kept so the offset is recomputed from a fixed base each frame rather than
 * accumulating into Model::transform. */
extern Matf4 g_pose_demo_biped_base_transform;

/* One entry per successfully spawned entry in pose_config.json's
 * "attachments"; an attachment whose tag or marker is missing on the biped is
 * warned about and left out, so this can be shorter than the config list. */
struct SpawnedAttachment
{
    generation_idx_t model;
    u64              entity;
    /* mod2 node the marker hangs off, and the marker's transform in that
     * node's space. */
    u16   node_idx{0};
    Matf4 marker_local{1.f};
};

extern std::vector<SpawnedAttachment> g_pose_demo_attachments;

extern f32 g_pose_demo_mic_volume;

void spawn_static_biped(
    compo::EntityContainer& e, blam::map_container<halo_version>& container);
void setup_fixed_camera(compo::EntityContainer& e);
