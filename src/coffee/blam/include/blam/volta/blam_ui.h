#pragma once

#include "blam_base_types.h"
#include "blam_reference.h"
#include "blam_tag_ref.h"

#include <optional>

namespace blam {

using vec2i16 = typing::vectors::tvector<i16, 2>;
using vec4i16 = typing::vectors::tvector<i16, 4>;

struct ui_element
{
    enum class widget_type_t : u16
    {
        container,
        text_box,
        spinner_list,
        column_list,
        game_model,
        movie,
        custom,
    } widget_type;
    enum class controller_index_t : u16
    {
        player_1,
        player_2,
        player_3,
        player_4,
        any_player,
    } controller_index;
    bl_string name;
    vec4i16   bounds;
    enum class flags_t : u32
    {
        pass_unhandled_events_to_focused_child = 0x1,
        pause_game_time                        = 0x2,
        flash_background_bitmap                = 0x4,
        dpad_ud_tabs_through_children          = 0x8,
        dpad_lr_tabs_through_children          = 0x10,
        dpad_ud_tabs_through_items             = 0x20,
        dpad_lr_tabs_through_items             = 0x40,
        dont_focus_specific_child              = 0x80,
        pass_unhandled_events_to_all_children  = 0x100,
        render_regardless_of_controller        = 0x200,
        pass_handled_events_to_all_children    = 0x400,
        return_to_main_menu_if_no_history      = 0x800,
        always_use_tag_controller_index        = 0x1000,
        always_use_nifty_render_fix            = 0x2000,
        dont_push_history                      = 0x4000,
        force_handle_mouse                     = 0x8000,
    } flags;
    i32                               millis_to_auto_close;
    i32                               millis_auto_close_fade_time;
    tagref_typed_t<tag_class_t::bitm> background;

    enum class function_t : u16
    {
        clear_multiplayer_player_joins = 14,
        join_controller_to_mp_game     = 15,
        mp_profile_set_for_controller  = 37,
        mp_profile_change_name         = 41,
        player_profile_change_name     = 66,
        save_changes_profile_item      = 67,
    };

    /* Guerilla notes:
     * These functions use current game data to modify the appearance of
     * the widget. These functions are called every time the widget is rendered.
     */
    struct data_input_t
    {
        function_t function;
        u32        unknown[8];
    };

    reference<data_input_t> data_inputs;

    /* Guerilla notes:
     * These allow actions to be tied to certain UI events
     * The event handler runs every time the widget receives the specified event
     * By default, the "back" and "B" buttons will take you to the previous screen
     */
    struct event_handler_t
    {
        enum class flags_t : u32
        {
            close_current_widget       = 0x1,
            close_other_widget         = 0x2,
            close_all_widgets          = 0x4,
            open_widget                = 0x8,
            reload_self                = 0x10,
            reload_other_widget        = 0x20,
            give_focus_to_widget       = 0x40,
            run_function               = 0x80,
            replace_self_with_widget   = 0x100,
            go_back_to_previous_widget = 0x200,
            run_scenario_script        = 0x400,
            try_to_branch_on_failure   = 0x800,
        } flags;
        enum class type_t : u16
        {
            a_btn,
            b_btn,
            x_btn,
            y_btn,
            black_btn,
            white_btn,
            left_trigger,
            right_trigger,
            dpad_up,
            dpad_down,
            dpad_left,
            dpad_right,
            start_btn,
            back_btn,
            left_thumb,
            right_thumb,
            left_stick_up,
            left_stick_down,
            left_stick_left,
            left_stick_right,
            right_stick_up,
            right_stick_down,
            right_stick_left,
            right_stick_right,
            created,
            deleted,
            get_focus,
            lose_focus,
            left_mouse,
            middle_mouse,
            right_mouse,
            double_click,
            custom_activator,
            post_render,
        } event_type;
        function_t                        function;
        tagref_typed_t<tag_class_t::DeLa> widget;
        tagref_typed_t<tag_class_t::snd>  sound;
        bl_string                         script;
    };

    reference<event_handler_t> event_handlers;

