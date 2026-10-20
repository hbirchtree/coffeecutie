#pragma once

#include "../common/tag_class.h"
#include "structures.h"

#include <array>
#include <cstring>
#include <string_view>

namespace blam::dimeter::ui {

/*!
 * Halo 2 shell UI tags, layouts from Assembly's Halo2Xbox wgit.xml,
 * skin.xml, wigl.xml and Halo2's wgtz.xml / unic.xml: screens (wgit) are
 * panes of buttons, lists, text, bitmaps and model scenes; list skins
 * (skin) draw the items; the globals (wgtz, wigl) name the screens, sounds
 * and shared bitmaps; string lists (unic) hold the text per language.
 */

struct alignas(2) point16_t
{
    i16 x, y;
};

static_assert(sizeof(point16_t) == 4);

/*! Halo's rectangle2d: top, left, bottom, right */
struct alignas(2) rect16_t
{
    i16 top, left, bottom, right;
};

static_assert(sizeof(rect16_t) == 8);

/*! real_argb_color */
struct alignas(4) color_argb_t
{
    f32 a, r, g, b;
};

static_assert(sizeof(color_argb_t) == 16);

enum class screen_id_t : u16
{
    test_1 = 0,
    test_2,
    test_3,
    test_4,
    test_5,
    game_shell_background,
    main_menu,
    error_dialog_ok_cancel,
    error_dialog_ok,
    press_start_intro,
    player_profile_select,
    sp_level_select,
    sp_difficulty_select,
    network_squad_browser,
    mp_pregame_lobby,
    custom_game_menu,
    postgame_stats,
    mp_map_select,
    sp_pause_game,
    settings,
    gamertag_select,
    gamertag_passcode_entry,
    multiplayer_protocol,
    squad_settings,
};

/*! The A/B/X/Y legend drawn at the bottom of a screen */
enum class button_key_t : u16
{
    none = 0,
    a_select_b_back,
    a_select_b_cancel,
    a_enter_b_cancel,
    y_live_players,
    x_friends_options,
    x_clan_options,
    x_recent_players_options,
    x_options,
    a_select,
    x_settings_a_select_b_back,
    x_delete_a_select_b_done,
    a_accept,
    b_cancel,
    y_live_players_a_select_b_back,
    y_live_players_a_select_b_cancel,
    y_live_players_a_enter_b_cancel,
    y_live_players_a_select,
    y_live_players_a_select_b_done,
    y_live_players_a_accept,
    y_live_players_b_cancel,
    x_delete_a_select_b_back,
    a_ok,
};

/*! Index into the disc's font_table.txt */
enum class font_t : u16
{
    terminal = 0,
    body_text,
    title,
    super_large,
    large_body_text,
    split_screen_hud_message,
    full_screen_hud_message,
    english_body_text,
    hud_number_text,
    subtitle,
    main_menu,
};

/*! Index into wigl's screen animations; none = 0, else index + 1 */
enum class animation_index_t : u16
{
    none = 0,
};

enum class blend_method_t : u16
{
    standard = 0,
    multiply,
};

enum class value_type_t : u16
{
    integer = 0,
    floating_point,
    boolean,
    text,
};

enum class table_order_t : u8
{
    row_major = 0,
    column_major,
};

enum class skin_index_t : u16
{
    default_ = 0,
    squad_lobby_player_list,
    settings_list,
    playlist_entry_list,
    variants,
    game_browser,
    online_player_menu,
    game_setup_menu,
    playlist_contents_display,
    player_profile_picker,
    mp_map_selection,
    main_menu_list,
    color_picker,
    profile_picker,
    y_menu_recent_list,
    carnage_report_team_stats,
    carnage_report_player_stats,
    carnage_report_kill_stats,
    carnage_report_pvp_stats,
    carnage_report_medal_stats,
    matchmaking_progress,
    default_5,
    default_6,
    advanced_settings_list,
};

/* A label or value shown in a list item */
struct alignas(4) text_value_pair_t
{
    value_type_t value_type;
    u16          boolean_value;
    i32          integer_value;
    f32          float_value;
    string_id    text_value;
    string_id    text_label;
};

static_assert(sizeof(text_value_pair_t) == 0x14);

struct alignas(4) button_t
{
    u32               text_flags;
    animation_index_t animation;
    i16               intro_animation_delay_ms;
    i16               unknown_0;
    font_t            font;
    color_argb_t      text_color;
    rect16_t          bounds;
    tag_reference<tag_class_t::bitm> bitmap;
    point16_t         bitmap_offset;
    string_id         text;
    i16               render_depth_bias;
    i16               unknown_1;
    u32               button_flags;
};

static_assert(sizeof(button_t) == 0x3C);

struct alignas(4) list_definition_t
{
    u32               flags;
    skin_index_t      skin;
    i16               visible_items;
    point16_t         bottom_left;
    animation_index_t animation;
    i16               intro_animation_delay_ms;
    tag_block<text_value_pair_t> items;
};

static_assert(sizeof(list_definition_t) == 0x18);

struct alignas(4) table_cell_t
{
    u8 unknown[0x1C];
};

struct alignas(4) table_row_t
{
    u32                     flags;
    i16                     row_height;
    i16                     unknown_0;
    tag_block<table_cell_t> cells;
};

static_assert(sizeof(table_row_t) == 0x10);

/* Obsolete, kept for the layout */
struct alignas(4) table_view_t
{
    u32                    flags;
    animation_index_t      animation;
    i16                    intro_animation_delay_ms;
    font_t                 font;
    i16                    unknown_0;
    color_argb_t           text_color;
    point16_t              top_left;
    tag_block<table_row_t> rows;
};

static_assert(sizeof(table_view_t) == 0x28);

struct alignas(4) text_block_t
{
    u32               text_flags;
    animation_index_t animation;
    i16               intro_animation_delay_ms;
    i16               unknown_0;
    font_t            font;
    color_argb_t      text_color;
    rect16_t          bounds;
    string_id         text;
    i16               render_depth_bias;
    i16               unknown_1;
};

static_assert(sizeof(text_block_t) == 0x2C);

struct alignas(4) bitmap_block_t
{
    u32               flags;
    animation_index_t animation;
    i16               intro_animation_delay_ms;
    blend_method_t    blend;
    i16               initial_sprite_frame;
    point16_t         top_left;
    f32               horizontal_wraps_per_second;
    f32               vertical_wraps_per_second;
    tag_reference<tag_class_t::bitm> bitmap;
    i16               render_depth_bias;
    i16               unknown_0;
    f32               sprite_animation_fps;
    point16_t         progress_bottom_left;
    string_id         name;
    Vecf2             render_scale;
};

static_assert(sizeof(bitmap_block_t) == 0x38);

/* Named scenery of the map's scenario shown in a viewport */
struct alignas(4) model_scene_object_t
{
    bl_string name;
};

static_assert(sizeof(model_scene_object_t) == 0x20);

struct alignas(4) model_scene_t
{
    u32                              flags;
    animation_index_t                animation;
    i16                              intro_animation_delay_ms;
    i16                              render_depth_bias;
    i16                              unknown_0;
    tag_block<model_scene_object_t>  objects;
    tag_block<model_scene_object_t>  lights;
    Vecf3                            animation_scale_factor;
    Vecf3                            camera_position;
    f32                              fov_degrees;
    rect16_t                         viewport;
    string_id                        unused_intro_animation;
    string_id                        unused_outro_animation;
    string_id                        unused_ambient_animation;
};

static_assert(sizeof(model_scene_t) == 0x4C);

struct alignas(4) text_value_block_t
{
    bl_string                    name;
    tag_block<text_value_pair_t> pairs;
};

static_assert(sizeof(text_value_block_t) == 0x28);

struct alignas(4) hud_block_t
{
    u32                              flags;
    animation_index_t                animation;
    i16                              intro_animation_delay_ms;
    i16                              render_depth_bias;
    i16                              starting_sequence;
    tag_reference<tag_class_t::bitm> bitmap;
    tag_reference<tag_class_t::shad> shader;
    rect16_t                         bounds;
};

static_assert(sizeof(hud_block_t) == 0x24);

struct alignas(4) player_block_t
{
    u32                              unknown_0;
    tag_reference<tag_class_t::skin> skin;
    point16_t                        bottom_left;
    table_order_t                    table_order;
    i8                               max_players;
    i8                               rows;
    i8                               columns;
    i16                              row_height;
    i16                              column_width;
};

static_assert(sizeof(player_block_t) == 0x18);

struct alignas(4) pane_t
{
    i16                             unknown_0;
    animation_index_t               animation;
    tag_block<button_t>             buttons;
    tag_block<list_definition_t>    lists;
    tag_block<table_view_t>         table_views;
    tag_block<text_block_t>         text_blocks;
    tag_block<bitmap_block_t>       bitmap_blocks;
    tag_block<model_scene_t>        model_scenes;
    tag_block<text_value_block_t>   text_value_blocks;
    tag_block<hud_block_t>          hud_blocks;
    tag_block<player_block_t>       player_blocks;
};

static_assert(sizeof(pane_t) == 0x4C);

struct alignas(4) string_id_ref_t
{
    string_id id;
};

struct alignas(4) local_strings_t
{
    string_id                  section;
    tag_block<string_id_ref_t> references;
};

static_assert(sizeof(local_strings_t) == 0xC);

struct alignas(4) local_bitmap_t
{
    tag_reference<tag_class_t::bitm> bitmap;
};

static_assert(sizeof(local_bitmap_t) == 8);

/*! wgit: one screen */
struct alignas(4) screen_t
{
    static constexpr tag_class_t tag_class = tag_class_t::wgit;

