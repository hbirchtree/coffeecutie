#pragma once

#include "blam_base_types.h"
#include "blam_reflexive.h"
#include "blam_tag_ref.h"

namespace blam {

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
    bl_string                        name;
    typing::vectors::tvector<i16, 4> bounds;
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
        // so much stuff...
    };
    struct data_input_t
    {
        function_t function;
        u32        unknown[8];
    };
    reflexive_t<data_input_t> data_inputs;

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
        enum class event_type_t : u16
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
    reflexive_t<event_handler_t> event_handlers;
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
    struct text_box_t
    {
        tagref_typed_t<tag_class_t::ustr> unicode_strings;
        tagref_typed_t<tag_class_t::font> font;
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
        } flags;
        u32 unknown1[3];
        i16 string_list_index;
        i16 horizontal_offset;
        i16 vertical_offset;
        u32 unknown2[6];
        i16 unknown3;
    } text_box;
    /* Missing list items, conditional widgets, column list, spinner list */
    u32 padding[188];
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
    reflexive_t<child_widget_t> child_widgets;
};

static_assert(offsetof(ui_element, child_widgets) == 992);
// static_assert(sizeof(ui_element) == 1004);

struct ui_item_collection
{
    struct widget_definition_t
    {
        tagref_typed_t<tag_class_t::DeLa> definition;
    };
    reflexive_t<widget_definition_t> widget_definitions;
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
    reflexive_t<map_t> maps;
};

} // namespace blam
