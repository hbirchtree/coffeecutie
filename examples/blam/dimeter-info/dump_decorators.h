#pragma once

#include "dump.h"

#include <blam/dimeter/geometry.h>
#include <blam/dimeter/scenario.h>

#include <map>

namespace dimeter_info {

/* The scenario's decorator palette and each structure BSP's cache: its
 * groups by set and class, and whether they carry sprite geometry */
template<typename V>
inline void dump_decorators(h2::map_container<V> const& map, h2::raw_pool const& pool)
{
    auto const* scnr_tag = map.scenario();
    auto const* scnr =
        scnr_tag ? scnr_tag->template data<h2::scn::scenario_t>(map.magic()) : nullptr;
    if(!scnr)
        return;
    if(auto palette = scnr->decorator_palette.data(map.magic()); !palette.has_error())
        for(auto const& [i, e] : stl_types::enumerate(palette.value()))
        {
            auto const* t = map.tag_at(e.tag.tag_id);
            auto const* set =
                t ? t->template data<h2::geo::decorator_set_t>(map.magic()) : nullptr;
            fmt::print("decorator set {}: {}\n", i, t ? map.name_of(*t) : "-");
            if(!set)
                continue;
            if(auto classes = set->classes.data(map.magic()); !classes.has_error())
                for(auto const& [j, c] : stl_types::enumerate(classes.value()))
                    fmt::print(
                        "  class {} {}: type {} permutations {}\n",
                        j,
                        map.string(c.name),
                        static_cast<u32>(c.type),
                        c.permutations.count());
            fmt::print(
                "  shaders {} models {} vertices {} indices {}\n",
                set->shaders.count(),
                set->models.count(),
                set->vertices.count(),
                set->indices.count());
        }
    auto bsp_refs = scnr->structure_bsps.data(map.magic());
    if(bsp_refs.has_error())
        return;
    for(auto const& [b, ref] : stl_types::enumerate(bsp_refs.value()))
    {
        auto        located = h2::locate_bsp(map, ref);
        auto const* bsp     = located.body;
        auto        magic   = located.magic;
        if(!bsp)
            continue;
        auto caches = bsp->decorator_cache.data(magic);
        if(caches.has_error())
            continue;
        for(auto const& cache : caches.value())
        {
            auto blocks = cache.cache_blocks.data(magic);
            auto groups = cache.groups.data(magic);
            fmt::print(
                "bsp {}: grid origin ({}, {}, {}) cells {} cache blocks {} groups {}\n",
                b,
                from_le(cache.grid_origin[0]),
                from_le(cache.grid_origin[1]),
                from_le(cache.grid_origin[2]),
                from_le(cache.cell_count),
                cache.cache_blocks.count(),
                cache.groups.count());
            if(blocks.has_error() || groups.has_error())
                continue;
            u32 raw_bytes = 0;
            for(auto const& block : blocks.value())
                if(pool.resolve(magic, &block, block.data, from_le(block.size)))
                    raw_bytes += from_le(block.size);
            fmt::print("  {} bytes of placement data resolved\n", raw_bytes);
            std::map<std::string, std::pair<u32, u32>> kinds;
            for(auto const& g : groups.value())
            {
                auto key = fmt::format(
                    "set {} class {} shader {} {}",
                    g.set,
                    g.type,
                    g.shader,
                    from_le(g.vertex_count) > 0 ? "sprites" : "no geometry");
                kinds[key].first++;
                kinds[key].second += static_cast<u32>(std::max<i16>(from_le(g.placement_count), 0));
            }
            for(auto const& [k, v] : kinds)
                fmt::print("  {}: {} groups, {} placements\n", k, v.first, v.second);
        }
    }
}

} // namespace dimeter_info