    u32          flags;
    screen_id_t  screen_id;
    button_key_t button_key;
    color_argb_t default_text_color;
    tag_reference<tag_class_t::unic> strings;
    tag_block<pane_t>                panes;
    i16                              shape_group; /*!< wigl shape group + 1 */
    i16                              unknown_0;
    string_id                        header;
    tag_block<local_strings_t>       local_strings;
    tag_block<local_bitmap_t>        local_bitmaps;
    /* The screen-space post effect behind the screen */
    Vecf3 source_color;
    Vecf3 destination_color;
    Vecf2 accumulate_zoom_scale;
    Vecf2 refraction_scale;
};

static_assert(sizeof(screen_t) == 0x68);

struct alignas(4) keyframe_t
{
    u32   unknown_0;
    f32   alpha;
    Vecf3 position;
};

static_assert(sizeof(keyframe_t) == 0x14);

struct alignas(4) item_animation_t
{
    u32                   flags;
    i32                   period_ms;
    tag_block<keyframe_t> keyframes;
};

static_assert(sizeof(item_animation_t) == 0x10);

/*! skin: how a list draws its items */
struct alignas(4) list_skin_t
{
    static constexpr tag_class_t tag_class = tag_class_t::skin;

    u32                              flags;
    tag_reference<tag_class_t::bitm> arrows;
    point16_t                        up_arrows_offset;
    point16_t                        down_arrows_offset;
    tag_block<item_animation_t>      item_animations;
    tag_block<text_block_t>          text_blocks;
    tag_block<bitmap_block_t>        bitmap_blocks;
    tag_block<hud_block_t>           hud_blocks;
    tag_block<player_block_t>        player_blocks;
};

static_assert(sizeof(list_skin_t) == 0x3C);

struct alignas(4) screen_widget_ref_t
{
    tag_reference<tag_class_t::wgit> widget;
};

/*! wgtz: the screens of the shell */
struct alignas(4) globals_t
{
    static constexpr tag_class_t tag_class = tag_class_t::wgtz;