    /* Guerilla notes:
     * These are used to run a search-and-replace on the specified word in the text-box text
     * replacing all occurences of the word with the output of the replace function
     * These are invoked each time the text box is rendered (after any game data input
     * functions have been run). The searching is case-sensitive
     */
    struct search_and_replace_t
    {
        bl_string query;
        enum class replacement_t : u16
        {
            none,
            widget_controller,
            build_number,
            pid,
        } replacement;
    };

    u32 unknown_data[10];

    u32 padding_0[25];

    /* Guerilla notes:
     * Parameters specific to text box widgets
     * NOTE: The string list tag can also be used for lists whose items come
     * from a string list tag
     */
    struct text_box_t
    {
        tagref_typed_t<tag_class_t::ustr> unicode_strings;
        tagref_typed_t<tag_class_t::font> font;
        // Based on inspection, color is in ARGB format
        Vecf4                             color;
        enum class justification_t : u16
        {
            left,
            right,
            center,
        } justification;
        enum class flags_t : u32
        {
            editable   = 0x1,
            password   = 0x2,
            flashing   = 0x4,
            dont_focus = 0x8,
        } flags [[gnu::packed]]; /* on disk directly after justification */
        u16 unknown1[6];

        // More text box parameters

        i16 string_list_index;
        i16 horizontal_offset;
        i16 vertical_offset;
        u16 padding;

        Vecf4 remapped_color() const
        {
            return Vecf4{color.g, color.b, color.a, color.r};
        }
    } text_box;
    
    u32 padding_1[6];

    /* Missing list items, conditional widgets, column list, spinner list */

    /* Guerilla notes:
     * These options affect list items for both spinner and column lists
     * * child widgets are used to define the visible list items
     * * for lists with code-generated list items, the child widgets are used
     *   as templated for visible item placement
     * IMPORTANT: for list widgets, the ONLY thing you can have as child widgets
     * are the list item widgets!
     */
    struct list_items_t
    {
        enum flags_t : u32
        {
            none                          = 0x0,
            list_items_generated_in_code  = 0x1,
            list_items_from_list_tag      = 0x2,
            list_items_only_one_tooltip   = 0x4,
            list_single_preview_no_scroll = 0x8,
        } flags;
    } list_items; // Currently not located

    /* Guerilla notes:
     * Parameters specific to spinner list widgets
     * Child widgets are the list items
     */
    struct spinner_list_t
    {
        tagref_typed_t<tag_class_t::bitm> list_header_bitmap;
        tagref_typed_t<tag_class_t::bitm> list_footer_bitmap;
        vec4i16                           header_bounds;
        vec4i16                           footer_bounds;
    } spinner_list;

    u32 padding_2[8];

    /* Guerilla notes:
     * Parameters specific to column list widgets
     * Child widgets are the list items
     */
    struct column_list_t
    {
        tagref_typed_t<tag_class_t::ui_element> extended_description_widget;
    } column_list;

    u32 padding_3[72];

    /* Guerilla notes:
     * Use this to attach widgets that are loaded only if some internal
     * criteria is met while processing a widget event
     */
    struct conditional_widget_t
    {
        tagref_typed_t<tag_class_t::ui_element> widget_tag;
        bl_string name; /* unused */
        enum flags_t : u32
        {
            load_if_event_handler_function_fails = 0x1,
        } flags;
        u32 custom_controller_index; /* unused */
    };
    reference<conditional_widget_t> conditional_widgets;

    u32 padding_4[64];

    /* Guerilla notes:
     * Use this to attach widgets thata are loaded as "children" of
     * this widget (children are always loaded as part of the parent widget)
     */
    struct child_widget_t
    {
        tagref_typed_t<tag_class_t::DeLa> widget;
        bl_string                         name;
        enum class flags_t : u32
        {
            use_custom_controller_index = 0x1,
        } flags;
        i16 custom_controller_index;
        i16 vertical_offset;
        i16 horizontal_offset;
        u32 unknown[5];
    };

