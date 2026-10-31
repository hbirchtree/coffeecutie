#pragma once

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

void alloc_devices(compo::EntityContainer& e);
