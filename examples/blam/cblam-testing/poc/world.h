#pragma once

/* Builds a World (hit_model.h) out of a loaded map */

#include "hit_model.h"

#include <blam/volta/blam_collision.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_scenario.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_unit.h>

#include <string>

namespace poc {

template<typename Ver>
std::optional<World> load_world(
    blam::map_container<Ver> const& map, std::string& error)
{
    blam::tag_index_view<Ver> index(map);
    auto const&               magic = map.magic;

    /* The player's biped: globals -> multiplayer (or singleplayer) unit */
    blam::tagref_t unit_ref{};
    for(blam::tag_t const& tag : index)
    {
        if(!tag.valid() || !tag.matches(blam::tag_class_t::matg))
            continue;
        auto glob = tag.template data<blam::globals::globals>(magic);
        if(!glob.has_value())
            break;
        if(auto mp = glob.value()->multiplayer.data(magic);
           mp.has_value() && !mp.value().empty())
            unit_ref = mp.value()[0].unit;
        else if(auto sp = glob.value()->player.data(magic);
                sp.has_value() && !sp.value().empty())
            unit_ref = sp.value()[0].unit;
        break;
    }
    if(!unit_ref.valid())
    {
        error = "no player biped in globals";
        return std::nullopt;
    }
    auto biped_it = index.find(unit_ref);
    if(biped_it == index.end())
    {
        error = "player biped not in tag index";
        return std::nullopt;
    }
    auto biped = (*biped_it).template data<blam::scn::biped>(magic);
    if(biped.has_error())
    {
        error = "player biped has no data";
        return std::nullopt;
    }

    auto coll_it = index.find(biped.value()->collider);
    auto mod2_it = index.find(biped.value()->model);
    if(coll_it == index.end() || mod2_it == index.end())
    {
        error = "biped has no coll or mod2";
        return std::nullopt;
    }
    auto coll  = (*coll_it).template data<blam::coll::header>(magic);
    auto mod2  = (*mod2_it).template data<blam::mod2::header<Ver>>(magic);
    if(coll.has_error() || mod2.has_error())
    {
        error = "coll or mod2 has no data";
        return std::nullopt;
    }
    auto bones = mod2.value()->bones.data(magic);
    if(bones.has_error())
    {
        error = "mod2 has no bones";
        return std::nullopt;
    }
    auto model = build_hit_model(
        *coll.value(), bones.value().data(), bones.value().size(), magic);
    if(!model)
    {
        error = "could not build hit model";
        return std::nullopt;
    }

    World world{
        .biped_name = std::string(unit_ref.to_name().to_string(magic)),
        .model      = std::move(*model),
    };

    /* Terrain: the structure BSP of the first player start */
    auto scn = map.tags->scenario(map.map, magic);
    if(!scn)
    {
        error = "no scenario";
        return std::nullopt;
    }
    std::uint16_t bsp_index = 0;
    if(auto locs = (*scn)->player_start.locations.data(magic);
       locs.has_value())
        for(auto const& l : locs.value())
        {
            if(world.starts.empty())
                bsp_index = l.bsp_index;
            world.starts.push_back({l.pos, l.rot, true});
        }
    if(auto bsps = (*scn)->bsp_info.data(magic);
       bsps.has_value() && bsp_index < bsps.value().size())
    {
        auto const& bi  = bsps.value()[bsp_index];
        auto const  bm  = bi.bsp_magic(magic);
        auto        hdr = bi.to_bsp(bm).to_header().data(bm, blam::single_value);
        if(hdr.has_value())
            if(auto c = hdr.value()->collision_header.data(
                   bm, blam::single_value);
               c.has_value())
            {
                world.terrain.bsp   = c.value();
                world.terrain.magic = bm;
            }
    }
    if(!world.terrain.bsp)
    {
        error = "no structure BSP collision";
        return std::nullopt;
    }
    return world;
}

} // namespace poc
