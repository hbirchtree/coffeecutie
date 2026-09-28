#pragma once

#include "blam_base_types.h"
#include "blam_reference.h"
#include "blam_tag_ref.h"

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

    using function_t = u16;

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

struct unit_hud_interface
{
    struct multitex_effectors_t
    {
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

        /* In/out bounds
         * When the source is the lower inbound, the destination ends up
         * the lower outbound and vice-versa applies for the upper values
         */
        Vecf2 in_bounds; // source units
        Vecf2 out_bounds; // pixels

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
        f32 function_period; // seconds
        f32 function_phase; // seconds
    };

    struct multitex_overlay_t
    {
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

        reference<multitex_effectors_t> effectors;
    };

    struct background_t
    {
        vec2i16 anchor_offset;
        i16 width_scale;
        i16 height_scale;
        enum class scaling_flags_t
        {
            none               = 0x0,
            dont_scale_offset  = 0x1,
            dont_scale_size    = 0x2,
            use_high_res_scale = 0x4,
        } scaling_flags;
        tagref_typed_t<tag_class_t::bitm> interface_bitmap;
        Vecf4 default_color; // In ARGB
        Vecf4 flashing_color; // In ARGB
        f32 flash_period;
        f32 flash_delay;
        f32 num_flashes;
        enum class flash_flags_t
        {
            none                            = 0x0,
            reverse_default_flashing_colors = 0x1,
        } flash_flags;
        f32 flash_length;
        Vecf4 disabled_color; // In ARGB
        i16 sequence_index;

        reference<multitex_overlay_t> overlays;
    };

    struct overlay_t : background_t
    {
        enum class type_t : u16
        {
            team_icon,
        } type;
        enum class overlay_type_t : u16
        {
            none           = 0x0,
            use_team_color = 0x1,
        } flags;
    };

    struct meter_t
    {
        vec2i16 anchor_offset;
        i16 width_scale;
        i16 height_scale;
        background_t::scaling_flags_t scaling_flags;
        tagref_typed_t<tag_class_t::bitm> meter_bitmap;
        Vecf3 color_at_minimum;
        Vecf3 color_at_maximum;
        Vecf3 flash_color;
        Vecf4 empty_color; // In ARGB
        enum class meter_flags_t : u16
        {
            none = 0x0,
            use_min_max_for_state_changes     = 0x1,
            interpolate_min_max_flash         = 0x2, // text cut off in Guerilla
            interpolate_color_along_hsv_space = 0x4,
            more_colors_for_hsv_interpolation = 0x8,
            invert_interpolation              = 0x10,
        } flags;
        f32 mininum_meter_value;
        i16 sequence_index;
        f32 alpha_multiplier;
        f32 alpha_bias;
        f32 value_scale;
        f32 opacity;
        f32 translucency;
        Vecf4 disabled_color; // In ARGB
        f32 minimum_fraction_cutoff;
        enum class meter_flags2_t : u16
        {
            none = 0x0,
            show_only_when_active                  = 0x1,
            flash_once_if_activated_while_disabled = 0x2,
        } flags2;
    };

    struct aux_hud_meter_t
    {
        enum class type_t : u16
        {
            integrated_light,
        } type;
        background_t background;
        meter_t      meter;
    };

    struct sound_t
    {
        tagref_t sound; // snd or lsnd
        enum class latched_to_t : u16
        {
            none                = 0x0,
            shield_recharging   = 0x1,
            shield_damaged      = 0x2,
            shield_low          = 0x4,
            shield_empty        = 0x8,
            health_low          = 0x10,
            health_empty        = 0x20,
            health_minor_damage = 0x40,
            health_major_damage = 0x40,
        } latched_to;
        f32 scale;
    };

    enum class anchor_t
    {
        top_left,
        top_right,
        bottom_left,
        bottom_right,
        center,
    } anchor;

    background_t unit_hud_background;
    background_t shield_panel_background;
    meter_t      shield_panel_meter;
    background_t health_panel_background;
    meter_t      health_panel_meter;
    background_t motion_sensor_background;
    // motion sensor foreground
    // motion sensor center
    struct aux_overlays_t
    {
        anchor_t             anchor;
        reference<overlay_t> overlays;
    } aux_overlays;
    // auxiliary overlays
    reference<sound_t>         hud_warning_sounds;
    reference<aux_hud_meter_t> auxiliary_hud_meters;
};

} // namespace blam