    reference<child_widget_t> child_widgets;
};

C_FLAGS(ui_element::flags_t, u32);
C_FLAGS(ui_element::event_handler_t::flags_t, u32);
C_FLAGS(ui_element::text_box_t::flags_t, u32);
C_FLAGS(ui_element::child_widget_t::flags_t, u32);
static_assert(offsetof(ui_element::text_box_t, flags) == 50);
static_assert(offsetof(ui_element::text_box_t, string_list_index) == 66);
static_assert(offsetof(ui_element, text_box) == 236);
static_assert(offsetof(ui_element, spinner_list) == 340);
static_assert(offsetof(ui_element, column_list) == 420);
static_assert(offsetof(ui_element, conditional_widgets) == 724);
static_assert(offsetof(ui_element, child_widgets) == 992);
static_assert(sizeof(ui_element) == 1004);

struct ui_item_collection
{
    struct widget_definition_t
    {
        tagref_typed_t<tag_class_t::DeLa> definition;
    };

    reference<widget_definition_t> widget_definitions;
};

struct multiplayer_scenarios
{
    struct map_t
    {
        tagref_typed_t<tag_class_t::bitm> preview;
        tagref_typed_t<tag_class_t::ustr> name;
        bl_string                         directory_path;
        u32                               padding[4];
    };

    reference<map_t> maps;
};

/* D3DCOLOR, a little-endian 0xAARRGGBB: B, G, R, A in memory */
struct argb8_t
{
    u8 b, g, r, a;
};

struct unit_hud_interface
{
    struct multitex_effectors_t
    {
        u32 padding_0[16];

        /* Source/destination
         * These describe the relationship that causes the effect
         * * destination type is the type of variable you want to be affected
         * * destination tells which texture map (or geom offset) to apply it to
         * * source says which value to look at when computing the effect
         */
        enum class destination_type_t : u16
        {
            tint_01, // tint (0, 1)
            horizontal_offset,
            vertical_offset,
            fade_01, // fade (0, 1)
        } destination_type;
        enum class destination_t : u16
        {
            geometry_offset,
            primary_map,
            secondary_map,
            tertiary_map,
        } destination;
        enum class source_t : u16
        {
            player_pitch,
            player_pitch_tangent,
            player_yaw,
            weapon_ammo_total,
            weapon_ammo_loaded,
            weapon_heat,
            explicit_use_low_bound, // explicit (uses low bound)
            weapon_zoom_level,
        } source;
        u16 padding_1;

        /* In/out bounds
         * When the source is the lower inbound, the destination ends up
         * the lower outbound and vice-versa applies for the upper values
         */
        Vecf2 in_bounds;  // source units
        Vecf2 out_bounds; // pixels

        u32 padding_2[16];

        /* Tint color bounds
         * If destination is tint, these values are used instead of the out bounds
         */
        Vecf3 tint_color_lower_bound;
        Vecf3 tint_color_upper_bound;

        /* Periodic functions
         * If you use periodic function as the source, this lets you tweak it
         */
        enum class periodic_function_t : u16
        {
            one,
            zero,
            cosine,
            cosine_variable_period, // cosine (variable period)
            diagonal_wave,
            diagonal_wave_variable_period,
            slide,
            slide_variable_period,
            noise,
            jitter,
            wander,
            spark,
        } periodic_function;
        u16 padding_3;
        f32 function_period; // seconds
        f32 function_phase;  // seconds

        u32 padding_4[8];
    };

    struct multitex_overlay_t
    {
        u16 padding_0;
        i16 type; // ???
        enum class blend_func_t : u16
        {
            alpha_blend,
            multiply,
            double_multiply,
            add,
            subtract,
            component_min,
            component_max,
            alpha_multiply_add,
        } framebuffer_blend_func;
        u16 padding_1[17];

        /* Where you want the origin of the texture
         * "texture" uses the texture coordinates supplied (?)
         * "screen" uses the origin of the screen as the origin of the texture
         */
        enum class anchor_t : u16
        {
            texture,
            screen,
        };
        anchor_t primary_anchor;
        anchor_t secondary_anchor;
        anchor_t tertiary_anchor;

        /* How to blend the fextures together */
        enum class texture_blend_func_t : u16
        {
            add,
            subtract,
            multiply,
            multiply2x,
            dot,
        };
        texture_blend_func_t zero_to_one_blend;
        texture_blend_func_t one_to_two_blend;
        u16                  padding_2;

