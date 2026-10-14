#pragma once

/* scnr and the sbsp blocks beyond geometry: placements, spawns, netgame,
 * AI, scripts, cutscenes, palettes */

#include "dump.h"

#include <blam/dimeter/ai.h>
#include <blam/dimeter/scenario.h>
#include <blam/dimeter/script.h>

#include <type_traits>

/* fmt prints the dimeter enum classes as their integer */
#define DIMETER_INFO_FORMAT_AS(ns)                                          \
    namespace ns {                                                          \
    template<typename E>                                                    \
        requires std::is_scoped_enum_v<E>                                   \
    constexpr auto format_as(E e)                                           \
    {                                                                       \
        return static_cast<std::underlying_type_t<E>>(e);                   \
    }                                                                       \
    }
DIMETER_INFO_FORMAT_AS(blam::dimeter)
DIMETER_INFO_FORMAT_AS(blam::dimeter::geo)
DIMETER_INFO_FORMAT_AS(blam::dimeter::coll)
DIMETER_INFO_FORMAT_AS(blam::dimeter::scn)
DIMETER_INFO_FORMAT_AS(blam::dimeter::ai)
DIMETER_INFO_FORMAT_AS(blam::dimeter::hsc)
DIMETER_INFO_FORMAT_AS(blam::dimeter::obj)
DIMETER_INFO_FORMAT_AS(blam::dimeter::obj::coll)
DIMETER_INFO_FORMAT_AS(blam::dimeter::obj::phmo)
DIMETER_INFO_FORMAT_AS(blam::dimeter::obj::chr)
DIMETER_INFO_FORMAT_AS(blam::dimeter::obj::matg)
#undef DIMETER_INFO_FORMAT_AS

