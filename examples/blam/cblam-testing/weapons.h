#pragma once

#include "components.h"

#include <blam/volta/blam_unit.h>
#include <coffee/components/entity_container.h>

#include <algorithm>
#include <array>

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

/* The magazine a weapon's first trigger loads from */
inline blam::scn::weapon::magazine_t const* first_magazine(
    blam::scn::weapon const* weapon, blam::map_ptr const& magic)
{
    if(!weapon)
        return nullptr;
    auto triggers  = weapon->triggers.data(magic);
    auto magazines = weapon->magazines.data(magic);
    if(!triggers.has_value() || triggers.value().empty() || !magazines.has_value())
        return nullptr;
    auto const i = triggers.value()[0].magazine;
    if(i < 0 || static_cast<size_t>(i) >= magazines.value().size())
        return nullptr;
    return &magazines.value()[i];
}

/* A new weapon's rounds, loaded then reserve: a full magazine and the
 * initial reserve */
inline std::array<libc_types::u16, 2> initial_rounds(
    blam::scn::weapon const* weapon, blam::map_ptr const& magic)
{
    auto const* magazine = first_magazine(weapon, magic);
    if(!magazine)
        return {};
    return {
        static_cast<libc_types::u16>(std::max<libc_types::i16>(magazine->rounds_loaded_maximum, 0)),
        static_cast<libc_types::u16>(std::max<libc_types::i16>(magazine->rounds_total_initial, 0)),
    };
}

/* Rounds a carried weapon's reserve has room for; none on a battery */
inline libc_types::i32 reserve_room(
    BipedEquipment::weapon_t const& carried, blam::map_ptr const& magic)
{
    auto const* magazine = first_magazine(carried.weapon, magic);
    if(!magazine || magazine->rounds_loaded_maximum <= 0)
        return 0;
    return std::max(magazine->rounds_reserved_maximum - carried.rounds_total, 0);
}

/* Rounds an ammo pack adds to a carried weapon's reserve: what its magazine
 * objects give for it, up to the reserve's maximum */
inline libc_types::i32 ammo_pack_rounds(
    BipedEquipment::weapon_t const& carried, blam::tag_index<> pack, blam::map_ptr const& magic)
{
    auto const* magazine = first_magazine(carried.weapon, magic);
    if(!magazine)
        return 0;
    auto objects = magazine->magazine_objects.data(magic);
    if(!objects.has_value())
        return 0;
    for(auto const& object : objects.value())
        if(object.equipment.tag_id.value() == pack.value())
            return std::clamp<libc_types::i32>(object.rounds, 0, reserve_room(carried, magic));
    return 0;
}