        /* How much to scale the textures */
        Vecf2 primary_scale;
        Vecf2 secondary_scale;
        Vecf2 tertiary_scale;

        /* How much to offset the origin of the texture */
        Vecf2 primary_offset;
        Vecf2 secondary_offset;
        Vecf2 tertiary_offset;

        /* Which maps to use */
        enum class wrap_mode_t : u16
        {
            clamp,
            wrap,
        };
        tagref_typed_t<tag_class_t::bitm> primary;
        tagref_typed_t<tag_class_t::bitm> secondary;
        tagref_typed_t<tag_class_t::bitm> tertiary;
        wrap_mode_t                       primary_wrap_mode;
        wrap_mode_t                       secondary_wrap_mode;
        wrap_mode_t                       tertiary_wrap_mode;
        u16                               padding_3;

        u32 padding_4[46];

        reference<multitex_effectors_t> effectors;

        u32 padding_5[32];
    };

    /* Placement shared by every HUD element */
    struct background_base_t
    {
        vec2i16 anchor_offset;
        f32     width_scale;
        f32     height_scale;
        enum class scaling_flags_t : u16
        {
            none               = 0x0,
            dont_scale_offset  = 0x1,
            dont_scale_size    = 0x2,
            use_high_res_scale = 0x4,
        } scaling_flags;
        u16 padding_0;
        u32 padding_1[5];
    };

    struct colors_t
    {
        argb8_t default_color;
        argb8_t flashing_color;
        f32     flash_period;
        f32     flash_delay;
        i16     num_flashes;
        enum class flash_flags_t : u16
        {
            none                            = 0x0,
            reverse_default_flashing_colors = 0x1,
        } flash_flags;
        f32     flash_length;
        argb8_t disabled_color;
        u32     padding_0;
    };

    struct background_t : background_base_t
    {
        tagref_typed_t<tag_class_t::bitm> interface_bitmap;
        colors_t                          colors;
        i16                               sequence_index;
        u16                               padding_2;
        reference<multitex_overlay_t>     overlays;
        u32                               padding_3;
    };

    struct overlay_t : background_t
    {
        enum class type_t : u16
        {
            team_icon,
        } type;
        enum class flags_t : u16
        {
            none           = 0x0,
            use_team_color = 0x1,
        } flags;
        u32 padding_4[6];
    };

    struct meter_base_t : background_base_t
    {
        tagref_typed_t<tag_class_t::bitm> meter_bitmap;
        argb8_t                           color_at_minimum;
        argb8_t                           color_at_maximum;
        argb8_t                           flash_color;
        argb8_t                           empty_color;
        enum class meter_flags_t : u8
        {
            none                              = 0x0,
            use_min_max_for_state_changes     = 0x1,
            interpolate_min_max_flash         = 0x2, // text cut off in Guerilla
            interpolate_color_along_hsv_space = 0x4,
            more_colors_for_hsv_interpolation = 0x8,
            invert_interpolation              = 0x10,
        } flags;
        u8      minimum_meter_value;
        i16     sequence_index;
        u8      alpha_multiplier;
        u8      alpha_bias;
        i16     value_scale;
        f32     opacity;
        f32     translucency;
        argb8_t disabled_color;
        u32     padding_2[4];
    };

    struct aux_hud_meter_t
    {
        enum class type_t : u16
        {
            integrated_light,
        } type;
        u16          padding_0;
        u32          padding_1[4];
        background_t background;
        meter_base_t meter;
        f32          minimum_fraction_cutoff;
        enum class flags_t : u32
        {
            none                                   = 0x0,
            show_only_when_active                  = 0x1,
            flash_once_if_activated_while_disabled = 0x2,
        } flags;
        u32 padding_2[22];
    };

    struct sound_t
    {
        tagref_t sound; // snd! or lsnd
        enum class latched_to_t : u32
        {
            none                = 0x0,
            shield_recharging   = 0x1,
            shield_damaged      = 0x2,
            shield_low          = 0x4,
            shield_empty        = 0x8,
            health_low          = 0x10,
            health_empty        = 0x20,
            health_minor_damage = 0x40,
            health_major_damage = 0x80,
        } latched_to;
        f32 scale;
        u32 padding_0[8];
    };

