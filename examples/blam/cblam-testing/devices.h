#pragma once

#include "components.h"

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>

#include <map>
#include <optional>
#include <vector>

/*! The scenario's device groups: one value each that the positions or powers
 *  of every device in the group follow. Started from the scenario, changed by
 *  controls, doors and scripts on the authority, and by the server elsewhere */
struct DeviceGroups : compo::SubsystemBase
{
    using type = DeviceGroups;
    using f32  = libc_types::f32;
    using i16  = libc_types::i16;
    using u32  = libc_types::u32;

    std::vector<f32>  values;
    std::vector<bool> changes_once; /*!< Spent once changed */
    std::vector<bool> spent;
    u32               load_generation{0};

    /*! A device's state while its section is not loaded, by net id: what it
     *  was left at, or what a script told it since */
    struct remembered_t
    {
        std::optional<f32>  position;
        std::optional<f32>  position_target;
        std::optional<f32>  power;
        std::optional<f32>  power_target;
        std::optional<bool> automatic;
        std::optional<bool> one_sided;
        std::optional<bool> used;
    };
    std::map<u32, remembered_t> remembered;

    bool valid(i16 group) const
    {
        return group >= 0 && static_cast<size_t>(group) < values.size();
    }

    f32 get(i16 group) const
    {
        return valid(group) ? values[group] : 0.f;
    }

    /*! False when the group can no longer change */
    bool set(i16 group, f32 value)
    {
        if(!valid(group) || spent[group])
            return false;
        if(values[group] == value)
            return true;
        values[group] = value;
        if(changes_once[group])
            spent[group] = true;
        return true;
    }
};

/*! The control a player's use presses: in reach, nearest the view centre.
 *  `usable` when it would take the press rather than deny it */
struct aimed_control_t
{
    libc_types::u64 id{0};
    bool            usable{false};
};

template<typename Proxy>
aimed_control_t aimed_control(Proxy& p, PlayerCamera const& cam)
{
    using f32 = libc_types::f32;
    constexpr f32 use_reach = 1.5f;
    constexpr f32 use_cone  = .5f; /* cos 60 deg */

    Vecf3 const     eye   = cam.camera.position;
    Vecf3 const     look  = glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
    aimed_control_t best  = {};
    f32             aimed = use_cone;
    for(auto entity : p.template select<DeviceState, Model>())
    {
        auto [device, model] = entity.components();
        if(!device.control)
            continue;
        Vecf3 const to = model.position - eye;
        f32 const   d  = glm::length(to);
        if(d > use_reach || d <= 0.f)
            continue;
        if(f32 const a = glm::dot(to / d, look); a > aimed)
        {
            aimed   = a;
            best.id = entity.id();
        }
    }
    if(best.id == 0)
        return best;
    auto const& device = *p.template get<DeviceState>(best.id);
    auto const& model  = *p.template get<Model>(best.id);
    bool const  both   = device.kind_flags & DeviceState::both_sides;
    bool const  facing = glm::dot(eye - model.position, model.rotation * Vecf3{1.f, 0.f, 0.f}) >= 0.f;
    best.usable        = !(device.flags & DeviceState::not_usable) && device.power > 0.f &&
                  !device.used && (both || facing);
    return best;
}

void alloc_devices(compo::EntityContainer& e);