    tag_reference<tag_class_t::wigl> shared_globals;
    tag_block<screen_widget_ref_t>   screens;
    tag_reference<>                  mp_variant_settings;
    tag_reference<>                  game_hopper_descriptions;
};

static_assert(sizeof(globals_t) == 0x20);

struct alignas(4) screen_animation_t
{
    u32                   flags;
    i32                   intro_period_ms;
    tag_block<keyframe_t> intro;
    i32                   outro_period_ms;
    tag_block<keyframe_t> outro;
    i32                   ambient_period_ms;
    i16                   ambient_looping_style;
    i16                   unknown_0;
    tag_block<keyframe_t> ambient;
};

static_assert(sizeof(screen_animation_t) == 0x2C);

struct alignas(4) shape_point_t
{
    point16_t point;
};

struct alignas(4) shape_t
{
    u32                      flags;
    animation_index_t        animation;
    i16                      intro_animation_delay_ms;
    color_argb_t             color;
    tag_block<shape_point_t> points;
    i16                      render_depth_bias;
    i16                      unknown_0;
    u8                       unknown_1[0xC];
};

static_assert(sizeof(shape_t) == 0x30);

/* The frame drawn around every screen of a kind: full screen, large and
 * small dialogs */
struct alignas(4) shape_group_t
{
    tag_block<shape_t>        shapes;
    tag_block<model_scene_t>  model_scenes;
    tag_block<bitmap_block_t> bitmap_blocks;
};

static_assert(sizeof(shape_group_t) == 0x18);

struct alignas(4) persistent_background_animation_t
{
    u32                   unknown_0;
    i32                   period_ms;
    tag_block<keyframe_t> keyframes;
};

static_assert(sizeof(persistent_background_animation_t) == 0x10);

struct alignas(4) list_skin_ref_t
{
    tag_reference<tag_class_t::skin> skin;
};

/*! wigl: everything the screens share */
struct alignas(4) shared_globals_t
{
    static constexpr tag_class_t tag_class = tag_class_t::wigl;