    enum class anchor_t : u16
    {
        top_left,
        top_right,
        bottom_left,
        bottom_right,
        center,
    } anchor;
    u16 padding_0;
    u32 padding_1[8];

    background_t unit_hud_background;
    background_t shield_panel_background;
    meter_base_t shield_panel_meter;
    argb8_t      overcharge_minimum_color;
    argb8_t      overcharge_maximum_color;
    argb8_t      overcharge_flash_color;
    argb8_t      overcharge_empty_color;
    u32          padding_2[4];
    background_t health_panel_background;
    meter_base_t health_panel_meter;
    argb8_t      medium_health_left_color;
    f32          max_color_health_fraction_cutoff;
    f32          min_color_health_fraction_cutoff;
    u32          padding_3[5];
    background_t motion_sensor_background;
    background_t motion_sensor_foreground;
    u32          padding_4[8];
    background_base_t motion_sensor_center;
    struct aux_overlays_t
    {
        anchor_t             anchor;
        u16                  padding_0;
        u32                  padding_1[8];
        reference<overlay_t> overlays;
        u32                  padding_2[4];
    } aux_overlays;
    reference<sound_t>         hud_warning_sounds;
    reference<aux_hud_meter_t> auxiliary_hud_meters;
    u32                        padding_5[101];
};

static_assert(sizeof(unit_hud_interface::multitex_effectors_t) == 220);
static_assert(sizeof(unit_hud_interface::multitex_overlay_t) == 480);
static_assert(sizeof(unit_hud_interface::background_base_t) == 36);
static_assert(sizeof(unit_hud_interface::colors_t) == 32);
static_assert(sizeof(unit_hud_interface::background_t) == 104);
static_assert(sizeof(unit_hud_interface::overlay_t) == 132);
static_assert(sizeof(unit_hud_interface::meter_base_t) == 104);
static_assert(sizeof(unit_hud_interface::aux_hud_meter_t) == 324);
static_assert(sizeof(unit_hud_interface::sound_t) == 56);
static_assert(sizeof(unit_hud_interface) == 1388);

struct grenade_hud_interface
{
    struct overlay_t : unit_hud_interface::background_base_t
    {
        unit_hud_interface::colors_t colors;
        i16                          frame_rate;
        u16                          padding_2;
        i16                          sequence_index;
        enum class type_t : u16
        {
            none             = 0x0,
            show_on_flashing = 0x1,
            show_on_empty    = 0x2,
            show_on_default  = 0x4,
            show_always      = 0x8,
        } type; // Possibly mislabeled in Guerilla
        enum class flags_t : u32
        {
            none                = 0x0,
            flashes_when_active = 0x1,
        } flags;
        u32 padding_3[14];
    };

    struct numbers_t : unit_hud_interface::background_base_t
    {
        unit_hud_interface::colors_t colors;
        u8                           maximum_number_digits;
        enum class flags_t : u8
        {
            none                  = 0x0,
            show_leading_zeros    = 0x1,
            only_show_when_zoomed = 0x2,
            draw_a_trailing_m     = 0x4, // ???
        } flags;
        u8  number_of_fractional_digits;
        u8  padding_2;
        u32 padding_3[3];
    };

    /* Some shipped tags carry junk in here (sequence_index 0x6269) */
    struct messaging_information_t
    {
        i16     sequence_index;
        i16     width_offset;
        vec2i16 offset_from_reference_corner;
        argb8_t override_icon_color;
        u8      frame_rate; // 0-30
        enum class flags_t : u8
        {
            none                                = 0x0,
            use_text_from_string_list_instead   = 0x1,
            override_default_color              = 0x2,
            width_offset_is_absolute_icon_width = 0x4,
        } flags;
        i16 text_index;
        u32 padding_0[12];
    };

