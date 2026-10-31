#pragma once

#include "components.h"

#include <blam/volta/blam_unit.h>
#include <coffee/components/entity_container.h>

#include <algorithm>

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

/* Rounds an ammo pack adds to a carried weapon's reserve: what its magazine
 * objects give for it, up to the reserve's maximum */
inline libc_types::i32 ammo_pack_rounds(
    BipedEquipment::weapon_t const& carried, blam::tag_index<> pack, blam::map_ptr const& magic)
{
    if(!carried.weapon)
        return 0;
    auto triggers  = carried.weapon->triggers.data(magic);
    auto magazines = carried.weapon->magazines.data(magic);
    if(!triggers.has_value() || triggers.value().empty() || !magazines.has_value())
        return 0;
    auto const i = triggers.value()[0].magazine;
    if(i < 0 || static_cast<size_t>(i) >= magazines.value().size())
        return 0;
    auto const& magazine = magazines.value()[i];
    auto        objects  = magazine.magazine_objects.data(magic);
    if(!objects.has_value())
        return 0;
    libc_types::i32 const room = std::max(magazine.rounds_reserved_maximum - carried.rounds_total, 0);
    for(auto const& object : objects.value())
        if(object.equipment.tag_id.value() == pack.value())
            return std::clamp<libc_types::i32>(object.rounds, 0, room);
    return 0;
}