    i16          unknown_0;
    i16          unknown_1;
    u8           unknown_2[0x40];
    f32          overlayed_screen_alpha_mod;
    i16          incremental_text_update_period;
    i16          incremental_text_block_character;
    f32          callout_text_scale;
    color_argb_t progress_bar_color;
    f32          near_clip_plane;
    f32          projection_plane;
    f32          far_clip_plane;
    color_argb_t overlayed_interface_color;
    u8           unknown_3[0xC];
    u8           errors[8]; /*!< tag_block, layout not needed yet */
    tag_reference<tag_class_t::snd> cursor_sound;
    tag_reference<tag_class_t::snd> selection_sound;
    tag_reference<tag_class_t::snd> error_sound;
    tag_reference<tag_class_t::snd> advancing_sound;
    tag_reference<tag_class_t::snd> retreating_sound;
    tag_reference<tag_class_t::snd> initial_login_sound;
    tag_reference<tag_class_t::snd> vkbd_cursor_sound;
    tag_reference<tag_class_t::snd> vkbd_insert_sound;
    tag_reference<tag_class_t::snd> online_notification_sound;
    tag_reference<tag_class_t::snd> tabbing_sound;
    tag_reference<tag_class_t::snd> pregame_countdown_sound;
    tag_reference<>                 unknown_sound_0;
    tag_reference<tag_class_t::snd> matchmaking_advance_sound;
    tag_reference<>                 unknown_sound_1;
    tag_reference<>                 unknown_sound_2;
    tag_reference<>                 unknown_sound_3;
    tag_reference<tag_class_t::bitm> global_bitmaps;
    tag_reference<tag_class_t::unic> strings;
    tag_block<screen_animation_t>    screen_animations;
    tag_block<shape_group_t>         shape_groups;
    tag_block<persistent_background_animation_t> background_animations;
    tag_block<list_skin_ref_t>       list_skins;
    tag_reference<tag_class_t::unic> button_key_strings;
    tag_reference<tag_class_t::unic> game_type_strings;
    tag_reference<>                  unknown_4;
    u8                               skill_mappings[8];
    font_t                           full_screen_header_font;
    font_t                           large_dialog_header_font;
    font_t                           half_dialog_header_font;
    font_t                           quarter_dialog_header_font;
    color_argb_t                     default_text_color;
    rect16_t                         full_screen_header_bounds;
    rect16_t                         full_screen_button_key_bounds;
    rect16_t                         large_dialog_header_bounds;
    rect16_t                         large_dialog_button_key_bounds;
    rect16_t                         half_dialog_header_bounds;
    rect16_t                         half_dialog_button_key_bounds;
    rect16_t                         quarter_dialog_header_bounds;
    rect16_t                         quarter_dialog_button_key_bounds;
    tag_reference<tag_class_t::lsnd> main_menu_music;
    i32                              music_fade_time_ms;
};

static_assert(offsetof(shared_globals_t, cursor_sound) == 0x90);
static_assert(offsetof(shared_globals_t, global_bitmaps) == 0x110);
static_assert(offsetof(shared_globals_t, default_text_color) == 0x168);
static_assert(sizeof(shared_globals_t) == 0x1C4);

/* unic: the text itself lives in the cache's per-language string tables;
 * each language names its run of entries there */
struct alignas(4) language_table_t
{
    i16 first; /*!< Index into the language's string index */
    i16 count;
};

static_assert(sizeof(language_table_t) == 4);

enum class language_t : u8
{
    english = 0,
    japanese,
    german,
    french,
    spanish,
    italian,
    korean,
    chinese,
    portuguese,
    count,
};

struct alignas(4) import_string_ref_t
{
    string_id id;
    std::array<i32, static_cast<size_t>(language_t::count)> offsets;
};

static_assert(sizeof(import_string_ref_t) == 0x28);

/*! unic: a string list */
struct alignas(4) string_list_t
{
    static constexpr tag_class_t tag_class = tag_class_t::unic;