    unit_hud_interface::anchor_t     anchor;
    u16                              padding_0;
    u32                              padding_1[8];
    unit_hud_interface::background_t grenade_hud_background;
    unit_hud_interface::background_t total_grenades_background;
    struct total_grenades_numbers_t : numbers_t
    {
        i16 flash_cutoff;
        u16 padding_4;
    } total_grenades_numbers;
    struct total_grenades_overlays_t
    {
        tagref_typed_t<tag_class_t::bitm> overlay_bitmap;
        reference<overlay_t>              overlays;
    } total_grenades_overlays;
    reference<unit_hud_interface::sound_t> warning_sounds;
    u32                                    padding_2[17];
    messaging_information_t                messaging_information;
};

static_assert(sizeof(grenade_hud_interface::overlay_t) == 136);
static_assert(sizeof(grenade_hud_interface::numbers_t) == 84);
static_assert(sizeof(grenade_hud_interface::messaging_information_t) == 64);
static_assert(sizeof(grenade_hud_interface) == 504);

struct weapon_hud_interface
{
    enum class attached_to_t : u16
    {
        total_ammo,
        loaded_ammo,
        heat,
        age,
        secondary_weapon_total_ammo,
        secondary_weapon_loaded_ammo,
        distance_to_target,
        elevation_to_target,
    };
    enum class use_on_map_type_t : u16
    {
        any,
        solo,
        multiplayer,
    };

    /* Common to static, meter, number and overlay elements */
    struct element_header_t
    {
        attached_to_t     state_attached_to;
        u16               padding_0;
        use_on_map_type_t can_use_on_map_type;
        u16               padding_1;
        u32               padding_2[7];
    };

    struct static_element_t : element_header_t
    {
        unit_hud_interface::background_t background;
        u32                              padding_3[10];
    };
    struct meter_element_t : element_header_t
    {
        unit_hud_interface::meter_base_t meter;
        u32                              padding_3[10];
    };
    struct number_element_t : element_header_t
    {
        struct number_t : grenade_hud_interface::numbers_t
        {
            enum class weapon_flags_t : u16
            {
                none                       = 0x0,
                divide_number_by_clip_size = 0x1,
            } weapon_flags;
            u16 padding_4;
            u32 padding_5[9];
        } number;
    };
    struct crosshair_overlay_t : unit_hud_interface::background_base_t
    {
        unit_hud_interface::colors_t colors;
        i16                          frame_rate;
        i16                          sequence_index;
        enum class flags_t : u32
        {
            none                      = 0x0,
            flashes_when_active       = 0x1,
            not_a_sprite              = 0x2,
            show_only_when_zoomed     = 0x4,
            show_sniper_data          = 0x8,
            hide_area_outside_reticle = 0x10,
            one_zoom_level            = 0x20,
            dont_show_when_zoomed     = 0x40,
        } flags;
        u32 padding_2[8];
    };
    struct crosshair_t
    {
        enum class crosshair_type_t : u16
        {
            aim,
            zoom,
            charge,
            should_reload,
            flash_heat,
            flash_total_ammo,
            flash_battery,
            reload_overheat,
            flash_when_firing_and_no_ammo,
            flash_when_throwing_and_no_grenades,
            low_ammo_and_none_left_to_reload,
            should_reload_secondary_trigger,
            flash_secondary_total_ammo,
            flash_secondary_reload,
            flash_when_firing_secondary_trigger,
            low_secondary_ammo_and_non, // text cut off in Guerilla
            primary_trigger_ready,
            secondary_trigger_ready,
            flash_when_firing_with_depleted,
        } crosshair_type;
        u16                               padding_0;
        use_on_map_type_t                 can_use_on_map_type;
        u16                               padding_1;
        u32                               padding_2[7];
        tagref_typed_t<tag_class_t::bitm> crosshair_bitmap;
        reference<crosshair_overlay_t>    crosshair_overlays;
        u32                               padding_3[10];
    };
    struct overlay_t : unit_hud_interface::background_base_t
    {
        unit_hud_interface::colors_t colors;
        i16                          frame_rate;
        u16                          padding_2;
        i16                          sequence_index;
        enum class type_t : u16
        {
            none                       = 0x0,
            show_on_flashing           = 0x1,
            show_on_empty              = 0x2,
            show_on_reload_overheating = 0x4,
            show_on_default            = 0x8,
            show_always                = 0x10,
        } type; // Possibly mislabeled in Guerilla
        enum class flags_t : u32
        {
            none                = 0x0,
            flashes_when_active = 0x1,
        } flags;
        u32 padding_3[14];
    };
    struct overlay_element_t : element_header_t
    {
        tagref_typed_t<tag_class_t::bitm> overlay_bitmap;
        reference<overlay_t>              overlays;
        u32                               padding_3[10];
    };
    struct screen_effect_t
    {
        enum class flags_t : u16
        {
            none                  = 0x0,
            only_when_zoomed      = 0x1,
            connect_to_flashlight = 0x2,
            // night vision: masked = 0x4
            // desaturation: additive = 0x4
            // desaturation: masked = 0x8
        };

