#pragma once

/* Object tags (obje and derived), their models, collision, physics, AI
 * characters, item collections and the globals */

#include "dump.h"
#include "dump_scenario.h"

#include <blam/dimeter/objects.h>

namespace dimeter_info {

namespace obj = blam::dimeter::obj;

template<typename V>
inline void dump_hlmt_body(
    h2::map_container<V> const& map, obj::hlmt_t const& hlmt, const char* indent)
{
    fmt::print(
        "{}render model {}\n{}collision model {}\n{}animation {}\n{}physics {}\n"
        "{}physics model {}\n{}disappear={} fade={} lod reduce [{}, {}, {}, {}, {}]\n",
        indent,
        tag_label(map, hlmt.render_model),
        indent,
        tag_label(map, hlmt.collision_model),
        indent,
        tag_label(map, hlmt.animation),
        indent,
        tag_label(map, hlmt.physics),
        indent,
        tag_label(map, hlmt.physics_model),
        indent,
        from_le(hlmt.disappear_distance),
        from_le(hlmt.begin_fade_distance),
        from_le(hlmt.reduce_to_l1),
        from_le(hlmt.reduce_to_l2),
        from_le(hlmt.reduce_to_l3),
        from_le(hlmt.reduce_to_l4),
        from_le(hlmt.reduce_to_l5));
    each(map, hlmt.variants, [&](size_t i, obj::model_variant_t const& v) {
        fmt::print(
            "{}variant {} '{}': dialogue {} {} child objects\n",
            indent,
            i,
            sid(map, v.name),
            tag_label(map, v.dialogue),
            count_of(map, v.objects));
        each(map, v.regions, [&](size_t j, obj::variant_region_t const& r) {
            fmt::print(
                "{}  region {} '{}' (mode region {}):",
                indent,
                j,
                sid(map, r.name),
                r.runtime_region);
            each(map, r.permutations, [&](size_t, obj::variant_permutation_t const& pm) {
                fmt::print(
                    " '{}'={} p={}",
                    sid(map, pm.name),
                    pm.runtime_permutation,
                    from_le(pm.probability));
            });
            fmt::print("\n");
        });
    });
}

template<typename V>
inline void dump_hlmt(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    if(auto const* hlmt = tag.template data<obj::hlmt_t>(map.magic()))
        dump_hlmt_body(map, *hlmt, "  ");
}

template<typename V>
inline void dump_unit(h2::map_container<V> const& map, obj::unit_t const& u)
{
    fmt::print(
        "  unit: flags={:#x} team={} camera fov={} marker={} pitch=[{}, {}] "
        "grenades type={} count={} {} weapons\n",
        from_le(u.unit_flags),
        from_le(u.default_team),
        from_le(u.camera_field_of_view),
        sid(map, u.camera_marker_name),
        from_le(u.pitch_range[0]),
        from_le(u.pitch_range[1]),
        from_le(u.grenade_type),
        from_le(u.grenade_count_raw),
        count_of(map, u.weapons));
    each(map, u.weapons, [&](size_t i, obj::tag_ref_t const& w) {
        fmt::print("    weapon {}: {}\n", i, tag_label(map, w.tag));
    });
    each(map, u.seats, [&](size_t i, obj::seat_t const& s) {
        fmt::print(
            "    seat {} '{}': marker={} entry={} flags={:#x} ai_type={} camera={} "
            "gunner={} entry_radius={}\n",
            i,
            sid(map, s.seat_animation),
            sid(map, s.seat_marker_name),
            sid(map, s.entry_marker_name),
            from_le(s.flags),
            from_le(s.ai_seat_type),
            sid(map, s.camera_marker_name),
            tag_label(map, s.built_in_gunner),
            from_le(s.entry_radius));
    });
    each(map, u.postures, [&](size_t i, obj::posture_t const& p) {
        fmt::print("    posture {} '{}': pill offset {}\n", i, sid(map, p.name), v3(p.pill_offset));
    });
    fmt::print(
        "    melee {} boarding {} / {} landing {} flurry {} smash {}\n",
        tag_label(map, u.melee_damage),
        tag_label(map, u.boarding_melee_damage),
        tag_label(map, u.boarding_melee_response),
        tag_label(map, u.landing_melee_damage),
        tag_label(map, u.flurry_melee_damage),
        tag_label(map, u.obstacle_smash_damage));
}

template<typename V>
inline void dump_item(h2::map_container<V> const& map, obj::item_t const& it)
{
    fmt::print(
        "  item: flags={:#x} pickup='{}' swap='{}' picked_up='{}' collision sound {} "
        "detonation {} delay [{}, {}]\n",
        from_le(it.item_flags),
        sid(map, it.pickup_message),
        sid(map, it.swap_message),
        sid(map, it.picked_up_message),
        tag_label(map, it.collision_sound),
        tag_label(map, it.detonation_damage_effect),
        from_le(it.detonation_delay[0]),
        from_le(it.detonation_delay[1]));
}

template<typename V>
inline void dump_object(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* o = tag.template data<obj::object_t>(map.magic());
    if(!o)
        return;
    fmt::print(
        "  {}: flags={:#x} bounding radius={} offset {} acceleration_scale={} "
        "shadow_mode={} sweetener={} variant={}\n  model {}\n  crate {} shader {} "
        "creation {} material effects {}\n",
        obj::to_string(o->type()),
        from_le(o->flags),
        from_le(o->bounding_radius),
        v3(o->bounding_offset),
        from_le(o->acceleration_scale),
        from_le(o->lightmap_shadow_mode),
        o->sweetener_size,
        sid(map, o->default_model_variant),
        tag_label(map, o->model),
        tag_label(map, o->crate_object),
        tag_label(map, o->modifier_shader),
        tag_label(map, o->creation_effect),
        tag_label(map, o->material_effects));
    if(auto const* hlmt_tag = map.tag_at(o->model.tag_id))
        if(auto const* hlmt = hlmt_tag->template data<obj::hlmt_t>(map.magic()))
            dump_hlmt_body(map, *hlmt, "    ");
    each(map, o->ai_properties, [&](size_t, obj::ai_properties_t const& p) {
        fmt::print(
            "  ai: type='{}' size={} leap={} flags={:#x}\n",
            sid(map, p.ai_type_name),
            from_le(p.ai_size),
            from_le(p.leap_jump_speed),
            from_le(p.flags));
    });
    each(map, o->functions, [&](size_t i, obj::object_function_t const& f) {
        fmt::print(
            "  function {} '{}': export '{}' turn_off_with '{}' scale_by '{}' "
            "min={} {} bytes\n",
            i,
            sid(map, f.import_name),
            sid(map, f.export_name),
            sid(map, f.turn_off_with),
            sid(map, f.scale_by),
            from_le(f.minimum_value),
            f.default_function.size());
    });
    each(map, o->attachments, [&](size_t i, obj::attachment_t const& a) {
        fmt::print(
            "  attachment {}: {} at marker '{}' change_color={}\n",
            i,
            tag_label(map, a.type),
            sid(map, a.marker),
            from_le(a.change_color));
    });
    each(map, o->widgets, [&](size_t i, obj::tag_ref_t const& w) {
        fmt::print("  widget {}: {}\n", i, tag_label(map, w.tag));
    });
    fmt::print(
        "  {} change colors, {} predicted resources, hud message index {}\n",
        count_of(map, o->change_colors),
        count_of(map, o->predicted_resources),
        from_le(o->hud_text_message_index_raw));

    size_t const size = tag.size();
    switch(o->type())
    {
    case obj::object_type_t::biped: {
        if(size < sizeof(obj::biped_t))
            break;
        auto const& b = *reinterpret_cast<obj::biped_t const*>(o);
        dump_unit(map, b);
        fmt::print(
            "  biped: flags={:#x} jump={} turning speed={} heights standing={} "
            "crouching={} radius={} mass={} camera heights {} / {} autoaim width={} "
            "nodes pelvis={} head={} physics_control={} materials living='{}' "
            "dead='{}'\n",
            from_le(b.biped_flags),
            from_le(b.jump_velocity),
            from_le(b.moving_turning_speed),
            from_le(b.height_standing),
            from_le(b.height_crouching),
            from_le(b.radius),
            from_le(b.mass),
            from_le(b.standing_camera_height),
            from_le(b.crouching_camera_height),
            from_le(b.autoaim_width),
            b.pelvis_node.value(),
            b.head_node.value(),
            b.physics_control_node.value(),
            sid(map, b.living_global_material_name),
            sid(map, b.dead_global_material_name));
        fmt::print(
            "    landing soft [{} s, {} u/s] hard [{} s, {} .. {} u/s] death {} u/s "
            "stun={} area damage {} shapes dead={} pill={} sphere={}\n",
            from_le(b.maximum_soft_landing_time),
            from_le(b.minimum_soft_landing_velocity),
            from_le(b.maximum_hard_landing_time),
            from_le(b.minimum_hard_landing_velocity),
            from_le(b.maximum_hard_landing_velocity),
            from_le(b.death_hard_landing_velocity),
            from_le(b.stun_duration),
            tag_label(map, b.area_damage_effect),
            count_of(map, b.dead_sphere_shapes),
            count_of(map, b.pill_shapes),
            count_of(map, b.sphere_shapes));
        break;
    }
    case obj::object_type_t::vehicle: {
        if(size < sizeof(obj::vehicle_t))
            break;
        auto const& v = *reinterpret_cast<obj::vehicle_t const*>(o);
        dump_unit(map, v);
        fmt::print(
            "  vehicle: type={} control={} flags={:#x} speed forward={} reverse={} "
            "accel={} decel={} turn left={} right={} rate={} wheel={} size={} "
            "flip='{}' engine moment={} max angular={} {} gears\n",
            static_cast<u16>(v.vehicle_type()),
            from_le(v.control),
            from_le(v.vehicle_flags),
            from_le(v.maximum_forward_speed),
            from_le(v.maximum_reverse_speed),
            from_le(v.speed_acceleration),
            from_le(v.speed_deceleration),
            from_le(v.maximum_left_turn),
            from_le(v.maximum_right_turn),
            from_le(v.turn_rate),
            from_le(v.wheel_circumference),
            from_le(v.vehicle_size),
            sid(map, v.flip_message),
            from_le(v.engine_moment),
            from_le(v.engine_maximum_angular_velocity),
            count_of(map, v.gears));
        fmt::print(
            "    physics flags={:#x} ground friction={} depth={} damp={} moving={} "
            "slopes [{}, {}] gravity scale={} radius={} {} anti-gravity points "
            "{} friction points {} phantom shapes\n    sounds suspension {} crash {} "
            "effect {}\n",
            from_le(v.physics_flags),
            from_le(v.ground_friction),
            from_le(v.ground_depth),
            from_le(v.ground_damp_factor),
            from_le(v.ground_moving_friction),
            from_le(v.ground_maximum_slope[0]),
            from_le(v.ground_maximum_slope[1]),
            from_le(v.gravity_scale),
            from_le(v.radius),
            count_of(map, v.anti_gravity_points),
            count_of(map, v.friction_points),
            count_of(map, v.phantom_shapes),
            tag_label(map, v.suspension_sound),
            tag_label(map, v.crash_sound),
            tag_label(map, v.special_effect));
        break;
    }
    case obj::object_type_t::weapon: {
        if(size < sizeof(obj::weapon_t))
            break;
        auto const& w = *reinterpret_cast<obj::weapon_t const*>(o);
        dump_item(map, w);
        fmt::print(
            "  weapon: class='{}' name='{}' type={} mp_type={} tracking={} flags={:#x} "
            "ready={} s heat recovery={} overheat={} loss/s={} autoaim angle={} "
            "range={} magnetism angle={} range={} zoom levels={} [{}, {}] "
            "handle='{}' hud {}\n",
            sid(map, w.weapon_class),
            sid(map, w.weapon_name),
            from_le(w.weapon_type),
            from_le(w.multiplayer_weapon_type),
            from_le(w.tracking_type),
            from_le(w.weapon_flags),
            from_le(w.ready_time),
            from_le(w.heat_recovery_threshold),
            from_le(w.overheated_threshold),
            from_le(w.heat_loss_per_second),
            from_le(w.autoaim_angle),
            from_le(w.autoaim_range),
            from_le(w.magnetism_angle),
            from_le(w.magnetism_range),
            from_le(w.magnification_levels_raw),
            from_le(w.magnification_range[0]),
            from_le(w.magnification_range[1]),
            sid(map, w.handle_node),
            tag_label(map, w.new_hud_interface));
        fmt::print(
            "    melee {} / {} lunge {} / {} player melee {} pickup sound {}\n",
            tag_label(map, w.melee_damage[0]),
            tag_label(map, w.melee_damage[1]),
            tag_label(map, w.melee_damage[6]),
            tag_label(map, w.melee_damage[7]),
            tag_label(map, w.player_melee_damage),
            tag_label(map, w.pickup_sound));
        each(map, w.first_person, [&](size_t i, obj::first_person_t const& fp) {
            fmt::print(
                "    first person {}: model {} animations {}\n",
                i,
                tag_label(map, fp.model),
                tag_label(map, fp.animations));
        });
        each(map, w.magazines, [&](size_t i, obj::magazine_t const& m) {
            fmt::print(
                "    magazine {}: rounds initial={} max={} loaded max={} inventory "
                "max={} recharged={} reload={} s ({} rounds) chamber={} s flags={:#x} "
                "{} equipment\n",
                i,
                m.rounds_total_initial(),
                from_le(m.rounds_total_maximum_raw),
                m.rounds_loaded_maximum(),
                from_le(m.rounds_inventory_maximum_raw),
                from_le(m.rounds_recharged_raw),
                from_le(m.reload_time),
                from_le(m.rounds_reloaded_raw),
                from_le(m.chamber_time),
                from_le(m.flags),
                count_of(map, m.equipment));
            each(map, m.equipment, [&](size_t j, obj::magazine_equipment_t const& e) {
                fmt::print(
                    "      equipment {}: {} rounds={}\n",
                    j,
                    tag_label(map, e.equipment),
                    from_le(e.rounds_raw));
            });
        });
        each(map, w.triggers, [&](size_t i, obj::trigger_t const& t) {
            fmt::print(
                "    trigger {}: input={} behavior={} barrels primary={} secondary={} "
                "autofire={} s charging={} s charged={} s flags={:#x} effect {}\n",
                i,
                from_le(t.input),
                from_le(t.behavior),
                from_le(t.primary_barrel.raw),
                from_le(t.secondary_barrel.raw),
                from_le(t.autofire_time),
                from_le(t.charging_time),
                from_le(t.charged_time),
                from_le(t.flags),
                tag_label(map, t.charging_effect));
        });
        each(map, w.barrels, [&](size_t i, obj::barrel_t const& b) {
            fmt::print(
                "    barrel {}: rounds/s [{}, {}] accel={} decel={} shots/fire [{}, {}] "
                "recovery={} magazine={} rounds/shot={} min loaded={} tracers every {} "
                "marker='{}' flags={:#x}\n",
                i,
                from_le(b.rounds_per_second[0]),
                from_le(b.rounds_per_second[1]),
                from_le(b.acceleration_time),
                from_le(b.deceleration_time),
                from_le(b.shots_per_fire[0]),
                from_le(b.shots_per_fire[1]),
                from_le(b.fire_recovery_time),
                b.magazine.value(),
                b.rounds_per_shot(),
                from_le(b.minimum_rounds_loaded_raw),
                from_le(b.rounds_between_tracers_raw),
                sid(map, b.optional_barrel_marker_name),
                from_le(b.flags));
        });
        break;
    }
    case obj::object_type_t::equipment: {
        if(size < sizeof(obj::equipment_t))
            break;
        auto const& e = *reinterpret_cast<obj::equipment_t const*>(o);
        dump_item(map, e);
        fmt::print(
            "  equipment: powerup={} grenade={} time={} pickup sound {}\n",
            static_cast<u16>(e.powerup_type()),
            from_le(e.grenade_type),
            from_le(e.powerup_time),
            tag_label(map, e.pickup_sound));
        break;
    }
    case obj::object_type_t::machine:
    case obj::object_type_t::control:
    case obj::object_type_t::light_fixture: {
        if(size < sizeof(obj::device_t))
            break;
        auto const& d = *reinterpret_cast<obj::device_t const*>(o);
        fmt::print(
            "  device: flags={:#x} power transition={} s accel={} s position "
            "transition={} s accel={} s depowered {} s / {} s delay={} s activation "
            "radius={}\n    effects open {} close {} opened {} closed {} depowered {} "
            "repowered {} delay {}\n",
            from_le(d.device_flags),
            from_le(d.power_transition_time),
            from_le(d.power_acceleration_time),
            from_le(d.position_transition_time),
            from_le(d.position_acceleration_time),
            from_le(d.depowered_position_transition_time),
            from_le(d.depowered_position_acceleration_time),
            from_le(d.delay_time),
            from_le(d.automatic_activation_radius),
            tag_label(map, d.open_up),
            tag_label(map, d.close_down),
            tag_label(map, d.opened),
            tag_label(map, d.closed),
            tag_label(map, d.depowered),
            tag_label(map, d.repowered),
            tag_label(map, d.delay_effect));
        if(o->type() == obj::object_type_t::machine && size >= offsetof(obj::machine_t, unknown_3) + 2)
        {
            auto const& m = *reinterpret_cast<obj::machine_t const*>(o);
            fmt::print(
                "  machine: type={} flags={:#x} door open={} s occlusion [{}, {}] "
                "collision response={} elevator node={} pathfinding={}\n",
                static_cast<u16>(m.machine_type()),
                from_le(m.machine_flags),
                from_le(m.door_open_time),
                from_le(m.door_occlusion_bounds[0]),
                from_le(m.door_occlusion_bounds[1]),
                from_le(m.collision_response),
                m.elevator_node.value(),
                from_le(m.pathfinding_policy));
        }
        if(o->type() == obj::object_type_t::control && size >= sizeof(obj::control_t))
        {
            auto const& c = *reinterpret_cast<obj::control_t const*>(o);
            fmt::print(
                "  control: type={} triggers_when={} call_value={} action='{}' on {} "
                "off {} deny {}\n",
                from_le(c.control_type),
                from_le(c.triggers_when),
                from_le(c.call_value),
                sid(map, c.action_string),
                tag_label(map, c.on),
                tag_label(map, c.off),
                tag_label(map, c.deny));
        }
        break;
    }
    case obj::object_type_t::creature: {
        if(size < offsetof(obj::creature_t, dead_global_material) + 2)
            break;
        auto const& c = *reinterpret_cast<obj::creature_t const*>(o);
        fmt::print(
            "  creature: flags={:#x} team={} heights standing={} crouching={} "
            "radius={} mass={} turning max={} autoaim width={}\n",
            from_le(c.creature_flags),
            from_le(c.default_team),
            from_le(c.height_standing),
            from_le(c.height_crouching),
            from_le(c.radius),
            from_le(c.mass),
            from_le(c.turning_velocity_maximum),
            from_le(c.autoaim_width));
        break;
    }
    case obj::object_type_t::crate: {
        if(size < sizeof(obj::crate_t))
            break;
        auto const& c = *reinterpret_cast<obj::crate_t const*>(o);
        fmt::print("  crate: flags={:#x}\n", from_le(c.crate_flags));
        break;
    }
    default:
        break;
    }
}

template<typename V>
inline void dump_coll(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* coll = tag.template data<obj::coll::model_t>(map.magic());
    if(!coll)
        return;
    fmt::print("  flags={:#x}\n", from_le(coll->flags));
    each(map, coll->materials, [&](size_t i, obj::coll::material_t const& m) {
        fmt::print("  material {} '{}'\n", i, sid(map, m.name));
    });
    each(map, coll->regions, [&](size_t i, obj::coll::region_t const& r) {
        fmt::print("  region {} '{}'\n", i, sid(map, r.name));
        each(map, r.permutations, [&](size_t j, obj::coll::permutation_t const& p) {
            fmt::print(
                "    permutation {} '{}': {} bsp physics\n",
                j,
                sid(map, p.name),
                count_of(map, p.bsp_physics));
            each(map, p.bsps, [&](size_t k, obj::coll::bsp_ref_t const& b) {
                fmt::print("      bsp {} node={}:", k, b.node.value());
                dump_collision(map, map.magic(), b.bsp, " ");
            });
        });
    });
    each(map, coll->pathfinding_spheres, [&](size_t i, obj::coll::pathfinding_sphere_t const& s) {
        fmt::print(
            "  pathfinding sphere {}: node={} at {} radius={} flags={:#x}\n",
            i,
            s.node.value(),
            v3(s.center),
            from_le(s.radius),
            from_le(s.flags));
    });
    each(map, coll->nodes, [&](size_t i, obj::coll::node_t const& n) {
        fmt::print(
            "  node {} '{}': parent={} sibling={} child={}\n",
            i,
            sid(map, n.name),
            n.parent.value(),
            n.next_sibling.value(),
            n.first_child.value());
    });
}

template<typename V>
inline void dump_phmo(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* phmo = tag.template data<obj::phmo::model_t>(map.magic());
    if(!phmo)
        return;
    fmt::print(
        "  flags={:#x} mass={} deactivation scales [{}, {}] {} phantom types {} "
        "node edges {} mopps ({} bytes) constraints hinge={} ragdoll={} "
        "limited={} ball={} spring={} prismatic={} {} phantoms\n",
        from_le(phmo->flags),
        from_le(phmo->mass),
        from_le(phmo->low_frequency_deactivation_scale),
        from_le(phmo->high_frequency_deactivation_scale),
        count_of(map, phmo->phantom_types),
        count_of(map, phmo->node_edges),
        count_of(map, phmo->mopps),
        phmo->mopp_codes.size(),
        count_of(map, phmo->hinge_constraints),
        count_of(map, phmo->ragdoll_constraints),
        count_of(map, phmo->limited_hinge_constraints),
        count_of(map, phmo->ball_and_socket_constraints),
        count_of(map, phmo->stiff_spring_constraints),
        count_of(map, phmo->prismatic_constraints),
        count_of(map, phmo->phantoms));
    each(map, phmo->rigid_bodies, [&](size_t i, obj::phmo::rigid_body_t const& b) {
        fmt::print(
            "  rigid body {}: node={} region={} motion={} shape type={} index={} "
            "mass={} sphere {} r={} damping linear={} angular={} flags={:#x}\n",
            i,
            b.node.value(),
            b.region.value(),
            static_cast<u16>(b.motion_type()),
            static_cast<u16>(b.shape_type()),
            b.shape_index(),
            from_le(b.mass),
            v3(b.bounding_sphere_offset),
            from_le(b.bounding_sphere_radius),
            from_le(b.linear_damping),
            from_le(b.angular_damping),
            from_le(b.flags));
    });
    each(map, phmo->materials, [&](size_t i, obj::phmo::material_t const& m) {
        fmt::print(
            "  material {} '{}': global '{}' phantom={} flags={:#x}\n",
            i,
            sid(map, m.name),
            sid(map, m.global_material_name),
            from_le(m.phantom_type_raw),
            from_le(m.flags));
    });
    auto shape = [&](auto const& s, const char* what, size_t i) {
        fmt::print(
            "  {} {} '{}': material={} mass={} friction={} restitution={} "
            "volume={} flags={:#x}",
            what,
            i,
            sid(map, s.name),
            s.material.value(),
            from_le(s.mass),
            from_le(s.friction),
            from_le(s.restitution),
            from_le(s.volume),
            from_le(s.flags));
    };
    each(map, phmo->spheres, [&](size_t i, obj::phmo::sphere_t const& s) {
        shape(s, "sphere", i);
        fmt::print(" radius={} at {}\n", from_le(s.radius), v3(s.translation));
    });
    each(map, phmo->pills, [&](size_t i, obj::phmo::pill_t const& p) {
        shape(p, "pill", i);
        fmt::print(" radius={} {} .. {}\n", from_le(p.radius), v3(p.bottom), v3(p.top));
    });
    each(map, phmo->boxes, [&](size_t i, obj::phmo::box_t const& b) {
        shape(b, "box", i);
        fmt::print(" half extents {} at {}\n", v3(b.half_extents), v3(b.translation));
    });
    fmt::print(
        "  {} triangles {} polyhedra {} lists {} multi spheres\n",
        count_of(map, phmo->triangles),
        count_of(map, phmo->polyhedra),
        count_of(map, phmo->lists),
        count_of(map, phmo->multi_spheres));
    each(map, phmo->regions, [&](size_t i, obj::phmo::region_t const& r) {
        fmt::print("  region {} '{}': {} permutations\n", i, sid(map, r.name), count_of(map, r.permutations));
    });
    each(map, phmo->nodes, [&](size_t i, obj::phmo::node_t const& n) {
        fmt::print(
            "  node {} '{}': parent={} sibling={} child={} flags={:#x}\n",
            i,
            sid(map, n.name),
            n.parent.value(),
            n.sibling.value(),
            n.child.value(),
            from_le(n.flags));
    });
}

template<typename V>
inline void dump_char(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* c = tag.template data<obj::chr::character_t>(map.magic());
    if(!c)
        return;
    fmt::print(
        "  flags={:#x} parent {} unit {} creature {} style {} major {}\n",
        from_le(c->flags),
        tag_label(map, c->parent_character),
        tag_label(map, c->unit),
        tag_label(map, c->creature),
        tag_label(map, c->style),
        tag_label(map, c->major_character));
    each(map, c->variants, [&](size_t i, obj::chr::variant_t const& v) {
        fmt::print(
            "  variant {} '{}': index={} designator '{}'\n",
            i,
            sid(map, v.name),
            v.index.value(),
            sid(map, v.designator));
    });
    each(map, c->general, [&](size_t, obj::chr::general_t const& g) {
        fmt::print(
            "  general: flags={:#x} type={} scariness={}\n",
            from_le(g.flags),
            from_le(g.type),
            from_le(g.scariness));
    });
    each(map, c->vitality, [&](size_t, obj::chr::vitality_t const& v) {
        fmt::print(
            "  vitality: body={} shield={} legendary body={} shield={} recharge "
            "body delay={} time={} shield delay={} time={} stun threshold={} "
            "suicide radius={} flags={:#x}\n",
            from_le(v.normal_body_vitality),
            from_le(v.normal_shield_vitality),
            from_le(v.legendary_body_vitality),
            from_le(v.legendary_shield_vitality),
            from_le(v.body_recharge_delay_time),
            from_le(v.body_recharge_time),
            from_le(v.shield_recharge_delay_time),
            from_le(v.shield_recharge_time),
            from_le(v.stun_threshold),
            from_le(v.suicide_radius),
            from_le(v.flags));
    });
    each(map, c->perception, [&](size_t, obj::chr::perception_t const& p) {
        fmt::print(
            "  perception: vision distance={} angles central={} max={} "
            "peripheral={} ({} u) hearing={} notice projectile={} vehicle={} "
            "perception times combat={} guard={} non-combat={}\n",
            from_le(p.maximum_vision_distance),
            from_le(p.central_vision_angle),
            from_le(p.maximum_vision_angle),
            from_le(p.peripheral_vision_angle),
            from_le(p.peripheral_distance),
            from_le(p.hearing_distance),
            from_le(p.notice_projectile_chance),
            from_le(p.notice_vehicle_chance),
            from_le(p.combat_perception_time),
            from_le(p.guard_perception_time),
            from_le(p.non_combat_perception_time));
    });
    each(map, c->movement, [&](size_t, obj::chr::movement_t const& m) {
        fmt::print(
            "  movement: flags={:#x} pathfinding radius={} destination radius={} "
            "dive chance={} leap sizes [{}, {}] ignore={} smashable={} jump={} "
            "hints={:#x} throttle={}\n",
            from_le(m.flags),
            from_le(m.pathfinding_radius),
            from_le(m.destination_radius),
            from_le(m.dive_grenade_chance),
            from_le(m.obstacle_leap_minimum_size),
            from_le(m.obstacle_leap_maximum_size),
            from_le(m.obstacle_ignore_size),
            from_le(m.obstacle_smashable_size),
            from_le(m.jump_height),
            from_le(m.movement_hints),
            from_le(m.throttle_scale));
    });
    each(map, c->weapons, [&](size_t i, obj::chr::weapons_t const& w) {
        fmt::print(
            "  weapon {}: {} firing range [{}, {}] normal combat [{}, {}] timid "
            "[{}, {}] aggressive [{}, {}] bombardment={} flags={:#x}\n",
            i,
            tag_label(map, w.weapon),
            from_le(w.minimum_firing_range),
            from_le(w.maximum_firing_range),
            from_le(w.normal_combat_range[0]),
            from_le(w.normal_combat_range[1]),
            from_le(w.timid_combat_range[0]),
            from_le(w.timid_combat_range[1]),
            from_le(w.aggressive_combat_range[0]),
            from_le(w.aggressive_combat_range[1]),
            from_le(w.bombardment_range),
            from_le(w.flags));
    });
    each(map, c->placement, [&](size_t, obj::chr::placement_t const& p) {
        fmt::print(
            "  placement: upgrade chances few [{}, {}, {}, {}] normal [{}, {}, {}, {}] "
            "many [{}, {}, {}, {}]\n",
            from_le(p.few_upgrade_chance[0]),
            from_le(p.few_upgrade_chance[1]),
            from_le(p.few_upgrade_chance[2]),
            from_le(p.few_upgrade_chance[3]),
            from_le(p.normal_upgrade_chance[0]),
            from_le(p.normal_upgrade_chance[1]),
            from_le(p.normal_upgrade_chance[2]),
            from_le(p.normal_upgrade_chance[3]),
            from_le(p.many_upgrade_chance[0]),
            from_le(p.many_upgrade_chance[1]),
            from_le(p.many_upgrade_chance[2]),
            from_le(p.many_upgrade_chance[3]));
    });
    each(map, c->look, [&](size_t, obj::chr::look_t const& l) {
        fmt::print(
            "  look: aiming deviation {} looking deviation {} combat idle looking "
            "[{}, {}] aiming [{}, {}]\n",
            v2(l.maximum_aiming_deviation),
            v2(l.maximum_looking_deviation),
            from_le(l.combat_idle_looking[0]),
            from_le(l.combat_idle_looking[1]),
            from_le(l.combat_idle_aiming[0]),
            from_le(l.combat_idle_aiming[1]));
    });
    each(map, c->swarm, [&](size_t, obj::chr::swarm_t const& s) {
        fmt::print(
            "  swarm: scatter killed={} radius={} time={} hound [{}, {}]\n",
            from_le(s.scatter_killed_count_raw),
            from_le(s.scatter_radius),
            from_le(s.scatter_time),
            from_le(s.hound_distance[0]),
            from_le(s.hound_distance[1]));
    });
    each(map, c->ready, [&](size_t, obj::chr::ready_t const& r) {
        fmt::print(
            "  ready: time [{}, {}]\n",
            from_le(r.ready_time_bounds[0]),
            from_le(r.ready_time_bounds[1]));
    });
    each(map, c->engage, [&](size_t, obj::chr::engage_t const& e) {
        fmt::print(
            "  engage: flags={:#x} danger crouch={} stand={} move={}\n",
            from_le(e.flags),
            from_le(e.crouch_danger_threshold),
            from_le(e.stand_danger_threshold),
            from_le(e.fight_danger_move_threshold));
    });
    each(map, c->charge, [&](size_t, obj::chr::charge_t const& ch) {
        fmt::print(
            "  charge: flags={:#x} melee consider={} chance={} attack={} abort={} "
            "leap [{}, {}] chance={} berserk {}\n",
            from_le(ch.flags),
            from_le(ch.melee_consider_range),
            from_le(ch.melee_chance),
            from_le(ch.melee_attack_range),
            from_le(ch.melee_abort_range),
            from_le(ch.melee_leap_range[0]),
            from_le(ch.melee_leap_range[1]),
            from_le(ch.melee_leap_chance),
            tag_label(map, ch.berserk_weapon));
    });
    each(map, c->evasion, [&](size_t, obj::chr::evasion_t const& e) {
        fmt::print(
            "  evasion: danger={} delay={} chance={} proximity={} dive retreat={}\n",
            from_le(e.danger_threshold),
            from_le(e.delay_timer),
            from_le(e.chance),
            from_le(e.proximity_threshold),
            from_le(e.dive_retreat_chance));
    });
    each(map, c->cover, [&](size_t, obj::chr::cover_t const& co) {
        fmt::print(
            "  cover: flags={:#x} hide time [{}, {}] vitality={} shield={} chance "
            "[{}, {}] danger={} melee distance={}\n",
            from_le(co.flags),
            from_le(co.hide_behind_cover_time[0]),
            from_le(co.hide_behind_cover_time[1]),
            from_le(co.cover_vitality_threshold),
            from_le(co.cover_shield_fraction),
            from_le(co.cover_chance[0]),
            from_le(co.cover_chance[1]),
            from_le(co.cover_danger_threshold),
            from_le(co.proximity_melee_distance));
    });
    each(map, c->retreat, [&](size_t, obj::chr::retreat_t const& r) {
        fmt::print(
            "  retreat: flags={:#x} shield={} danger={} proximity={} leader dead={} "
            "peer dead={} zig-zag angle={} period={} grenade={} backup {}\n",
            from_le(r.flags),
            from_le(r.shield_threshold),
            from_le(r.danger_threshold),
            from_le(r.proximity_threshold),
            from_le(r.leader_dead_retreat_chance),
            from_le(r.peer_dead_retreat_chance),
            from_le(r.zig_zag_angle),
            from_le(r.zig_zag_period),
            from_le(r.retreat_grenade_chance),
            tag_label(map, r.backup_weapon));
    });
    each(map, c->search, [&](size_t, obj::chr::search_t const& s) {
        fmt::print(
            "  search: flags={:#x} time [{}, {}] uncover [{}, {}]\n",
            from_le(s.flags),
            from_le(s.search_time[0]),
            from_le(s.search_time[1]),
            from_le(s.uncover_distance_bounds[0]),
            from_le(s.uncover_distance_bounds[1]));
    });
    each(map, c->pre_search, [&](size_t, obj::chr::pre_search_t const& s) {
        fmt::print(
            "  pre-search: flags={:#x} min [{}, {}] max [{}, {}] certainty radius={} "
            "suppressing [{}, {}]\n",
            from_le(s.flags),
            from_le(s.minimum_presearch_time[0]),
            from_le(s.minimum_presearch_time[1]),
            from_le(s.maximum_presearch_time[0]),
            from_le(s.maximum_presearch_time[1]),
            from_le(s.minimum_certainty_radius),
            from_le(s.minimum_suppressing_time[0]),
            from_le(s.minimum_suppressing_time[1]));
    });
    each(map, c->idle, [&](size_t, obj::chr::idle_t const& i) {
        fmt::print(
            "  idle: pose delay [{}, {}]\n",
            from_le(i.idle_pose_delay_time[0]),
            from_le(i.idle_pose_delay_time[1]));
    });
    each(map, c->vocalization, [&](size_t, obj::chr::vocalization_t const& v) {
        fmt::print(
            "  vocalization: look comment={} long={}\n",
            from_le(v.look_comment_time),
            from_le(v.look_long_comment_time));
    });
    each(map, c->boarding, [&](size_t, obj::chr::boarding_t const& b) {
        fmt::print(
            "  boarding: flags={:#x} distance max={} abort={} speed={}\n",
            from_le(b.flags),
            from_le(b.maximum_distance),
            from_le(b.abort_distance),
            from_le(b.maximum_speed));
    });
    each(map, c->boss, [&](size_t, obj::chr::boss_t const& b) {
        fmt::print(
            "  boss: flurry damage={} time={}\n",
            from_le(b.flurry_damage_threshold),
            from_le(b.flurry_time));
    });
    each(map, c->firing_pattern, [&](size_t i, obj::chr::firing_pattern_properties_t const& f) {
        fmt::print("  firing pattern {}: {}\n", i, tag_label(map, f.weapon));
        each(map, f.patterns, [&](size_t j, obj::chr::firing_pattern_t const& p) {
            fmt::print(
                "    pattern {}: rate={} tracking={} leading={} burst duration [{}, {}] "
                "separation [{}, {}] damage={} error={}\n",
                j,
                from_le(p.rate_of_fire),
                from_le(p.target_tracking),
                from_le(p.target_leading),
                from_le(p.burst_duration[0]),
                from_le(p.burst_duration[1]),
                from_le(p.burst_separation[0]),
                from_le(p.burst_separation[1]),
                from_le(p.weapon_damage_modifier),
                from_le(p.projectile_error));
        });
    });
    each(map, c->grenades, [&](size_t, obj::chr::grenades_t const& g) {
        fmt::print(
            "  grenades: flags={:#x} type={} trajectory={} min enemies={} radius={} "
            "velocity={} ranges [{}, {}] chance={} delay={} count [{}, {}]\n",
            from_le(g.flags),
            from_le(g.grenade_type),
            from_le(g.trajectory_type),
            from_le(g.minimum_enemy_count_raw),
            from_le(g.enemy_radius),
            from_le(g.grenade_velocity),
            from_le(g.grenade_ranges[0]),
            from_le(g.grenade_ranges[1]),
            from_le(g.grenade_chance),
            from_le(g.grenade_throw_delay),
            from_le(g.grenade_count[0]),
            from_le(g.grenade_count[1]));
    });
    each(map, c->vehicle, [&](size_t i, obj::chr::vehicle_t const& v) {
        fmt::print(
            "  vehicle {}: {} style {} flags={:#x} pathfinding radius={} turning "
            "radius={} throttle max={} max speed={} ram timeout={}\n",
            i,
            tag_label(map, v.unit),
            tag_label(map, v.style),
            from_le(v.flags),
            from_le(v.ai_pathfinding_radius),
            from_le(v.ai_turning_radius),
            from_le(v.ai_throttle_maximum),
            from_le(v.theoretical_maximum_speed),
            from_le(v.vehicle_ram_timeout));
    });
}

template<typename V>
inline void dump_itmc(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* coll = tag.template data<obj::item_collection_t>(map.magic());
    if(!coll)
        return;
    fmt::print("  spawn time={}\n", coll->spawn_time());
    each(map, coll->permutations, [&](size_t i, obj::item_permutation_t const& p) {
        fmt::print(
            "  permutation {}: {} weight={} variant '{}'\n",
            i,
            tag_label(map, p.item),
            from_le(p.weight),
            sid(map, p.variant_name));
    });
}

template<typename V>
inline void dump_matg(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* g = tag.template data<obj::matg::globals_t>(map.magic());
    if(!g)
        return;
    fmt::print(
        "  language={} {} sounds {} ai globals {} difficulty {} interface tags {} "
        "weapons {} materials {} profile colors\n",
        from_le(g->language),
        count_of(map, g->sounds),
        count_of(map, g->ai_globals),
        count_of(map, g->difficulty),
        count_of(map, g->interface_tags),
        count_of(map, g->weapon_list),
        count_of(map, g->materials),
        count_of(map, g->profile_colors));
    each(map, g->grenades, [&](size_t i, obj::matg::grenade_t const& gr) {
        fmt::print(
            "  grenade {}: max={} equipment {} projectile {} effect {}\n",
            i,
            from_le(gr.maximum_count_raw),
            tag_label(map, gr.equipment),
            tag_label(map, gr.projectile),
            tag_label(map, gr.throwing_effect));
    });
    each(map, g->weapon_list, [&](size_t i, obj::tag_ref_t const& w) {
        fmt::print("  weapon {}: {}\n", i, tag_label(map, w.tag));
    });
    each(map, g->player_information, [&](size_t i, obj::matg::player_information_t const& p) {
        fmt::print(
            "  player information {}: walk={} run forward={} backward={} sideways={} "
            "accel={} sneak forward={} airborne accel={} grenade origin {} stun "
            "[{}, {}] s binoculars zoom {} [{}, {}] respawn effect {}\n",
            i,
            from_le(p.walking_speed),
            from_le(p.run_forward),
            from_le(p.run_backward),
            from_le(p.run_sideways),
            from_le(p.run_acceleration),
            from_le(p.sneak_forward),
            from_le(p.airborne_acceleration),
            v3(p.grenade_origin),
            from_le(p.minimum_stun_time),
            from_le(p.maximum_stun_time),
            from_le(p.binoculars_zoom_count_raw),
            from_le(p.binoculars_zoom_range[0]),
            from_le(p.binoculars_zoom_range[1]),
            tag_label(map, p.coop_respawn_effect));
    });
    each(map, g->player_representation, [&](size_t i, obj::matg::player_representation_t const& r) {
        fmt::print(
            "  player representation {}: hands {} body {} third person {} variant '{}'\n",
            i,
            tag_label(map, r.first_person_hands),
            tag_label(map, r.first_person_body),
            tag_label(map, r.third_person_unit),
            sid(map, r.third_person_variant));
    });
    each(map, g->multiplayer_information, [&](size_t i, obj::matg::multiplayer_information_t const& m) {
        fmt::print(
            "  multiplayer {}: flag {} unit {} ball {} hill shader {} flag shader {} "
            "text {} {} vehicles {} sounds events general={} slayer={} ctf={} "
            "oddball={} king={}\n",
            i,
            tag_label(map, m.flag),
            tag_label(map, m.unit),
            tag_label(map, m.ball),
            tag_label(map, m.hill_shader),
            tag_label(map, m.flag_shader),
            tag_label(map, m.in_game_text),
            count_of(map, m.vehicles),
            count_of(map, m.sounds),
            count_of(map, m.general_events),
            count_of(map, m.slayer_events),
            count_of(map, m.ctf_events),
            count_of(map, m.oddball_events),
            count_of(map, m.king_events));
        each(map, m.vehicles, [&](size_t j, obj::tag_ref_t const& v) {
            fmt::print("    vehicle {}: {}\n", j, tag_label(map, v.tag));
        });
    });
    each(map, g->materials, [&](size_t i, obj::matg::material_t const& m) {
        fmt::print(
            "  material {} '{}': parent '{}' armor '{}'/'{}' friction={} "
            "restitution={} density={} effects {}\n",
            i,
            sid(map, m.name),
            sid(map, m.parent_name),
            sid(map, m.general_armor),
            sid(map, m.specific_armor),
            from_le(m.friction),
            from_le(m.restitution),
            from_le(m.density),
            tag_label(map, m.material_effects));
    });
}

/*! Every class with a decoded dump; falls back to the geometry dumps */
template<typename V>
inline void dump_any(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    fmt::print(
        "{} [{}] {} bytes @{:#x}\n",
        map.tag_name(tag),
        class_name(tag.tag_class),
        tag.size(),
        tag.pointer());
    if(tag.matches("scnr"))
        dump_scenario(map, tag);
    else if(tag.matches("hlmt"))
        dump_hlmt(map, tag);
    else if(tag.matches("coll"))
        dump_coll(map, tag);
    else if(tag.matches("phmo"))
        dump_phmo(map, tag);
    else if(tag.matches("char"))
        dump_char(map, tag);
    else if(tag.matches("itmc") || tag.matches("vehc"))
        dump_itmc(map, tag);
    else if(tag.matches("matg"))
        dump_matg(map, tag);
    else if(derives_from(map, tag.tag_class, "obje"))
        dump_object(map, tag);
    else
        dump_tag(map, tag, false);
}

} // namespace dimeter_info