    tag_block<import_string_ref_t> import_references;
    data_reference<>               import_data;
    std::array<language_table_t, static_cast<size_t>(language_t::count)>
        languages;
};

static_assert(sizeof(string_list_t) == 0x34);

/* The per-language string tables, from the globals tag; the offsets are
 * file offsets. Layout from Assembly's H2X_Layouts_Core.xml */
struct alignas(4) language_pack_t
{
    u32 string_count;
    u32 table_size;
    u32 index_offset; /*!< string_count x {string_id, offset} */
    u32 data_offset;  /*!< UTF-8, offset per index entry */
    u8  unknown_0[0xC];
};

static_assert(sizeof(language_pack_t) == 0x1C);

struct alignas(4) language_globals_t
{
    u8 unknown_0[0x190];
    std::array<language_pack_t, static_cast<size_t>(language_t::count)>
        languages;
};

static_assert(offsetof(language_globals_t, languages) == 0x190);

struct alignas(4) locale_index_t
{
    string_id id;
    u32       offset;
};

/*! A string of a language's table by index; empty if absent */
template<typename Map>
inline std::string_view localized_string(
    Map const& map, language_t language, u32 index)
{
    auto const* globals = map.globals();
    auto const* packs =
        globals ? globals->template data<language_globals_t>(map.magic())
                : nullptr;
    if(!packs)
        return {};
    auto const& pack  = packs->languages[static_cast<size_t>(language)];
    u32         count = from_le(pack.string_count);
    u64         ioff  = from_le(pack.index_offset);
    u64         doff  = from_le(pack.data_offset);
    u64         dsize = from_le(pack.table_size);
    if(index >= count || ioff + (index + 1) * sizeof(locale_index_t) > map.data.size() ||
       doff + dsize > map.data.size())
        return {};
    auto const* entry = reinterpret_cast<locale_index_t const*>(
        map.data.data() + ioff + index * sizeof(locale_index_t));
    u64 at = from_le(entry->offset);
    if(at >= dsize)
        return {};
    auto const* text = reinterpret_cast<const char*>(map.data.data() + doff + at);
    auto        end  = static_cast<size_t>(dsize - at);
    return std::string_view(text, ::strnlen(text, end));
}

/*! A string of the list by its string id, found through the language's
 * index table; empty if the list has no such string */
template<typename Map>
inline std::string_view list_string_by_id(
    Map const& map, string_list_t const& list, language_t language, string_id id)
{
    auto const* globals = map.globals();
    auto const* packs =
        globals ? globals->template data<language_globals_t>(map.magic())
                : nullptr;
    if(!packs)
        return {};
    auto const& pack  = packs->languages[static_cast<size_t>(language)];
    auto const& range = list.languages[static_cast<size_t>(language)];
    u32         first = static_cast<u32>(std::max<i16>(0, from_le(range.first)));
    u32         count = static_cast<u32>(std::max<i16>(0, from_le(range.count)));
    u64         ioff  = from_le(pack.index_offset);
    if(first + count > from_le(pack.string_count) ||
       ioff + (first + count) * sizeof(locale_index_t) > map.data.size())
        return {};
    auto const* entries = reinterpret_cast<locale_index_t const*>(
        map.data.data() + ioff);
    for(u32 i = first; i < first + count; i++)
        if(from_le(entries[i].id.raw) == from_le(id.raw))
            return localized_string(map, language, i);
    return {};
}

/*! The string list's text for one language */
template<typename Map>
inline std::string_view list_string(
    Map const& map, string_list_t const& list, language_t language, u32 i)
{
    auto const& range = list.languages[static_cast<size_t>(language)];
    if(i >= static_cast<u32>(std::max<i16>(0, from_le(range.count))))
        return {};
    return localized_string(
        map, language, static_cast<u32>(from_le(range.first)) + i);
}

} // namespace blam::dimeter::ui