        u32 padding_0;
        struct
        {
            flags_t                           flags;
            u16                               padding_0;
            u32                               padding_1[4];
            tagref_typed_t<tag_class_t::bitm> mask_fullscreen;
            tagref_typed_t<tag_class_t::bitm> mask_splitscreen;
            u32                               padding_2[2];
        } mask;
        struct
        {
            flags_t flags;
            u16     padding_0;
            Vecf2   fov_in_bounds; // radians
            Vecf2   radius_out_bounds;
            u32     padding_1[6];
        } convolution;
        struct
        {
            flags_t flags;
            i16     script_source; // [0, 3]
            f32     intensity;     // [0, 1]
            u32     padding_0[6];
        } night_vision;
        struct
        {
            flags_t flags;
            i16     script_source; // [0, 3]
            f32     intensity;     // [0, 1]
            Vecf3   tint;
            u32     padding_0[6];
        } desaturation;
    };

    tagref_typed_t<tag_class_t::wphi> child_hud;
    struct flash_cutoff_t
    {
        enum flags_t : u16
        {
            none                               = 0x0,
            use_parent_hud_flashing_parameters = 0x1,
        } flags;
        u16 padding_0;
        i16 total_ammo_cutoff;
        i16 loaded_ammo_cutoff;
        i16 heat_cutoff;
        i16 age_cutoff;
        u32 padding_1[8];
    } flash_cutoffs;
    struct screen_alignment_t
    {
        unit_hud_interface::anchor_t anchor;
        u16                          padding_0;
        u32                          padding_1[8];
        reference<static_element_t>  static_elements;
    } screen_alignment;
    reference<meter_element_t>   meter_elements;
    reference<number_element_t>  number_elements;
    reference<crosshair_t>       crosshairs;
    reference<overlay_element_t> overlay_elements;
    u32 crosshair_types; // 1 << crosshair_type for each of crosshairs
    u32 padding_0[3];
    reference<screen_effect_t>                     screen_effects;
    u32                                            padding_1[33];
    grenade_hud_interface::messaging_information_t messaging_information;
};

static_assert(sizeof(weapon_hud_interface::static_element_t) == 180);
static_assert(sizeof(weapon_hud_interface::meter_element_t) == 180);
static_assert(sizeof(weapon_hud_interface::number_element_t) == 160);
static_assert(sizeof(weapon_hud_interface::crosshair_overlay_t) == 108);
static_assert(sizeof(weapon_hud_interface::crosshair_t) == 104);
static_assert(sizeof(weapon_hud_interface::overlay_t) == 136);
static_assert(sizeof(weapon_hud_interface::overlay_element_t) == 104);
static_assert(sizeof(weapon_hud_interface::screen_effect_t) == 184);
static_assert(sizeof(weapon_hud_interface) == 380);

/* hudg; only what the shell UI uses so far. Offsets checked against the
 * Xbox ui.map. */
struct hud_globals
{
    /* What a "%a-button" style token in HUD and menu text draws */
    struct button_icon_t
    {
        i16     sequence_index; /*!< in icon_bitmap */
        i16     width_offset;
        vec2i16 offset;
        argb8_t color;
        i8      frame_rate;
        enum class flags_t : u8
        {
            use_text_from_string_list           = 0x1,
            override_default_color              = 0x2,
            width_offset_is_absolute_icon_width = 0x4,
        } flags;
        i16 text_index; /*!< in alternate_icon_text */
    };

