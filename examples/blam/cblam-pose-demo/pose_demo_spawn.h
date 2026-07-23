#pragma once

#include "caching.h"
#include "selected_version.h"

#include <blam/volta/blam_versions.h>

extern generation_idx_t g_pose_demo_biped_model;
extern u64              g_pose_demo_biped_entity;
extern generation_idx_t g_pose_demo_pistol_model;
extern u64              g_pose_demo_pistol_entity;
extern u16              g_pose_demo_hand_node_idx;
extern Matf4            g_pose_demo_hand_marker_local;
extern bool             g_pose_demo_pistol_attached;
extern f32              g_pose_demo_mic_volume;

void spawn_static_biped(
    compo::EntityContainer& e, blam::map_container<halo_version>& container);
void setup_fixed_camera(compo::EntityContainer& e);