namespace dimeter_info {

namespace scn = blam::dimeter::scn;
namespace ai  = blam::dimeter::ai;
namespace hsc = blam::dimeter::hsc;

template<typename V>
inline std::string v3(V const& v)
{
    return fmt::format(
        "({:.4g}, {:.4g}, {:.4g})", from_le(v[0]), from_le(v[1]), from_le(v[2]));
}

template<typename V>
inline std::string v2(V const& v)
{
    return fmt::format("({:.4g}, {:.4g})", from_le(v[0]), from_le(v[1]));
}


template<typename V>
inline std::string sid(h2::map_container<V> const& map, h2::string_id id)
{
    auto s = map.string(id);
    return s.empty() ? std::string("-") : std::string(s);
}

/*! Iterates a tag block, silently skipping unresolvable ones */
template<typename V, typename T, typename Fn>
inline void each(
    h2::map_container<V> const& map, h2::tag_block<T> const& block, Fn&& fn)
{
    auto items = block.data(map.magic());
    if(items.has_error())
        return;
    for(auto const& [i, item] : stl_types::enumerate(items.value()))
        fn(i, item);
}

template<typename V, typename T>
inline size_t count_of(h2::map_container<V> const& map, h2::tag_block<T> const& block)
{
    auto items = block.data(map.magic());
    return items.has_error() ? 0 : items.value().size();
}

template<typename V>
inline void dump_placements(
    h2::map_container<V> const& map, scn::scenario_t const& scnr)
{
    auto names = scnr.object_names.data(map.magic());
    auto name_of = [&](auto const& p) -> std::string {
        if(names.has_error())
            return "-";
        auto const* n = p.name.in(names.value());
        return n ? std::string(n->name.str()) : "-";
    };
    auto block = [&](auto const& placements, auto const& palette, const char* what) {
        each(map, palette, [&](size_t i, scn::palette_entry_t const& p) {
            fmt::print("  {} palette {}: {}\n", what, i, tag_label(map, p.tag));
        });
        each(map, placements, [&](size_t i, auto const& p) {
            using P = std::remove_cvref_t<decltype(p)>;
            fmt::print(
                "  {} {}: palette={} name={} at {} rot={} scale={} bsp_flags={:#x} "
                "origin_bsp={} policy={}",
                what,
                i,
                p.palette.value(),
                name_of(p),
                v3(p.position),
                v3(p.rotation),
                from_le(p.scale),
                from_le(p.bsp_flags),
                p.origin_bsp.value(),
                p.bsp_policy);
            if constexpr(requires { p.variant; })
                fmt::print(" variant={}", sid(map, p.variant.variant_name));
            if constexpr(std::is_same_v<P, scn::unit_placement_t>)
                fmt::print(" vitality={}", from_le(p.body_vitality));
            if constexpr(std::is_same_v<P, scn::weapon_placement_t>)
                fmt::print(
                    " rounds={}/{}",
                    from_le(p.rounds_loaded_raw),
                    from_le(p.rounds_left_raw));
            if constexpr(std::is_base_of_v<scn::device_placement_t, P>)
                fmt::print(
                    " power_group={} position_group={} device_flags={:#x}",
                    p.power_group.value(),
                    p.position_group.value(),
                    from_le(p.device_flags));
            if constexpr(std::is_same_v<P, scn::light_fixture_placement_t>)
                fmt::print(
                    " color={} intensity={} falloff={} cutoff={}",
                    v3(p.color),
                    from_le(p.intensity),
                    from_le(p.falloff_angle),
                    from_le(p.cutoff_angle));
            if constexpr(std::is_same_v<P, scn::sound_scenery_placement_t>)
                fmt::print(
                    " volume_type={} height={} distance={}",
                    from_le(p.volume_type),
                    from_le(p.height),
                    v2(p.override_distance_bounds));
            if constexpr(std::is_same_v<P, scn::light_volume_placement_t>)
                fmt::print(
                    " light_type={} fov={} falloff={} cutoff={} target={}",
                    from_le(p.light_type),
                    from_le(p.field_of_view),
                    from_le(p.falloff_distance),
                    from_le(p.cutoff_distance),
                    v3(p.target_point));
            fmt::print("\n");
        });
    };
    block(scnr.scenery, scnr.scenery_palette, "scenery");
    block(scnr.bipeds, scnr.biped_palette, "biped");
    block(scnr.vehicles, scnr.vehicle_palette, "vehicle");
    block(scnr.equipment, scnr.equipment_palette, "equipment");
    block(scnr.weapons, scnr.weapon_palette, "weapon");
    block(scnr.machines, scnr.machine_palette, "machine");
    block(scnr.controls, scnr.control_palette, "control");
    block(scnr.light_fixtures, scnr.light_fixture_palette, "light fixture");
    block(scnr.sound_scenery, scnr.sound_scenery_palette, "sound scenery");
    block(scnr.light_volumes, scnr.light_volume_palette, "light volume");
    block(scnr.crates, scnr.crate_palette, "crate");
    block(scnr.creatures, scnr.creature_palette, "creature");
}

template<typename V>
inline void dump_spawns(h2::map_container<V> const& map, scn::scenario_t const& scnr)
{
    each(map, scnr.player_starting_profiles, [&](size_t i, scn::player_starting_profile_t const& p) {
        fmt::print(
            "  starting profile {} '{}': health_damage={} shield_damage={} "
            "primary={} ({}/{}) secondary={} ({}/{}) grenades frag={} plasma={}\n",
            i,
            p.name.str(),
            from_le(p.starting_health_damage),
            from_le(p.starting_shield_damage),
            tag_label(map, p.primary_weapon),
            from_le(p.primary_rounds_loaded_raw),
            from_le(p.primary_rounds_total_raw),
            tag_label(map, p.secondary_weapon),
            from_le(p.secondary_rounds_loaded_raw),
            from_le(p.secondary_rounds_total_raw),
            p.starting_frag_grenade_count,
            p.starting_plasma_grenade_count);
    });
    each(map, scnr.player_starting_locations, [&](size_t i, scn::player_spawn_t const& s) {
        fmt::print(
            "  player spawn {}: {} facing {} team={} bsp={} game_types=[{}, {}, {}, {}] "
            "spawn_types=[{}, {}, {}, {}] campaign_type={}\n",
            i,
            v3(s.position),
            from_le(s.facing_degrees),
            from_le(s.team),
            s.bsp_index.value(),
            scn::to_string(s.game_type(0)),
            scn::to_string(s.game_type(1)),
            scn::to_string(s.game_type(2)),
            scn::to_string(s.game_type(3)),
            from_le(s.spawn_types[0]),
            from_le(s.spawn_types[1]),
            from_le(s.spawn_types[2]),
            from_le(s.spawn_types[3]),
            from_le(s.campaign_player_type));
    });
    each(map, scnr.netgame_flags, [&](size_t i, scn::netgame_flag_t const& f) {
        fmt::print(
            "  netgame flag {}: {} at {} facing {} team={} id={} flags={:#x} "
            "spawn_object={} marker={}\n",
            i,
            scn::to_string(f.type()),
            v3(f.position),
            from_le(f.facing_degrees),
            static_cast<u16>(f.team()),
            from_le(f.identifier_raw),
            from_le(f.flags),
            sid(map, f.spawn_object_name),
            sid(map, f.spawn_marker_name));
    });
    each(map, scnr.netgame_equipment, [&](size_t i, scn::netgame_equipment_t const& s) {
        fmt::print(
            "  netgame equipment {}: {} at {} rot={} team={} spawn_time={} "
            "respawn_on_empty={} classification={} game_types=[{}, {}, {}, {}] "
            "flags={:#x}\n",
            i,
            tag_label(map, s.collection),
            v3(s.position),
            v3(s.orientation),
            from_le(s.team_index),
            from_le(s.spawn_time),
            from_le(s.respawn_on_empty_time),
            s.classification,
            scn::to_string(s.game_type(0)),
            scn::to_string(s.game_type(1)),
            scn::to_string(s.game_type(2)),
            scn::to_string(s.game_type(3)),
            from_le(s.flags));
    });
    each(map, scnr.starting_equipment, [&](size_t i, scn::starting_equipment_t const& s) {
        fmt::print(
            "  starting equipment {}: game_types=[{}, {}, {}, {}] flags={:#x}",
            i,
            scn::to_string(s.game_type(0)),
            scn::to_string(s.game_type(1)),
            scn::to_string(s.game_type(2)),
            scn::to_string(s.game_type(3)),
            from_le(s.flags));
        for(auto const& c : s.item_collections)
            if(c.tag_id.valid())
                fmt::print(" {}", tag_label(map, c));
        fmt::print("\n");
    });
    each(map, scnr.spawn_data, [&](size_t, scn::spawn_data_t const& d) {
        fmt::print(
            "  spawn data: dynamic heights [{}, {}] objective reset height {}\n",
            from_le(d.dynamic_spawn_lower_height),
            from_le(d.dynamic_spawn_upper_height),
            from_le(d.game_objective_reset_height));
        each(map, d.dynamic_spawn_overloads, [&](size_t i, scn::spawn_overload_t const& o) {
            fmt::print(
                "    overload {}: type={} radii [{}, {}] weight={}\n",
                i,
                from_le(o.type),
                from_le(o.inner_radius),
                from_le(o.outer_radius),
                from_le(o.weight));
        });
        auto zones = [&](auto const& block, const char* what) {
            each(map, block, [&](size_t i, scn::spawn_zone_t const& z) {
                fmt::print(
                    "    {} {} '{}': at {} heights [{}, {}] radii [{}, {}] weight={} "
                    "teams={:#x} games={:#x} flags={:#x}\n",
                    what,
                    i,
                    sid(map, z.name),
                    v3(z.position),
                    from_le(z.lower_height),
                    from_le(z.upper_height),
                    from_le(z.inner_radius),
                    from_le(z.outer_radius),
                    from_le(z.weight),
                    from_le(z.relevant_team),
                    from_le(z.relevant_games),
                    from_le(z.flags));
            });
        };
        zones(d.static_respawn_zones, "respawn zone");
        zones(d.static_initial_spawn_zones, "initial spawn zone");
    });
}

template<typename V>
inline void dump_volumes(h2::map_container<V> const& map, scn::scenario_t const& scnr)
{
    each(map, scnr.trigger_volumes, [&](size_t i, scn::trigger_volume_t const& v) {
        fmt::print(
            "  trigger volume {} '{}': at {} extents {} forward {} up {} "
            "object_name={} node={} kill={}\n",
            i,
            sid(map, v.name),
            v3(v.position),
            v3(v.extents),
            v3(v.forward),
            v3(v.up),
            v.object_name.value(),
            sid(map, v.node_name),
            v.kill_trigger_volume.value());
    });
    each(map, scnr.kill_triggers, [&](size_t i, scn::kill_trigger_t const& k) {
        fmt::print("  kill trigger {}: volume {}\n", i, k.trigger_volume.value());
    });
    each(map, scnr.bsp_switch_triggers, [&](size_t i, scn::bsp_switch_trigger_t const& sw) {
        fmt::print(
            "  bsp switch {}: volume {} bsp {} -> {}\n",
            i,
            sw.trigger_volume.value(),
            sw.source_bsp.value(),
            sw.destination_bsp.value());
    });
    each(map, scnr.bsp_transition_volumes, [&](size_t i, scn::bsp_transition_volume_t const& t) {
        fmt::print(
            "  bsp transition volume {}: key={} volume {}\n",
            i,
            from_le(t.bsp_index_key_raw),
            t.trigger_volume.value());
    });
}

template<typename V>
inline void dump_ai(h2::map_container<V> const& map, scn::scenario_t const& scnr)
{
    auto characters = scnr.character_palette.data(map.magic());
    auto character  = [&](h2::block_index<ai::character_ref_t> const& idx) {
        if(characters.has_error())
            return std::string("-");
        auto const* c = idx.in(characters.value());
        return c ? tag_label(map, c->character) : std::string("-");
    };
    each(map, scnr.character_palette, [&](size_t i, ai::character_ref_t const& c) {
        fmt::print("  character {}: {}\n", i, tag_label(map, c.character));
    });
    each(map, scnr.style_palette, [&](size_t i, ai::style_ref_t const& s) {
        fmt::print("  style {}: {}\n", i, tag_label(map, s.style));
    });
    each(map, scnr.squad_groups, [&](size_t i, ai::squad_group_t const& g) {
        fmt::print(
            "  squad group {} '{}': parent={} initial_order={}\n",
            i,
            g.name.str(),
            g.parent.value(),
            g.initial_order.value());
    });
    each(map, scnr.squads, [&](size_t i, ai::squad_t const& s) {
        fmt::print(
            "  squad {} '{}': team={} group={} count normal={} insane={} "
            "character={} ({}) zone={} order={} weapon={} vehicle={} "
            "delay={} flags={:#x} script='{}'\n",
            i,
            s.name.str(),
            static_cast<u16>(s.team()),
            s.parent_group.value(),
            s.normal_difficulty_count(),
            s.insane_difficulty_count(),
            s.character_type.value(),
            character(s.character_type),
            s.initial_zone.value(),
            s.initial_order.value(),
            s.initial_weapon.value(),
            s.vehicle_type.value(),
            from_le(s.delay_time),
            from_le(s.flags),
            s.placement_script.str());
        each(map, s.starting_locations, [&](size_t j, ai::squad_location_t const& l) {
            fmt::print(
                "    location {} '{}': at {} facing {} character={} weapon={} "
                "vehicle={} seat={} swarm={} variant={} flags={:#x} script='{}'\n",
                j,
                sid(map, l.name),
                v3(l.position),
                v2(l.facing),
                l.character_type.value(),
                l.initial_weapon.value(),
                l.vehicle_type.value(),
                static_cast<u16>(l.seat_type()),
                from_le(l.swarm_count_raw),
                sid(map, l.actor_variant),
                from_le(l.flags),
                l.placement_script.str());
        });
    });
    each(map, scnr.zones, [&](size_t i, ai::zone_t const& z) {
        fmt::print(
            "  zone {} '{}': bsp={} flags={:#x} {} firing positions, {} areas\n",
            i,
            z.name.str(),
            z.manual_bsp.value(),
            from_le(z.flags),
            count_of(map, z.firing_positions),
            count_of(map, z.areas));
        each(map, z.areas, [&](size_t j, ai::area_t const& a) {
            fmt::print(
                "    area {} '{}': at {} firing positions [{}, +{}) flags={:#x}\n",
                j,
                a.name.str(),
                v3(a.position),
                a.firing_position_start.value(),
                a.firing_position_count(),
                from_le(a.flags));
        });
        each(map, z.firing_positions, [&](size_t j, ai::firing_position_t const& f) {
            if(j < 4)
                fmt::print(
                    "    firing position {}: at {} area={} cluster={} flags={:#x} "
                    "normal={}\n",
                    j,
                    v3(f.position),
                    f.area.value(),
                    f.cluster.value(),
                    from_le(f.flags),
                    v2(f.normal));
        });
    });
    each(map, scnr.orders, [&](size_t i, ai::order_t const& o) {
        fmt::print(
            "  order {} '{}': style={} flags={:#x} combat_status={} entry_script='{}' "
            "follow_squad={} radius={}\n",
            i,
            o.name.str(),
            o.style.value(),
            from_le(o.flags),
            from_le(o.force_combat_status),
            o.entry_script.str(),
            o.follow_squad.value(),
            from_le(o.follow_radius));
        auto areas = [&](auto const& block, const char* what) {
            each(map, block, [&](size_t j, ai::area_ref_t const& a) {
                fmt::print(
                    "    {} {}: type={} zone={} area={}\n",
                    what,
                    j,
                    from_le(a.area_type),
                    a.zone.value(),
                    a.area.value());
            });
        };
        areas(o.primary_area_set, "primary area");
        areas(o.secondary_area_set, "secondary area");
        each(map, o.endings, [&](size_t j, ai::order_ending_t const& e) {
            fmt::print(
                "    ending {}: next_order={} rule={} delay={} dialogue={}",
                j,
                e.next_order.value(),
                from_le(e.combination_rule),
                from_le(e.delay_time),
                from_le(e.dialogue_type));
            each(map, e.triggers, [&](size_t, ai::trigger_ref_t const& t) {
                bool negated = (t.flags & ai::trigger_ref_flags_t::negate) !=
                               ai::trigger_ref_flags_t::none;
                fmt::print(" trigger {}{}", t.trigger.value(), negated ? "(not)" : "");
            });
            fmt::print("\n");
        });
    });
    each(map, scnr.ai_triggers, [&](size_t i, ai::trigger_t const& t) {
        fmt::print(
            "  ai trigger {} '{}': flags={:#x} rule={}\n",
            i,
            t.name.str(),
            from_le(t.flags),
            from_le(t.combination_rule));
        each(map, t.conditions, [&](size_t j, ai::trigger_rule_t const& c) {
            fmt::print(
                "    condition {}: type={} squad={} group={} a={} x={} volume={} "
                "script='{}' flags={:#x}\n",
                j,
                static_cast<u16>(c.rule_type()),
                c.squad.value(),
                c.squad_group.value(),
                c.a(),
                from_le(c.x),
                c.trigger_volume.value(),
                c.exit_condition_script.str(),
                from_le(c.flags));
        });
    });
    each(map, scnr.mission_scenes, [&](size_t i, ai::mission_scene_t const& m) {
        fmt::print(
            "  mission scene {} '{}': flags={:#x} {} conditions {} roles\n",
            i,
            sid(map, m.name),
            from_le(m.flags),
            count_of(map, m.trigger_conditions),
            count_of(map, m.roles));
    });
    each(map, scnr.ai_conversations, [&](size_t i, ai::conversation_t const& c) {
        fmt::print(
            "  conversation {} '{}': trigger_distance={} run_to_player={} "
            "{} participants {} lines\n",
            i,
            c.name.str(),
            from_le(c.trigger_distance),
            from_le(c.run_to_player_distance),
            count_of(map, c.participants),
            count_of(map, c.lines));
    });
    each(map, scnr.ai_animation_references, [&](size_t i, ai::animation_reference_t const& a) {
        fmt::print("  ai animation {} '{}': {}\n", i, a.name.str(), tag_label(map, a.graph));
    });
    each(map, scnr.ai_script_references, [&](size_t i, ai::named_reference_t const& r) {
        fmt::print("  ai script reference {} '{}'\n", i, r.name.str());
    });
    each(map, scnr.ai_recording_references, [&](size_t i, ai::named_reference_t const& r) {
        fmt::print("  ai recording reference {} '{}'\n", i, r.name.str());
    });
}

/*! Script and global tables; `decompile` prints each body as source */
template<typename V>
inline void dump_scripts(
    h2::map_container<V> const& map, scn::scenario_t const& scnr, bool decompile)
{
    auto syntax = scnr.syntax(map.magic());
    fmt::print(
        "  scripting: {} scripts, {} globals, {} expression nodes, {} bytes of "
        "strings, syntax data {} bytes\n",
        count_of(map, scnr.scripts),
        count_of(map, scnr.globals),
        syntax.nodes.size(),
        syntax.strings.size(),
        scnr.script_syntax_data.size());
    {
        u32 live = 0, outside = 0;
        for(auto const& n : syntax.nodes)
            if(n.salt() != 0 && static_cast<u16>(n.type()) < 0x20)
            {
                live++;
                outside += n.string_offset() >= syntax.strings.size();
            }
        fmt::print(
            "  {} live nodes, {} with string offsets past the table\n", live, outside);
    }
    /* The table's names against the ones the compiler wrote down */
    u32 groups = 0, mismatches = 0;
    for(auto const& node : syntax.nodes)
    {
        if(node.type() != hsc::expression_type_t::group)
            continue;
        auto const* head = syntax.at(node.data_datum());
        if(!head)
            continue;
        auto token = syntax.string(*head);
        if(token.empty())
            continue;
        groups++;
        auto expected = hsc::to_string(static_cast<hsc::bc::v8>(node.opcode()));
        if(token != expected)
        {
            if(mismatches < 8)
                fmt::print(
                    "    opcode {} table says '{}', script says '{}'\n",
                    node.opcode(),
                    expected,
                    token);
            mismatches++;
        }
    }
    fmt::print("  {} function calls, {} disagree with the opcode table\n", groups, mismatches);
    each(map, scnr.globals, [&](size_t i, hsc::global_t const& g) {
        if(decompile)
        {
            std::string out = "(global ";
            out += hsc::to_string(g.type());
            out += ' ';
            out += g.name.str();
            out += ' ';
            hsc::decompile(syntax, g.initializer, out, 2);
            out += ")";
            fmt::print("{}\n", out);
        } else
            fmt::print(
                "  global {} '{}': {} init={:#x}\n",
                i,
                g.name.str(),
                hsc::to_string(g.type()),
                from_le(g.initializer.raw));
    });
    each(map, scnr.scripts, [&](size_t i, hsc::script_t const& s) {
        if(decompile)
            fmt::print("{}", hsc::decompile_script(syntax, s));
        else
            fmt::print(
                "  script {} '{}': {} {} root={:#x}\n",
                i,
                s.name.str(),
                hsc::to_string(s.type()),
                hsc::to_string(s.return_type()),
                from_le(s.root.raw));
    });
    each(map, scnr.script_references, [&](size_t i, hsc::script_reference_t const& r) {
        fmt::print("  script reference {}: {}\n", i, tag_label(map, r.reference));
    });
    each(map, scnr.scripting_data, [&](size_t, hsc::scripting_data_t const& d) {
        each(map, d.point_sets, [&](size_t i, hsc::point_set_t const& ps) {
            fmt::print(
                "  point set {} '{}': bsp={} flags={:#x}\n",
                i,
                ps.name.str(),
                ps.bsp_index.value(),
                from_le(ps.flags));
            each(map, ps.points, [&](size_t j, hsc::point_t const& p) {
                fmt::print(
                    "    point {} '{}': at {} facing {} surface={}\n",
                    j,
                    p.name.str(),
                    v3(p.position),
                    v2(p.facing),
                    from_le(p.surface_index_raw));
            });
        });
    });
}

template<typename V>
inline void dump_cutscenes(h2::map_container<V> const& map, scn::scenario_t const& scnr)
{
    fmt::print(
        "  cutscene blocks: {} flags, {} camera points, {} titles\n",
        scnr.cutscene_flags.count(),
        scnr.cutscene_camera_points.count(),
        scnr.cutscene_titles.count());
    each(map, scnr.cutscene_flags, [&](size_t i, scn::cutscene_flag_t const& f) {
        fmt::print(
            "  cutscene flag {} '{}': at {} facing {}\n",
            i,
            f.name.str(),
            v3(f.position),
            v2(f.facing));
    });
    each(map, scnr.cutscene_camera_points, [&](size_t i, scn::cutscene_camera_point_t const& c) {
        fmt::print(
            "  camera point {} '{}': type={} flags={:#x} at {} orientation {}\n",
            i,
            c.name.str(),
            from_le(c.type),
            from_le(c.flags),
            v3(c.position),
            v3(c.orientation));
    });
    each(map, scnr.cutscene_titles, [&](size_t i, scn::cutscene_title_t const& t) {
        fmt::print(
            "  cutscene title {} '{}': bounds [{}, {}, {}, {}] font={} justify={} "
            "times in={} up={} out={}\n",
            i,
            sid(map, t.name),
            from_le(t.text_bounds[0]),
            from_le(t.text_bounds[1]),
            from_le(t.text_bounds[2]),
            from_le(t.text_bounds[3]),
            from_le(t.font),
            from_le(t.justification),
            from_le(t.fade_in_time),
            from_le(t.up_time),
            from_le(t.fade_out_time));
    });
    each(map, scnr.recorded_animations, [&](size_t i, scn::recorded_animation_t const& r) {
        fmt::print(
            "  recorded animation {} '{}': version={} length={} ticks, {} bytes\n",
            i,
            r.name.str(),
            r.version,
            from_le(r.length_raw),
            r.event_stream.size());
    });
}

template<typename V>
inline void dump_environment(h2::map_container<V> const& map, scn::scenario_t const& scnr)
{
    each(map, scnr.background_sound_palette, [&](size_t i, geo::background_sound_palette_t const& b) {
        fmt::print(
            "  background sound {} '{}': {} inside {} cutoff={} scales interior={} "
            "portal={} exterior={}\n",
            i,
            b.name.str(),
            tag_label(map, b.background_sound),
            tag_label(map, b.inside_cluster_sound),
            from_le(b.cutoff_distance),
            from_le(b.interior_scale),
            from_le(b.portal_scale),
            from_le(b.exterior_scale));
    });
    each(map, scnr.sound_environment_palette, [&](size_t i, geo::sound_environment_palette_t const& s) {
        fmt::print(
            "  sound environment {} '{}': {} cutoff={}\n",
            i,
            s.name.str(),
            tag_label(map, s.sound_environment),
            from_le(s.cutoff_distance));
    });
    each(map, scnr.weather_palette, [&](size_t i, geo::weather_palette_t const& w) {
        fmt::print(
            "  weather {} '{}': {} wind {} direction {} magnitude={}\n",
            i,
            w.name.str(),
            tag_label(map, w.weather_system),
            tag_label(map, w.wind),
            v3(w.wind_direction),
            from_le(w.wind_magnitude));
    });
    each(map, scnr.atmospheric_fog_palette, [&](size_t i, scn::atmospheric_fog_t const& f) {
        fmt::print(
            "  atmospheric fog {} '{}': color {} density={} start={} opaque={} "
            "spread={}\n",
            i,
            sid(map, f.name),
            v3(f.color),
            from_le(f.maximum_density),
            from_le(f.start_distance),
            from_le(f.opaque_distance),
            from_le(f.spread_distance));
    });
    each(map, scnr.planar_fog_palette, [&](size_t i, scn::planar_fog_t const& f) {
        fmt::print("  planar fog {} '{}': {}\n", i, sid(map, f.name), tag_label(map, f.planar_fog));
    });
    each(map, scnr.cluster_data, [&](size_t i, scn::cluster_data_t const& c) {
        fmt::print(
            "  cluster data {}: {} sounds={} environments={} weather={} fog={} "
            "centroids={}\n",
            i,
            tag_label(map, c.bsp),
            count_of(map, c.background_sounds),
            count_of(map, c.sound_environments),
            count_of(map, c.weather),
            count_of(map, c.atmospheric_fog),
            count_of(map, c.centroids));
    });
    each(map, scnr.flocks, [&](size_t i, scn::flock_t const& f) {
        fmt::print(
            "  flock {} '{}': {} bsp={} volume={} boids [{}, {}] {} sources {} sinks\n",
            i,
            sid(map, f.name),
            tag_label(map, f.creature),
            f.bsp.value(),
            f.bounding_trigger_volume.value(),
            from_le(f.boid_count[0]),
            from_le(f.boid_count[1]),
            count_of(map, f.sources),
            count_of(map, f.sinks));
    });
    each(map, scnr.decal_palette, [&](size_t i, scn::tag_ref_t const& d) {
        fmt::print("  decal palette {}: {}\n", i, tag_label(map, d.tag));
    });
    fmt::print("  {} decals\n", count_of(map, scnr.decals));
    each(map, scnr.detail_object_collection_palette, [&](size_t i, scn::palette_entry_t const& d) {
        fmt::print("  detail object collection {}: {}\n", i, tag_label(map, d.tag));
    });
    each(map, scnr.decorator_palette, [&](size_t i, scn::tag_ref_t const& d) {
        fmt::print("  decorator set {}: {}\n", i, tag_label(map, d.tag));
    });
    each(map, scnr.structure_bsp_lighting, [&](size_t i, scn::bsp_lighting_t const& l) {
        fmt::print("  bsp lighting {}: {}\n", i, tag_label(map, l.bsp));
    });
}

template<typename V>
inline void dump_scenario(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* scnr = tag.template data<scn::scenario_t>(map.magic());
    if(!scnr)
        return;
    fmt::print(
        "  type={} flags={:#x} local_north={} {} comments {} environment objects\n",
        static_cast<u16>(scnr->type()),
        from_le(scnr->flags),
        from_le(scnr->local_north),
        count_of(map, scnr->comments),
        count_of(map, scnr->environment_objects));
    each(map, scnr->skies, [&](size_t i, scn::sky_ref_t const& s) {
        fmt::print("  sky {}: {}\n", i, tag_label(map, s.sky));
    });
    each(map, scnr->child_scenarios, [&](size_t i, scn::child_scenario_t const& c) {
        fmt::print("  child scenario {}: {}\n", i, tag_label(map, c.child));
    });
    each(map, scnr->structure_bsps, [&](size_t i, geo::structure_bsp_ref_t const& ref) {
        fmt::print(
            "  structure bsp {}: {} lightmap {} region={:#x} size={} magic={:#x} "
            "default_sky={}\n",
            i,
            tag_label(map, ref.bsp),
            tag_label(map, ref.lightmap),
            ref.meta_address(),
            from_le(ref.size_raw),
            ref.magic(),
            ref.default_sky.value());
    });
    fmt::print(
        "  custom object names {} chapter titles {} hud messages {} subtitles {} "
        "global lighting {} sound effects {}\n",
        tag_label(map, scnr->custom_object_names),
        tag_label(map, scnr->chapter_title_text),
        tag_label(map, scnr->hud_messages),
        tag_label(map, scnr->subtitles),
        tag_label(map, scnr->global_lighting),
        tag_label(map, scnr->sound_effect_collection));
    each(map, scnr->object_names, [&](size_t i, scn::object_name_t const& n) {
        fmt::print(
            "  object name {} '{}': type={} placement={}\n",
            i,
            n.name.str(),
            from_le(n.type_raw) == 0xFFFF ? std::string_view("-")
                                          : blam::dimeter::obj::to_string(n.type()),
            n.placement_index());
    });
    each(map, scnr->device_groups, [&](size_t i, scn::device_group_t const& g) {
        fmt::print(
            "  device group {} '{}': initial={} flags={:#x}\n",
            i,
            g.name.str(),
            from_le(g.initial_value),
            from_le(g.flags));
    });
    dump_placements(map, *scnr);
    dump_spawns(map, *scnr);
    dump_volumes(map, *scnr);
    dump_ai(map, *scnr);
    dump_scripts(map, *scnr, false);
    dump_cutscenes(map, *scnr);
    dump_environment(map, *scnr);
    each(map, scnr->unit_seats_mapping, [&](size_t i, scn::unit_seats_mapping_t const& m) {
        fmt::print("  unit seats mapping {}: {} seats={:#x}\n", i, tag_label(map, m.unit), from_le(m.seats));
    });
    each(map, scnr->scenario_resources, [&](size_t, scn::scenario_resources_t const& r) {
        fmt::print(
            "  resources: {} references, {} script sources, {} ai resources\n",
            count_of(map, r.references),
            count_of(map, r.script_source),
            count_of(map, r.ai_resources));
    });
    each(map, scnr->old_structure_physics, [&](size_t i, scn::old_structure_physics_t const& p) {
        fmt::print(
            "  structure physics {}: mopp {} bytes, {} environment objects, bounds {} .. {}\n",
            i,
            p.mopp_code.size(),
            count_of(map, p.environment_object_identifiers),
            v3(p.mopp_bounds_min),
            v3(p.mopp_bounds_max));
    });
}

/*! The sbsp blocks past the geometry: materials, markers, baked objects,
 * sound and weather, water */
template<typename V>
inline void dump_sbsp_extras(
    h2::map_container<V> const& map,
    geo::bsp_header_t const&    bsp,
    h2::bsp_magic const&        magic)
{
    auto each_b = [&](auto const& block, auto&& fn) {
        auto items = block.data(magic);
        if(items.has_error())
            return;
        for(auto const& [i, item] : stl_types::enumerate(items.value()))
            fn(i, item);
    };
    auto count_b = [&](auto const& block) -> size_t {
        auto items = block.data(magic);
        return items.has_error() ? 0 : items.value().size();
    };
    each_b(bsp.collision_materials, [&](size_t i, geo::collision_material_t const& m) {
        fmt::print(
            "  collision material {}: {} global={} conveyor={}\n",
            i,
            tag_label(map, m.shader),
            m.global_material.value(),
            from_le(m.conveyor_surface.raw));
    });
    each_b(bsp.fog_planes, [&](size_t i, geo::fog_plane_t const& f) {
        fmt::print(
            "  fog plane {}: scenario fog={} plane ({:.3g}, {:.3g}, {:.3g}; {:.3g}) "
            "priority={} flags={:#x}\n",
            i,
            from_le(f.scenario_planar_fog.raw),
            from_le(f.plane[0]),
            from_le(f.plane[1]),
            from_le(f.plane[2]),
            from_le(f.plane[3]),
            from_le(f.priority_raw),
            from_le(f.flags));
    });
    each_b(bsp.weather_palette, [&](size_t i, geo::weather_palette_t const& w) {
        fmt::print(
            "  weather {} '{}': {} wind {} {}\n",
            i,
            w.name.str(),
            tag_label(map, w.weather_system),
            tag_label(map, w.wind),
            v3(w.wind_direction));
    });
    fmt::print(
        "  {} weather polyhedra, {} detail object cells, {} conveyor surfaces, "
        "{} breakable surfaces, {} pathfinding data, {} runtime decals, "
        "{} transparent planes\n",
        count_b(bsp.weather_polyhedra),
        count_b(bsp.detail_objects),
        count_b(bsp.conveyor_surfaces),
        count_b(bsp.breakable_surfaces),
        count_b(bsp.pathfinding_data),
        count_b(bsp.runtime_decals),
        count_b(bsp.transparent_planes));
    each_b(bsp.background_sound_palette, [&](size_t i, geo::background_sound_palette_t const& b) {
        fmt::print(
            "  background sound {} '{}': {} inside {} cutoff={}\n",
            i,
            b.name.str(),
            tag_label(map, b.background_sound),
            tag_label(map, b.inside_cluster_sound),
            from_le(b.cutoff_distance));
    });
    each_b(bsp.sound_environment_palette, [&](size_t i, geo::sound_environment_palette_t const& s) {
        fmt::print(
            "  sound environment {} '{}': {} cutoff={}\n",
            i,
            s.name.str(),
            tag_label(map, s.sound_environment),
            from_le(s.cutoff_distance));
    });
    each_b(bsp.markers, [&](size_t i, geo::marker_t const& m) {
        fmt::print(
            "  marker {} '{}': at {} rotation ({:.3g}, {:.3g}, {:.3g}, {:.3g})\n",
            i,
            m.name.str(),
            v3(m.position),
            from_le(m.rotation[0]),
            from_le(m.rotation[1]),
            from_le(m.rotation[2]),
            from_le(m.rotation[3]));
    });
    each_b(bsp.environment_object_palette, [&](size_t i, geo::environment_object_palette_t const& p) {
        fmt::print(
            "  environment object palette {}: {} model {} type={:#x}\n",
            i,
            tag_label(map, p.definition),
            tag_label(map, p.model),
            from_le(p.object_type));
    });
    each_b(bsp.environment_objects, [&](size_t i, geo::environment_object_t const& o) {
        fmt::print(
            "  environment object {} '{}': palette={} at {} scenario object '{}' "
            "type {} id={:#x}\n",
            i,
            o.name.str(),
            o.palette.value(),
            v3(o.translation),
            o.scenario_object_name.str(),
            o.exported_object_type.str(),
            from_le(o.unique_id.raw));
    });
    auto sound_clusters = [&](auto const& block, const char* what) {
        each_b(block, [&](size_t i, geo::sound_cluster_t const& c) {
            fmt::print(
                "  {} cluster {}: {} portals, {} interior clusters\n",
                what,
                i,
                count_b(c.enclosing_portal_designators),
                count_b(c.interior_cluster_indices));
        });
    };
    sound_clusters(bsp.ambience_sound_clusters, "ambience sound");
    sound_clusters(bsp.reverb_sound_clusters, "reverb sound");
    each_b(bsp.water_definitions, [&](size_t i, geo::water_definition_t const& w) {
        fmt::print("  water {}: {} {} sections\n", i, tag_label(map, w.shader), count_b(w.section));
    });
    fmt::print(
        "  vehicle limit radius={} center {} mopp {} bytes bounds {} .. {} "
        "decorators {}\n",
        from_le(bsp.vehicle_spherical_limit_radius),
        v3(bsp.vehicle_spherical_limit_center),
        bsp.mopp_codes.size(),
        v3(bsp.mopp_bounds_min),
        v3(bsp.mopp_bounds_max),
        tag_label(map, bsp.decorators));
}

} // namespace dimeter_info