    u8                                unknown_0[0x48];
    tagref_typed_t<tag_class_t::font> single_player_font;
    tagref_typed_t<tag_class_t::font> multi_player_font;
    u8                                unknown_1[0x94 - 0x68];
    tagref_typed_t<tag_class_t::ustr> item_message_text;
    tagref_typed_t<tag_class_t::bitm> icon_bitmap;
    tagref_typed_t<tag_class_t::ustr> alternate_icon_text;
    reference<button_icon_t>          button_icons; /*!< A, B, X, Y, ... */
};

static_assert(sizeof(hud_globals::button_icon_t) == 16);
static_assert(offsetof(hud_globals, icon_bitmap) == 0xa4);
static_assert(offsetof(hud_globals, button_icons) == 0xc4);

struct virtual_keyboard
{
    struct virtual_key_t
    {
        u16 key;

        // Key codes
        // Enter unicode character values as integer numbers
        u16 lowercase_character;
        u16 shift_character;
        u16 caps_character;
        u16 symbols_character;

        u16 shift_caps_character;
        u16 shift_symbols_character;
        u16 caps_symbol_character;

        tagref_typed_t<tag_class_t::bitm> unselected_bg;
        tagref_typed_t<tag_class_t::bitm> selected_bg;
        tagref_typed_t<tag_class_t::bitm> active_bg;
        tagref_typed_t<tag_class_t::bitm> sticky_bg;

        enum class action_t
        {
            none,
            done,
            backspace,
            left,
            right,
        };
        struct input_mode_t
        {
            bool shift{false};
            bool caps{false};
            bool symbols{false};
        };

        using token_t = std::tuple<char16_t, action_t, input_mode_t>;

        inline std::optional<token_t> tokenize(input_mode_t mode) const
        {
            // Range 0-9 is numbers 1-9 + 0
            // Range 10-35 is A-Z
            // After that is special tokens
            enum action_idx_t
            {
                last_number = 9,
                last_letter = 35,
                done,
                shift,
                caps_lock,
                symbols,
                backspace,
                left,
                right,
                space,
            };

            auto map_character = [this, &mode] {
                if(mode.shift && mode.caps)
                    return shift_caps_character;
                else if(mode.shift && mode.symbols)
                    return shift_symbols_character;
                else if(mode.caps && mode.symbols)
                    return caps_symbol_character;
                else if(mode.symbols)
                    return symbols_character;
                else if(mode.caps)
                    return caps_character;
                else if(mode.shift)
                    return shift_character;
                else
                    return lowercase_character;
            };

            if(key >= 0 && key <= last_number)
                return token_t{
                    map_character(),
                    action_t::none,
                    mode,
                };
            else if(key > last_number && key <= last_letter)
                return token_t{
                    map_character(),
                    action_t::none,
                    mode,
                };
            else
            {
                switch(key)
                {
                case done:
                    return token_t{0, action_t::done, mode};
                case shift:
                    return token_t{0, action_t::none, input_mode_t{
                            .shift   = !mode.shift,
                            .caps    = mode.caps,
                            .symbols = mode.symbols,
                        },
                    };
                case caps_lock:
                    return token_t{0, action_t::none, input_mode_t{
                            .shift   = mode.shift,
                            .caps    = !mode.caps,
                            .symbols = mode.symbols,
                        },
                    };
                case symbols:
                    return token_t{0, action_t::none, input_mode_t{
                            .shift   = mode.shift,
                            .caps    = mode.caps,
                            .symbols = !mode.symbols,
                        },
                    };
                case backspace:
                    return token_t{0, action_t::backspace, mode};
                case left:
                    return token_t{0, action_t::left, mode};
                case right:
                    return token_t{0, action_t::right, mode};
                case space:
                    return token_t{' ', action_t::none, mode};
                default:
                    return std::nullopt;
                }
            }
        }
    };

    tagref_typed_t<tag_class_t::font> display_font;
    tagref_typed_t<tag_class_t::bitm> background;
    tagref_typed_t<tag_class_t::ustr> special_key_labels_string_list;
    reference<virtual_key_t> virtual_keys;
};

static_assert(sizeof(virtual_keyboard::virtual_key_t) == 80);
static_assert(sizeof(virtual_keyboard) == 60);

} // namespace blam
