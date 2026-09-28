#pragma once

#include "blam_base_types.h"
#include "blam_reference.h"
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

} // namespace blam
