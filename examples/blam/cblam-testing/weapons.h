#pragma once

#include "components.h"

#include <coffee/components/entity_container.h>

/* Players pulling the trigger: projectiles, firing effects, the first-person
 * fire animation and the weapon's object functions */
void alloc_weapons(compo::EntityContainer& e);

/* Whether a body reaches an item on the ground: it rests against the body,
 * below its middle */
inline bool within_pickup_reach(PlayerInfo const& info, PlayerCamera const& cam, Vecf3 const& item)
{
    constexpr libc_types::f32 reach = .8f;
    Vecf3 const to = item - (cam.camera.position - Vecf3{0.f, 0.f, info.eye_height() * .5f});
    return glm::length(Vecf2(to)) <= reach && std::abs(to.z) <= info.biped.height;
}
