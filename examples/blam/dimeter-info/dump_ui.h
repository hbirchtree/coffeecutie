#pragma once

/* Shell UI tags: screens, list skins, the globals and string lists */

#include "dump.h"
#include "dump_scenario.h"

#include <blam/dimeter/ui.h>

namespace dimeter_info {

namespace ui = blam::dimeter::ui;

inline std::string rect_str(ui::rect16_t const& r)
{
    return fmt::format(
        "[{},{} .. {},{}]",
        from_le(r.left),
        from_le(r.top),
        from_le(r.right),
        from_le(r.bottom));
}

inline std::string point_str(ui::point16_t const& p)
{
    return fmt::format("({}, {})", from_le(p.x), from_le(p.y));
}

inline std::string color_str(ui::color_argb_t const& c)
{
    return fmt::format(
        "argb({:.2f}, {:.2f}, {:.2f}, {:.2f})",
        from_le(c.a),
        from_le(c.r),
        from_le(c.g),
        from_le(c.b));
}

template<typename V>
inline void dump_text_blocks(
    h2::map_container<V> const&           map,
    h2::tag_block<ui::text_block_t> const& blocks,
    const char*                           indent)
{
    each(map, blocks, [&](size_t i, ui::text_block_t const& t) {
        fmt::print(
            "{}text {}: '{}' font {} anim {} delay {} flags {:#x} bounds {} "
            "color {} depth {}\n",
            indent,
            i,
            sid(map, t.text),
            static_cast<u16>(t.font),
            static_cast<u16>(t.animation),
            from_le(t.intro_animation_delay_ms),
            from_le(t.text_flags),
            rect_str(t.bounds),
            color_str(t.text_color),
            from_le(t.render_depth_bias));
    });
}

template<typename V>
inline void dump_bitmap_blocks(
    h2::map_container<V> const&             map,
    h2::tag_block<ui::bitmap_block_t> const& blocks,
    const char*                             indent)
{
    each(map, blocks, [&](size_t i, ui::bitmap_block_t const& b) {
        fmt::print(
            "{}bitmap {}: {} '{}' at {} scale ({}, {}) blend {} frame {} fps {} "
            "wraps/s ({}, {}) anim {} delay {} flags {:#x} depth {} progress {}\n",
            indent,
            i,
            tag_label(map, b.bitmap),
            sid(map, b.name),
            point_str(b.top_left),
            from_le(b.render_scale.x),
            from_le(b.render_scale.y),
            static_cast<u16>(b.blend),
            from_le(b.initial_sprite_frame),
            from_le(b.sprite_animation_fps),
            from_le(b.horizontal_wraps_per_second),
            from_le(b.vertical_wraps_per_second),
            static_cast<u16>(b.animation),
            from_le(b.intro_animation_delay_ms),
            from_le(b.flags),
            from_le(b.render_depth_bias),
            point_str(b.progress_bottom_left));
    });
}

template<typename V>
inline void dump_hud_blocks(
    h2::map_container<V> const&          map,
    h2::tag_block<ui::hud_block_t> const& blocks,
    const char*                          indent)
{
    each(map, blocks, [&](size_t i, ui::hud_block_t const& h) {
        fmt::print(
            "{}hud {}: {} shader {} bounds {} seq {} anim {} flags {:#x}\n",
            indent,
            i,
            tag_label(map, h.bitmap),
            tag_label(map, h.shader),
            rect_str(h.bounds),
            from_le(h.starting_sequence),
            static_cast<u16>(h.animation),
            from_le(h.flags));
    });
}

template<typename V>
inline void dump_player_blocks(
    h2::map_container<V> const&             map,
    h2::tag_block<ui::player_block_t> const& blocks,
    const char*                             indent)
{
    each(map, blocks, [&](size_t i, ui::player_block_t const& p) {
        fmt::print(
            "{}players {}: skin {} at {} {}x{} max {} cell {}x{} order {}\n",
            indent,
            i,
            tag_label(map, p.skin),
            point_str(p.bottom_left),
            p.rows,
            p.columns,
            p.max_players,
            from_le(p.column_width),
            from_le(p.row_height),
            static_cast<u8>(p.table_order));
    });
}

template<typename V>
inline void dump_model_scenes(
    h2::map_container<V> const&            map,
    h2::tag_block<ui::model_scene_t> const& scenes,
    const char*                            indent)
{
    each(map, scenes, [&](size_t i, ui::model_scene_t const& s) {
        fmt::print(
            "{}scene {}: camera ({}, {}, {}) fov {} viewport {} anim scale "
            "({}, {}, {}) anim {} flags {:#x}\n",
            indent,
            i,
            from_le(s.camera_position.x),
            from_le(s.camera_position.y),
            from_le(s.camera_position.z),
            from_le(s.fov_degrees),
            rect_str(s.viewport),
            from_le(s.animation_scale_factor.x),
            from_le(s.animation_scale_factor.y),
            from_le(s.animation_scale_factor.z),
            static_cast<u16>(s.animation),
            from_le(s.flags));
        each(map, s.objects, [&](size_t, ui::model_scene_object_t const& o) {
            fmt::print("{}  object '{}'\n", indent, o.name.str());
        });
        each(map, s.lights, [&](size_t, ui::model_scene_object_t const& o) {
            fmt::print("{}  light '{}'\n", indent, o.name.str());
        });
    });
}

template<typename V>
inline void dump_wgit(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* s = tag.template data<ui::screen_t>(map.magic());
    if(!s)
        return;
    fmt::print(
        "  screen id {} button key {} flags {:#x} header '{}' shape group {} "
        "strings {}\n  default text {} source ({:.2f}, {:.2f}, {:.2f}) dest "
        "({:.2f}, {:.2f}, {:.2f}) zoom ({}, {}) refraction ({}, {})\n",
        static_cast<u16>(s->screen_id),
        static_cast<u16>(s->button_key),
        from_le(s->flags),
        sid(map, s->header),
        from_le(s->shape_group),
        tag_label(map, s->strings),
        color_str(s->default_text_color),
        from_le(s->source_color.x),
        from_le(s->source_color.y),
        from_le(s->source_color.z),
        from_le(s->destination_color.x),
        from_le(s->destination_color.y),
        from_le(s->destination_color.z),
        from_le(s->accumulate_zoom_scale.x),
        from_le(s->accumulate_zoom_scale.y),
        from_le(s->refraction_scale.x),
        from_le(s->refraction_scale.y));
    each(map, s->panes, [&](size_t pi, ui::pane_t const& pane) {
        fmt::print(
            "  pane {}: anim {} unknown {}\n",
            pi,
            static_cast<u16>(pane.animation),
            from_le(pane.unknown_0));
        each(map, pane.buttons, [&](size_t i, ui::button_t const& b) {
            fmt::print(
                "    button {}: '{}' font {} bounds {} bitmap {} offset {} "
                "color {} text flags {:#x} button flags {:#x} anim {} delay {}\n",
                i,
                sid(map, b.text),
                static_cast<u16>(b.font),
                rect_str(b.bounds),
                tag_label(map, b.bitmap),
                point_str(b.bitmap_offset),
                color_str(b.text_color),
                from_le(b.text_flags),
                from_le(b.button_flags),
                static_cast<u16>(b.animation),
                from_le(b.intro_animation_delay_ms));
        });
        each(map, pane.lists, [&](size_t i, ui::list_definition_t const& l) {
            fmt::print(
                "    list {}: skin {} visible {} bottom left {} flags {:#x} "
                "anim {} delay {}\n",
                i,
                static_cast<u16>(l.skin),
                from_le(l.visible_items),
                point_str(l.bottom_left),
                from_le(l.flags),
                static_cast<u16>(l.animation),
                from_le(l.intro_animation_delay_ms));
            each(map, l.items, [&](size_t j, ui::text_value_pair_t const& p) {
                fmt::print(
                    "      item {}: label '{}' value '{}' type {} int {} "
                    "float {} bool {}\n",
                    j,
                    sid(map, p.text_label),
                    sid(map, p.text_value),
                    static_cast<u16>(p.value_type),
                    from_le(p.integer_value),
                    from_le(p.float_value),
                    from_le(p.boolean_value));
            });
        });
        dump_text_blocks(map, pane.text_blocks, "    ");
        dump_bitmap_blocks(map, pane.bitmap_blocks, "    ");
        dump_model_scenes(map, pane.model_scenes, "    ");
        each(map, pane.text_value_blocks, [&](size_t i, ui::text_value_block_t const& t) {
            fmt::print(
                "    text values {} '{}': {} pairs\n",
                i,
                t.name.str(),
                count_of(map, t.pairs));
        });
        dump_hud_blocks(map, pane.hud_blocks, "    ");
        dump_player_blocks(map, pane.player_blocks, "    ");
        if(count_of(map, pane.table_views))
            fmt::print("    {} table views\n", count_of(map, pane.table_views));
    });
    each(map, s->local_strings, [&](size_t i, ui::local_strings_t const& l) {
        fmt::print(
            "  local strings {} '{}': {} references\n",
            i,
            sid(map, l.section),
            count_of(map, l.references));
    });
    each(map, s->local_bitmaps, [&](size_t i, ui::local_bitmap_t const& b) {
        fmt::print("  local bitmap {}: {}\n", i, tag_label(map, b.bitmap));
    });
}

template<typename V>
inline void dump_skin(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* s = tag.template data<ui::list_skin_t>(map.magic());
    if(!s)
        return;
    fmt::print(
        "  flags {:#x} arrows {} up {} down {}\n",
        from_le(s->flags),
        tag_label(map, s->arrows),
        point_str(s->up_arrows_offset),
        point_str(s->down_arrows_offset));
    each(map, s->item_animations, [&](size_t i, ui::item_animation_t const& a) {
        fmt::print(
            "  item animation {}: period {} ms flags {:#x}",
            i,
            from_le(a.period_ms),
            from_le(a.flags));
        each(map, a.keyframes, [&](size_t, ui::keyframe_t const& k) {
            fmt::print(
                " [a={:.2f} ({}, {}, {})]",
                from_le(k.alpha),
                from_le(k.position.x),
                from_le(k.position.y),
                from_le(k.position.z));
        });
        fmt::print("\n");
    });
    dump_text_blocks(map, s->text_blocks, "  ");
    dump_bitmap_blocks(map, s->bitmap_blocks, "  ");
    dump_hud_blocks(map, s->hud_blocks, "  ");
    dump_player_blocks(map, s->player_blocks, "  ");
}

template<typename V>
inline void dump_wgtz(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* g = tag.template data<ui::globals_t>(map.magic());
    if(!g)
        return;
    fmt::print(
        "  shared globals {}\n  variant settings {}\n  hopper descriptions {}\n",
        tag_label(map, g->shared_globals),
        tag_label(map, g->mp_variant_settings),
        tag_label(map, g->game_hopper_descriptions));
    each(map, g->screens, [&](size_t i, ui::screen_widget_ref_t const& r) {
        fmt::print("  screen {}: {}\n", i, tag_label(map, r.widget));
    });
}

template<typename V>
inline void dump_wigl(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* g = tag.template data<ui::shared_globals_t>(map.magic());
    if(!g)
        return;
    fmt::print(
        "  overlay alpha {} text scale {} clip near {} proj {} far {}\n"
        "  progress bar {} overlay color {} default text {}\n"
        "  global bitmaps {}\n  strings {}\n  button key strings {}\n"
        "  game type strings {}\n  music {} fade {} ms\n"
        "  sounds: cursor {} select {} error {} advance {} retreat {}\n"
        "  header fonts: full {} large {} half {} quarter {}\n"
        "  header bounds: full {} large {} half {} quarter {}\n"
        "  button key bounds: full {} large {} half {} quarter {}\n",
        from_le(g->overlayed_screen_alpha_mod),
        from_le(g->callout_text_scale),
        from_le(g->near_clip_plane),
        from_le(g->projection_plane),
        from_le(g->far_clip_plane),
        color_str(g->progress_bar_color),
        color_str(g->overlayed_interface_color),
        color_str(g->default_text_color),
        tag_label(map, g->global_bitmaps),
        tag_label(map, g->strings),
        tag_label(map, g->button_key_strings),
        tag_label(map, g->game_type_strings),
        tag_label(map, g->main_menu_music),
        from_le(g->music_fade_time_ms),
        tag_label(map, g->cursor_sound),
        tag_label(map, g->selection_sound),
        tag_label(map, g->error_sound),
        tag_label(map, g->advancing_sound),
        tag_label(map, g->retreating_sound),
        static_cast<u16>(g->full_screen_header_font),
        static_cast<u16>(g->large_dialog_header_font),
        static_cast<u16>(g->half_dialog_header_font),
        static_cast<u16>(g->quarter_dialog_header_font),
        rect_str(g->full_screen_header_bounds),
        rect_str(g->large_dialog_header_bounds),
        rect_str(g->half_dialog_header_bounds),
        rect_str(g->quarter_dialog_header_bounds),
        rect_str(g->full_screen_button_key_bounds),
        rect_str(g->large_dialog_button_key_bounds),
        rect_str(g->half_dialog_button_key_bounds),
        rect_str(g->quarter_dialog_button_key_bounds));
    each(map, g->screen_animations, [&](size_t i, ui::screen_animation_t const& a) {
        fmt::print(
            "  animation {}: intro {} ms x{} outro {} ms x{} ambient {} ms x{} "
            "loop {} flags {:#x}\n",
            i,
            from_le(a.intro_period_ms),
            count_of(map, a.intro),
            from_le(a.outro_period_ms),
            count_of(map, a.outro),
            from_le(a.ambient_period_ms),
            count_of(map, a.ambient),
            from_le(a.ambient_looping_style),
            from_le(a.flags));
        auto frames = [&](const char* what, auto const& block) {
            auto data = block.data(map.magic());
            if(data.has_error() || data.value().empty())
                return;
            fmt::print("    {}:", what);
            for(auto const& k : data.value())
                fmt::print(
                    " [a={:.2f} ({}, {}, {})]",
                    from_le(k.alpha),
                    from_le(k.position.x),
                    from_le(k.position.y),
                    from_le(k.position.z));
            fmt::print("\n");
        };
        frames("intro", a.intro);
        frames("outro", a.outro);
        frames("ambient", a.ambient);
    });
    each(map, g->shape_groups, [&](size_t i, ui::shape_group_t const& sg) {
        fmt::print("  shape group {}:\n", i);
        each(map, sg.shapes, [&](size_t j, ui::shape_t const& s) {
            fmt::print(
                "    shape {}: color {} anim {} flags {:#x} depth {} points:",
                j,
                color_str(s.color),
                static_cast<u16>(s.animation),
                from_le(s.flags),
                from_le(s.render_depth_bias));
            each(map, s.points, [&](size_t, ui::shape_point_t const& p) {
                fmt::print(" {}", point_str(p.point));
            });
            fmt::print("\n");
        });
        dump_model_scenes(map, sg.model_scenes, "    ");
        dump_bitmap_blocks(map, sg.bitmap_blocks, "    ");
    });
    each(map, g->background_animations, [&](size_t i, ui::persistent_background_animation_t const& a) {
        fmt::print(
            "  background animation {}: period {} ms, {} keyframes\n",
            i,
            from_le(a.period_ms),
            count_of(map, a.keyframes));
    });
    each(map, g->list_skins, [&](size_t i, ui::list_skin_ref_t const& r) {
        fmt::print("  list skin {}: {}\n", i, tag_label(map, r.skin));
    });
}

template<typename V>
inline void dump_unic(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* u = tag.template data<ui::string_list_t>(map.magic());
    if(!u)
        return;
    fmt::print(
        "  {} import references, {} bytes import data\n",
        count_of(map, u->import_references),
        u->import_data.size());
    static constexpr const char* names[] = {
        "english",
        "japanese",
        "german",
        "french",
        "spanish",
        "italian",
        "korean",
        "chinese",
        "portuguese"};
    for(size_t i = 0; i < u->languages.size(); i++)
        if(from_le(u->languages[i].count))
            fmt::print(
                "  {}: {} strings from {}\n",
                names[i],
                from_le(u->languages[i].count),
                from_le(u->languages[i].first));
    auto const* packs = map.globals()
                            ? map.globals()->template data<ui::language_globals_t>(map.magic())
                            : nullptr;
    if(packs)
        for(size_t i = 0; i < packs->languages.size(); i++)
            if(auto const& p = packs->languages[i]; from_le(p.string_count))
                fmt::print(
                    "  table {}: {} strings, index @{:#x} data @{:#x} ({} bytes)\n",
                    names[i],
                    from_le(p.string_count),
                    from_le(p.index_offset),
                    from_le(p.data_offset),
                    from_le(p.table_size));
    u32 n = static_cast<u32>(std::max<i16>(0, from_le(u->languages[0].count)));
    for(u32 i = 0; i < n; i++)
        fmt::print(
            "  [{}] '{}'\n", i, ui::list_string(map, *u, ui::language_t::english, i));
    each(map, u->import_references, [&](size_t i, ui::import_string_ref_t const& r) {
        if(i < 8)
            fmt::print(
                "  ref {}: '{}' english offset {}\n",
                i,
                sid(map, r.id),
                from_le(r.offsets[0]));
    });
}

} // namespace dimeter_info
