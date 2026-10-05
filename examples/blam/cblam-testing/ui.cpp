#include "ui.h"

#include <cctype>
#include <set>

#include "coffee/core/types/input/event_types.h"
#include "components.h"
#include "data.h"
#include "graphics_api.h"
#include "shader_compiler.h"
#include "ui_caching.h"
#include "ui_data.h"
#include "ui_keyboard.h"

#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/enumerate.h>

using semantic::RSCA;

/* One open menu, owned by a seat (or any seat, for the main menu) */
struct UIScreen
{
    using value_type = UIScreen;
    using type       = compo::alloc::VectorContainer<value_type>;
    using tag_type   = type;

    static constexpr u32 any_seat = ~0u;

    struct frame_t
    {
        generation_idx_t widget;
        /* Child index per level, root first. Kept per frame so going back
         * restores the previous selection. */
        std::vector<u16> focus{};
        /* Text entry the engine overlays on name screens */
        std::optional<VirtualKeyboard> keyboard{};
        /* replace_self_with_widget on a child swaps only that slot; slots
         * are child indices from the root. Quadrants share one tag, so this
         * can't live in the cache. */
        std::vector<std::pair<std::vector<u16>, generation_idx_t>> replaced{};

        generation_idx_t at(
            std::vector<u16> const& slot, generation_idx_t original) const
        {
            for(auto const& [where, widget] : replaced)
                if(where == slot)
                    return widget;
            return original;
        }

        void replace(std::vector<u16> const& slot, generation_idx_t widget)
        {
            for(auto& [where, current] : replaced)
                if(where == slot)
                {
                    current = widget;
                    return;
                }
            replaced.emplace_back(slot, widget);
        }
    };

    u32                  seat{any_seat};
    blam::vec4i16        box{0, 0, 640, 480};
    generation_idx_t     home; /*!< shown by UINavigation::open */
    std::vector<frame_t> stack; /*!< back() is on screen; empty = closed */

    /* Selected value of each spinner_list, until settings back them */
    std::vector<std::pair<blam::ui_element const*, u16>> spinner_values;

    bool accepts(u32 seat_idx) const
    {
        return seat == any_seat || seat == seat_idx;
    }

    u16& spinner_value(blam::ui_element const* spinner)
    {
        for(auto& [widget, value] : spinner_values)
            if(widget == spinner)
                return value;
        return spinner_values.emplace_back(spinner, 0).second;
    }

    u16 spinner_value(blam::ui_element const* spinner) const
    {
        for(auto const& [widget, value] : spinner_values)
            if(widget == spinner)
                return value;
        return 0;
    }
};

using UIRendererManifest = compo::SubsystemManifest<
    type_list_t<UIScreen>,
    type_list_t<
        gfx::system,
        RenderingParameters,
        UIEventBus,
        UIDataSource>,
    type_list_t<
        comp_app::DisplayInfo,
        comp_app::GraphicsFramebuffer,
        comp_app::MouseInput,
        comp_app::BasicEventBus<Coffee::Input::CIEvent>>>;

struct UIRenderer : compo::RestrictedSubsystem<UIRenderer, UIRendererManifest>
{
    using type  = UIRenderer;
    using Proxy = compo::proxy_of<UIRendererManifest>;

    struct vertex_t
    {
        Vecf2 position;
        Vecf2 tex_coord;
    };

    struct instance_vertex_t
    {
        Vecf4                        color;
        Vecf4                        tex_scale_offset{};
        typing::vector_types::Vecui4 texture_source{};
        u32                          padding[4] = {};
    };

    struct atlas_intermediate_t
    {
        Vecf2 atlas_offset;
        Vecf2 atlas_scale;
        u32   layer;
    };

    UIRenderer(
        gfx::api&                     api,
        UIElementCache<halo_version>& ui_cache,
        BitmapCache<halo_version>&    bitm_cache,
        FontCache<halo_version>&      font_cache)
        : api(api)
        , ui_cache(ui_cache)
        , bitm_cache(bitm_cache)
        , font_cache(font_cache)
    {
        using namespace std::string_view_literals;

        priority = 850;

        create_shaders(
            api,
            std::array<shader_pair_t, 1>{{
                {
                    .vertex_file   = "ui"sv,
                    .fragment_file = "ui"sv,
                    .shader        = ui_painter,
                },
            }});
        vertices = api.alloc_buffer(
            gfx::buffers::vertex, RSCA::WriteOnly | RSCA::Streaming);
        instance_vertices = api.alloc_buffer(
            gfx::buffers::constants, RSCA::WriteOnly | RSCA::Streaming);
        array = api.alloc_vertex_array();

        vertices->alloc();
        instance_vertices->alloc();
        array->alloc();

        array->add(gfx::vertex_attribute::from_member(&vertex_t::position));
        array->add(
            gfx::vertex_attribute::from_member(&vertex_t::tex_coord).at(1));
        array->set_buffer(gfx::buffers::vertex, vertices, 0);
    }

    gfx::api&                     api;
    UIElementCache<halo_version>& ui_cache;
    BitmapCache<halo_version>&    bitm_cache;
    FontCache<halo_version>&      font_cache;

    std::shared_ptr<gfx::program_t>      ui_painter;
    std::shared_ptr<gfx::buffer_t>       vertices;
    std::shared_ptr<gfx::buffer_t>       instance_vertices;
    std::shared_ptr<gfx::vertex_array_t> array;

    Vecf2                           screen_size;
    Vecf2                           mouse_pos;
    Vecf2                           m_mouse_raw{};
    bool                            m_bus_subscribed{false};
    bool                            m_mouse_active{false};
    bool                            m_click_pending{false};
    CIMouseButtonEvent::MouseButton mouse_buttons{CIMouseButtonEvent::NoneBtn};
    generation_idx_t                m_cursor_bitmap;
    generation_idx_t                m_default_font; /*!< for text with none */
    VirtualKeyboardData             m_keyboard;

    /* hudg's button icons, drawn for "%a-button" style tokens in text */
    struct button_icon_t
    {
        generation_idx_t image;
        Vecf4            uv{};     /*!< sprite: left, top, right, bottom */
        Vecf2            size{};   /*!< in texels */
        f32              advance{};
        Vecf4            color{1, 1, 1, 1};
        std::u16string   text; /*!< drawn instead, for text-only entries */
    };
    std::vector<button_icon_t> m_button_icons;
    /* Tokens become private-use characters until they are drawn */
    static constexpr char16_t first_icon_char = 0xE000;

    /* Provider hooks; valid during end_restricted */
    UIDataSource*               m_data{nullptr};
    u32                         m_seat{};         /*!< seat of the event */
    u64                         m_screen_id{};    /*!< entity of its screen */
    std::optional<u16>          m_selected;       /*!< nearest list's focus */
    std::set<blam::ui_element::function_t> m_unprovided; /*!< functions logged once */

    /* A handler waiting on a provider's UIFunctionDone */
    struct pending_t
    {
        u64                                token;
        u64                                screen_id;
        UIElementItem const*               widget;
        blam::ui_element::event_handler_t const* handler;
        std::vector<u16>                   slot{};
    };
    std::vector<pending_t> m_pending;

    std::shared_ptr<UIEventBus::queue_type<UIFunctionDone>> m_done_queue;
    std::vector<UIFunctionDone>                              m_done;

    std::shared_ptr<UIEventBus::queue_type<UINavigation>> m_nav_queue;
    std::vector<UINavigation> m_nav_events; /*!< filled by m_nav_queue */

    struct widget_data_t
    {
        std::vector<vertex_t>&          vertex_data;
        std::vector<instance_vertex_t>& instance_data;
        blam::vec4i16                   box;
    };

    /* All in 640x480 UI space */
    struct layout_data_t
    {
        Vecf2 origin; /*!< the screen's root */
        Vecf2 frame;  /*!< child offsets summed down to this widget */
        /* Focus path below this widget; empty when it isn't on the path */
        gsl::span<u16 const> focus;
        /* Item the owning list has focused, for extended descriptions */
        std::optional<u32> index;
        /* Stack entry whose slot replacements apply, and this widget's slot */
        UIScreen::frame_t const* stack_frame{nullptr};
        std::vector<u16>         slot{};
        /* Generated list item this widget belongs to; item_root is the
         * template's own container, which keeps its normal background */
        UIDataSource::list_t const* list{nullptr};
        std::optional<u16>          item{};
        bool                        item_root{false};
    };

    Vecf2 window_to_ui(Vecf2 const& point)
    {
        // DO NOT TOUCH
        // A LOT OF TRIAL AND ERROR WENT INTO THIS EQUATION!11!!
        auto normalized = point / screen_size;
        f32  aspect     = screen_size.x / screen_size.y;
        auto x_offset   = (screen_size.x - 640) / (2.f * screen_size.x);
        auto x_scale    = 1.f - x_offset * 2.f;
        auto offset     = Vecf2(x_offset, 0);

        auto offset_normalized = normalized - offset;
        return offset_normalized * Vecf2(640 / x_scale, 480);
    }

    Vecf2 ui_to_screen(Vecf2 const& ui_point)
    {
        f32   x_offset   = (screen_size.x - 640) / (2.f * screen_size.x);
        f32   x_scale    = 1.f - x_offset * 2.f;
        Vecf2 normalized = ui_point / Vecf2(640.f / x_scale, 480.f);
        return (normalized + Vecf2(x_offset, 0.f)) * screen_size;
    }

    bool mouse_in_bounds(Vecf2 const& ui_min, Vecf2 const& ui_max)
    {
        Vecf2 smin = ui_to_screen(ui_min);
        Vecf2 smax = ui_to_screen(ui_max);
        return m_mouse_raw.x > smin.x && m_mouse_raw.x < smax.x &&
               m_mouse_raw.y > smin.y && m_mouse_raw.y < smax.y;
    }

    bool mouse_down(
        CIMouseButtonEvent::MouseButton button = CIMouseButtonEvent::LeftButton)
    {
        return mouse_buttons & button;
    }

    /* Walk the widget tree depth-first. Child offsets add up, a widget's
     * bounds only place itself.
     * visit(el, min, max, layout) -> bool: return false to skip children. */
    template<typename Fn>
    void traverse_widget(
        generation_idx_t const& item, layout_data_t const& layout, Fn&& visit)
    {
        UIElementItem& el = ui_cache.find(item)->second;
        if(!el.visible)
            return;

        auto& bounds = el.ui_element->bounds;
        // bounds stored [y1, x1, y2, x2] as [.x, .y, .z, .w]
        Vecf2 const min = layout.frame + Vecf2(bounds.y, bounds.x);
        Vecf2 const max = layout.frame + Vecf2(bounds.w, bounds.z);

        if(!visit(el, min, max, layout))
            return;

        std::optional<u16> focused;
        if(!layout.focus.empty())
            focused = layout.focus.front();

        /* Its bounds are screen-absolute, and its images follow the list's
         * focused item */
        if(el.extended_description.valid())
            traverse_widget(
                el.extended_description,
                layout_data_t{
                    .origin = layout.origin,
                    .frame  = layout.origin,
                    .index  = focused.value_or(0),
                },
                visit);

        if(el.children.empty())
            return;
        auto children_opt = el.ui_element->child_widgets.data(bitm_cache.magic);
        if(!children_opt.has_value())
            return;
        auto const children = children_opt.value();
        /* Generated items are centred on the selected one */
        auto const* list     = list_of(el);
        i32 const   first    = list ? static_cast<i32>(list->get()) -
                                    static_cast<i32>(el.children.size() / 2)
                                    : 0;
        for(auto const& [i, child] : stl_types::const_enumerate(el.children))
        {
            std::optional<u16> item = layout.item;
            if(list)
            {
                i32 const at = first + static_cast<i32>(i);
                if(at < 0 || at >= static_cast<i32>(list->count()))
                    continue;
                item = static_cast<u16>(at);
            }
            auto const& meta = children[i];
            auto        slot = layout.slot;
            slot.push_back(static_cast<u16>(i));
            auto const widget =
                layout.stack_frame ? layout.stack_frame->at(slot, child) : child;
            traverse_widget(
                widget,
                layout_data_t{
                    .origin = layout.origin,
                    .frame  = layout.frame + Vecf2(meta.horizontal_offset,
                                                   meta.vertical_offset),
                    .focus  = focused == i ? layout.focus.subspan(1)
                                           : gsl::span<u16 const>{},
                    .index       = layout.index,
                    .stack_frame = layout.stack_frame,
                    .slot        = std::move(slot),
                    .list        = list ? list : layout.list,
                    .item        = item,
                    .item_root   = list != nullptr,
                },
                visit);
        }
    }

    using eh_t = blam::ui_element::event_handler_t;

    UIElementItem* find_item(generation_idx_t const& id)
    {
        auto it = ui_cache.find(id);
        return it != ui_cache.end() ? &it->second : nullptr;
    }

    static bool tabs_through_children(UIElementItem const& el)
    {
        using flags_t = blam::ui_element::flags_t;
        return has_flag(el, flags_t::dpad_ud_tabs_through_children) ||
               has_flag(el, flags_t::dpad_lr_tabs_through_children) ||
               has_flag(el, flags_t::dpad_ud_tabs_through_items) ||
               has_flag(el, flags_t::dpad_lr_tabs_through_items);
    }

    static bool has_flag(UIElementItem const& el, blam::ui_element::flags_t f)
    {
        return static_cast<u32>(el.ui_element->flags) & static_cast<u32>(f);
    }

    /* The provider's list, for a widget whose items are generated in code */
    UIDataSource::list_t const* list_of(UIElementItem const& el) const
    {
        using items_t = blam::ui_element::list_items_t;
        if(!m_data || !(el.ui_element->list_items.flags &
                        items_t::list_items_generated_in_code))
            return nullptr;
        auto inputs = el.ui_element->data_inputs.data(bitm_cache.magic);
        if(!inputs.has_value())
            return nullptr;
        for(auto const& input : inputs.value())
            if(auto const* list = m_data->list(input.function))
                return list;
        return nullptr;
    }

    /* A column list's items take focus with no handlers of their own */
    bool focusable_child(UIElementItem const& parent, UIElementItem const& child)
    {
        using flags_t = blam::ui_element::flags_t;
        if(child.visible &&
           parent.ui_element->widget_type ==
               blam::ui_element::widget_type_t::column_list &&
           (has_flag(parent, flags_t::dpad_ud_tabs_through_items) ||
            has_flag(parent, flags_t::dpad_lr_tabs_through_items)))
            return true;
        return focusable(child);
    }

    /* Can the widget, or anything below it, take input? */
    bool focusable(UIElementItem const& el)
    {
        using widget_type = blam::ui_element::widget_type_t;

        if(!el.visible)
            return false;
        /* Takes left/right itself, with no handlers or children */
        if(el.ui_element->widget_type == widget_type::spinner_list)
            return true;
        auto handlers = el.ui_element->event_handlers.data(bitm_cache.magic);
        if(handlers.has_value() && handlers.value().size() > 0)
            return true;
        for(auto const& child : el.children)
            if(auto* item = find_item(child); item && focusable(*item))
                return true;
        return false;
    }

    /* First focusable child at or after `from`, stepping and wrapping */
    std::optional<u16> next_focusable(
        UIElementItem const& el, i32 from, i32 step)
    {
        i32 const count = static_cast<i32>(el.children.size());
        for(i32 n = 0; n < count; ++n)
        {
            i32 idx = ((from + step * n) % count + count) % count;
            if(auto* child = find_item(el.children[idx]);
               child && focusable_child(el, *child))
                return static_cast<u16>(idx);
        }
        return std::nullopt;
    }

    /* Walk the frame's focus path from the root, re-picking indices that
     * are out of range or point at something that can't take focus */
    std::vector<UIElementItem*> resolve_focus(UIScreen::frame_t& frame)
    {
        std::vector<UIElementItem*> path;
        for(auto* el = find_item(frame.widget); el;)
        {
            path.push_back(el);
            auto const depth = path.size() - 1;
            if(frame.focus.size() <= depth)
                frame.focus.resize(depth + 1, u16(~0));
            auto& idx     = frame.focus[depth];
            auto* current = idx < el->children.size()
                                ? find_item(el->children[idx])
                                : nullptr;
            if(!current || !focusable_child(*el, *current))
            {
                auto first = next_focusable(*el, 0, 1);
                if(!first)
                    break;
                idx = *first;
            }
            std::vector<u16> const slot(
                frame.focus.begin(), frame.focus.begin() + depth + 1);
            el = find_item(frame.at(slot, el->children[idx]));
        }
        frame.focus.resize(path.empty() ? 0 : path.size() - 1);
        return path;
    }

    static eh_t::type_t handler_type(UINavigation::action_t action)
    {
        using t = eh_t::type_t;
        switch(action)
        {
        case UINavigation::accept:
            return t::a_btn;
        case UINavigation::option:
            return t::y_btn;
        case UINavigation::option_2:
            return t::x_btn;
        case UINavigation::back:
            return t::b_btn;
        case UINavigation::open:
        case UINavigation::close:
            break;
        case UINavigation::up:
            return t::dpad_up;
        case UINavigation::down:
            return t::dpad_down;
        case UINavigation::left:
            return t::dpad_left;
        case UINavigation::right:
            return t::dpad_right;
        }
        return t::a_btn;
    }

    /* Returns true if the widget has a handler for the event */
    bool run_handlers(
        UIScreen&               screen,
        UIElementItem const&    el,
        eh_t::type_t            type,
        std::vector<u16> const& slot = {})
    {
        auto handlers = el.ui_element->event_handlers.data(bitm_cache.magic);
        if(!handlers.has_value())
            return false;

        bool handled = false;
        for(auto const& eh : handlers.value())
        {
            /* PC tags put confirm on the left mouse button, or on a
             * list's activator */
            bool matches = eh.event_type == type ||
                           (type == eh_t::type_t::a_btn &&
                            (eh.event_type == eh_t::type_t::left_mouse ||
                             eh.event_type == eh_t::type_t::custom_activator));
            if(!matches)
                continue;

            auto flags = static_cast<u32>(eh.flags);
            if(flags == 0)
                continue;
            handled = true;

            auto const result = run_function(el, eh, type);
            if(result == ui_result_t::pending)
                m_pending.back().slot = slot;
            else
                finish_handler(screen, el, eh, result == ui_result_t::ok, slot);
            if(screen.stack.empty())
                break;
        }
        return handled;
    }

    /* A provider's binding wins over the screen's own copy */
    u16 spinner_get(UIScreen const* screen, UIElementItem const& el) const
    {
        if(auto const* list = list_of(el))
            return list->get();
        if(m_data)
            if(auto const* value = m_data->value(el.tag_name))
                return value->get();
        return screen ? screen->spinner_value(el.ui_element) : 0;
    }

    void spinner_set(UIScreen& screen, UIElementItem const& el, u16 value)
    {
        if(auto const* list = list_of(el))
        {
            list->set(value);
            return;
        }
        if(m_data)
            if(auto const* bound = m_data->value(el.tag_name))
            {
                bound->set(value);
                return;
            }
        screen.spinner_value(el.ui_element) = value;
    }

    /* Sends the event to the child whose custom controller index is the
     * seat, or to every child that names none */
    bool dispatch_to_controller(
        UIScreen&                screen,
        UIScreen::frame_t const& frame,
        UIElementItem const&     el,
        std::vector<u16> const&  slot,
        UINavigation const&      nav)
    {
        using child_flags_t = blam::ui_element::child_widget_t::flags_t;

        auto children = el.ui_element->child_widgets.data(bitm_cache.magic);
        if(!children.has_value())
            return false;
        bool handled = false;
        for(auto const& [i, meta] : stl_types::const_enumerate(children.value()))
        {
            if(i >= el.children.size())
                break;
            bool const own = static_cast<u32>(meta.flags) &
                             static_cast<u32>(
                                 child_flags_t::use_custom_controller_index);
            if(own && static_cast<u32>(meta.custom_controller_index) !=
                          nav.seat_idx)
                continue;
            auto child_slot = slot;
            child_slot.push_back(static_cast<u16>(i));
            auto* child = find_item(frame.at(child_slot, el.children[i]));
            if(!child)
                continue;
            /* The child's own focus path is not tracked; its handlers are
             * on the quadrant widgets themselves */
            handled |= run_handlers(
                screen, *child, handler_type(nav.action), child_slot);
            if(screen.stack.empty())
                break;
        }
        return handled;
    }

    static bool opens_keyboard(blam::ui_element::function_t function)
    {
        using func_t = blam::ui_element::function_t;
        return function == func_t::mp_profile_change_name ||
               function == func_t::player_profile_change_name;
    }

    /* The provider runs before the handler opens or closes anything, since
     * it often sets up what the next widget shows */
    ui_result_t run_function(
        UIElementItem const& el, eh_t const& eh, eh_t::type_t type)
    {
        if(!(static_cast<u32>(eh.flags) &
             static_cast<u32>(eh_t::flags_t::run_function)))
            return ui_result_t::ok;
        /* These get the text once the keyboard is done */
        if(opens_keyboard(eh.function) && m_keyboard.valid())
            return ui_result_t::ok;
        if(!m_data)
            return ui_result_t::ok;

        UIFunctionCall const call{
            .function = eh.function,
            .event    = type,
            .seat     = m_seat,
            .widget   = el.ui_element,
            .selected = m_selected,
            .token    = m_data->next_token(),
        };
        auto result = m_data->call(call);
        if(!result)
        {
            if(m_unprovided.insert(eh.function).second)
                cDebug(
                    "UI: no provider for function {} ({})",
                    magic_enum::enum_name(eh.function),
                    el.tag_name);
            return ui_result_t::ok;
        }
        if(*result == ui_result_t::pending)
            m_pending.push_back({call.token, m_screen_id, &el, &eh});
        return *result;
    }

    /* Everything a handler does after its function */
    void finish_handler(
        UIScreen&               screen,
        UIElementItem const&    el,
        eh_t const&             eh,
        bool                    ok,
        std::vector<u16> const& slot = {})
    {
        auto flags = static_cast<u32>(eh.flags);
        auto has   = [flags](eh_t::flags_t f) {
            return (flags & static_cast<u32>(f)) != 0;
        };

        if(!ok)
        {
            /* The widget's conditional widgets are the failure branch */
            using cond_t = blam::ui_element::conditional_widget_t;
            if(!has(eh_t::flags_t::try_to_branch_on_failure))
                return;
            auto conditionals =
                el.ui_element->conditional_widgets.data(bitm_cache.magic);
            if(!conditionals.has_value())
                return;
            for(cond_t const& cond : conditionals.value())
                if(cond.flags & cond_t::load_if_event_handler_function_fails)
                    if(auto widget = ui_cache.predict(cond.widget_tag);
                       widget.valid())
                    {
                        open_widget(screen, widget, false);
                        return;
                    }
            return;
        }

        if(has(eh_t::flags_t::close_all_widgets))
            screen.stack.clear();
        else if(has(eh_t::flags_t::close_current_widget) &&
                !screen.stack.empty())
            screen.stack.pop_back();
        else if(has(eh_t::flags_t::go_back_to_previous_widget))
            go_back(screen);

        if(has(eh_t::flags_t::open_widget) ||
           has(eh_t::flags_t::replace_self_with_widget))
        {
            auto widget = ui_cache.predict(eh.widget);
            if(!widget.valid())
                return;
            cDebug(
                "open_widget: {}",
                eh.widget.name.to_string(bitm_cache.magic));
            /* A child replaces only its own slot, e.g. one player's
             * quadrant; a top-level widget replaces the whole entry */
            if(has(eh_t::flags_t::replace_self_with_widget) && !slot.empty() &&
               !screen.stack.empty())
            {
                screen.stack.back().replace(slot, widget);
                fire_created(screen, widget, slot);
                return;
            }
            open_widget(
                screen,
                widget,
                has(eh_t::flags_t::replace_self_with_widget));

            /* The name screens are placeholders; the function that
             * opens them brings up the keyboard, prompt 9 for game
             * settings (41) and 8 for player profiles (66) */
            if(has(eh_t::flags_t::run_function) && m_keyboard.valid() &&
               opens_keyboard(eh.function))
            {
                auto& keyboard    = screen.stack.back().keyboard.emplace();
                keyboard.function = eh.function;
                keyboard.prompt   =
                    static_cast<u16>(
                        eh.function == blam::ui_element::function_t::mp_profile_change_name 
                            ? 9 
                            : 8);
            }
        }
    }

    void open_widget(UIScreen& screen, generation_idx_t widget, bool replace)
    {
        using flags_t = blam::ui_element::flags_t;

        /* A widget flagged dont_push_history is left out of the back stack */
        if(!screen.stack.empty())
            if(auto* top = find_item(screen.stack.back().widget);
               top && has_flag(*top, flags_t::dont_push_history))
                replace = true;

        if(replace && !screen.stack.empty())
            screen.stack.back() = {.widget = widget};
        else
            screen.stack.push_back({.widget = widget});
        fire_created(screen, widget);
    }

    /* A widget and everything below it runs its created handlers when it
     * appears, e.g. a lobby clearing its joins */
    void fire_created(
        UIScreen& screen, generation_idx_t widget, std::vector<u16> slot = {})
    {
        auto* el = find_item(widget);
        if(!el)
            return;
        run_handlers(screen, *el, eh_t::type_t::created, slot);
        for(auto const& [i, child] : stl_types::const_enumerate(el->children))
        {
            auto child_slot = slot;
            child_slot.push_back(static_cast<u16>(i));
            fire_created(screen, child, std::move(child_slot));
        }
    }

    void go_back(UIScreen& screen)
    {
        using flags_t = blam::ui_element::flags_t;

        if(screen.stack.size() > 1)
        {
            screen.stack.pop_back();
            return;
        }
        if(screen.stack.empty())
            return;
        auto* top = find_item(screen.stack.back().widget);
        if(top && has_flag(*top, flags_t::return_to_main_menu_if_no_history))
        {
            /* The main menu lives in ui.map, not in a game map */
            cDebug("UI: return to main menu requested, closing instead");
            screen.stack.clear();
        }
    }

    /* Events enter at the root and move down the focus path while the
     * widget passes unhandled events on, as the tag flags describe */
    void apply(UIScreen& screen, UINavigation const& nav)
    {
        using flags_t = blam::ui_element::flags_t;

        m_seat     = nav.seat_idx;
        m_selected = std::nullopt;

        if(nav.action == UINavigation::open)
        {
            if(screen.stack.empty() && screen.home.valid())
                open_widget(screen, screen.home, false);
            return;
        }
        if(nav.action == UINavigation::close)
        {
            screen.stack.clear();
            return;
        }
        if(screen.stack.empty())
            return;
        if(auto& keyboard = screen.stack.back().keyboard)
        {
            switch(nav.action)
            {
            case UINavigation::up:
                keyboard->move(0, -1);
                break;
            case UINavigation::down:
                keyboard->move(0, 1);
                break;
            case UINavigation::left:
                keyboard->move(-1, 0);
                break;
            case UINavigation::right:
                keyboard->move(1, 0);
                break;
            case UINavigation::accept: {
                if(!keyboard->press(m_keyboard))
                    break;
                cDebug(
                    "UI: entered name \"{}\"",
                    std::string(keyboard->text.begin(), keyboard->text.end()));
                /* A provider can reject the name and keep the keyboard up */
                std::optional<ui_result_t> result;
                if(m_data)
                    result = m_data->call(UIFunctionCall{
                        .function = keyboard->function,
                        .event    = eh_t::type_t::a_btn,
                        .seat     = m_seat,
                        .text     = keyboard->text,
                    });
                if(result == ui_result_t::failed)
                    break;
                go_back(screen);
                break;
            }
            case UINavigation::back:
                go_back(screen);
                break;
            default:
                break;
            }
            return;
        }
        auto& frame = screen.stack.back();
        auto  path  = resolve_focus(frame);

        /* The selection a provider sees: the nearest list's focused item */
        for(size_t depth = path.size() - 1; depth-- > 0;)
            if(tabs_through_children(*path[depth]))
            {
                m_selected = frame.focus[depth];
                break;
            }

        bool const vertical =
            nav.action == UINavigation::up || nav.action == UINavigation::down;
        bool const horizontal = nav.action == UINavigation::left ||
                                nav.action == UINavigation::right;
        i32 const  step =
            nav.action == UINavigation::up || nav.action == UINavigation::left
                 ? -1
                 : 1;

        for(size_t depth = 0; depth < path.size(); ++depth)
        {
            auto&                  el = *path[depth];
            std::vector<u16> const slot(
                frame.focus.begin(), frame.focus.begin() + depth);
            if(run_handlers(screen, el, handler_type(nav.action), slot))
                return;

            /* Children here belong to controllers, not to focus: each
             * player's quadrant takes that player's input */
            if(has_flag(el, flags_t::pass_unhandled_events_to_all_children))
            {
                dispatch_to_controller(screen, frame, el, slot, nav);
                return;
            }

            if(horizontal &&
               el.ui_element->widget_type ==
                   blam::ui_element::widget_type_t::spinner_list &&
               has_flag(el, flags_t::dpad_lr_tabs_through_items))
            {
                /* Clamped: the arrows mark the ends of the range */
                auto const* list = list_of(el);
                i32 const   last =
                    static_cast<i32>(std::max<size_t>(
                        list ? list->count() : el.text_strings.size(), 1)) -
                    1;
                spinner_set(
                    screen,
                    el,
                    static_cast<u16>(std::clamp(
                        static_cast<i32>(spinner_get(&screen, el)) + step,
                        0,
                        last)));
                return;
            }

            bool const tabs =
                (vertical &&
                 (has_flag(el, flags_t::dpad_ud_tabs_through_children) ||
                  has_flag(el, flags_t::dpad_ud_tabs_through_items))) ||
                (horizontal &&
                 (has_flag(el, flags_t::dpad_lr_tabs_through_children) ||
                  has_flag(el, flags_t::dpad_lr_tabs_through_items)));
            if(tabs && depth + 1 < path.size())
            {
                auto& idx  = frame.focus[depth];
                auto  next = next_focusable(el, idx + step, step);
                cDebug("Trying to switch to next element");
                if(next && *next != idx)
                {
                    cDebug("Switching element");
                    /* Focus handlers may change the stack, so `frame` is
                     * done with before they run */
                    auto* lost   = path[depth + 1];
                    auto* gained = find_item(el.children[*next]);
                    idx          = *next;
                    frame.focus.resize(depth + 1);
                    run_handlers(screen, *lost, eh_t::type_t::lose_focus);
                    if(gained && !screen.stack.empty())
                        run_handlers(screen, *gained, eh_t::type_t::get_focus);
                }
                return;
            }

            if(!has_flag(el, flags_t::pass_unhandled_events_to_focused_child))
                break;
        }

        /* B/back with no handler goes to the previous widget; a handler
         * replaces this, so tags that also want it set the flag */
        if(nav.action == UINavigation::back)
        {
            go_back(screen);
            return;
        }
        cDebug(
            "UI: unhandled {} for seat {}",
            magic_enum::enum_name(nav.action),
            nav.seat_idx);
    }

    /* One quad per glyph, appended straight into the frame's UI buffers --
     * for_each_glyph only owns where each one goes */
    void push_text(
        widget_data_t&      data,
        FontItem const&     font,
        std::u16string_view text,
        f32                 start_x,
        f32                 baseline_y,
        Vecf4 const&        color)
    {
        constexpr u32 kFontSource = 9u;
        u32 const     tex_source  = (kFontSource << 24) | font.atlas_layer;

        /* Runs of glyphs between inline button icons */
        f32 pen = start_x;
        while(!text.empty())
        {
            auto const icon_at = std::find_if(
                text.begin(), text.end(), [this](char16_t c) {
                    return icon_of(c) != nullptr;
                });
            auto const run =
                text.substr(0, static_cast<size_t>(icon_at - text.begin()));
            push_glyphs(data, font, run, pen, baseline_y, color, tex_source);
            pen += font.measure(run);
            text.remove_prefix(run.size());
            if(text.empty())
                break;

            auto const& icon = *icon_of(text.front());
            if(icon.image.valid())
            {
                /* Centred on the line, as the Xbox menus draw it */
                f32 const middle =
                    baseline_y - (static_cast<f32>(font.font->ascend_height) -
                                  font.font->descend_height) *
                                     0.5f;
                Vecf2 const top_left = Vecf2(pen, middle - icon.size.y * 0.5f);
                push_quad(
                    data,
                    top_left,
                    top_left + icon.size,
                    icon.image,
                    icon.uv,
                    Vecf3(icon.color));
            }
            pen += icon.advance;
            text.remove_prefix(1);
        }
    }

    button_icon_t const* icon_of(char16_t c) const
    {
        auto const idx = static_cast<size_t>(c - first_icon_char);
        return c >= first_icon_char && idx < m_button_icons.size()
                   ? &m_button_icons[idx]
                   : nullptr;
    }

    /* Width including inline icons */
    f32 text_width(FontItem const& font, std::u16string_view text) const
    {
        f32 width = 0.f;
        for(char16_t c : text)
            width += icon_of(c) ? icon_of(c)->advance
                                : font.measure(std::u16string_view(&c, 1));
        return width;
    }

    /* "%a-button" and friends become an icon character, or the icon's text */
    std::u16string expand_tokens(std::u16string_view text) const
    {
        static constexpr std::u16string_view names[] = {
            u"a-button",     u"b-button",      u"x-button",
            u"y-button",     u"black-button",  u"white-button",
            u"left-trigger", u"right-trigger", u"dpad-up",
            u"dpad-down",    u"dpad-left",     u"dpad-right",
            u"start-button", u"back-button",   u"left-thumb",
            u"right-thumb",  u"left-stick",    u"right-stick",
        };
        std::u16string out;
        out.reserve(text.size());
        for(size_t i = 0; i < text.size(); ++i)
        {
            bool matched = false;
            if(text[i] == u'%')
                for(size_t n = 0; n < std::size(names) && n < m_button_icons.size();
                    ++n)
                    if(text.substr(i + 1).starts_with(names[n]))
                    {
                        auto const& icon = m_button_icons[n];
                        if(icon.text.empty())
                            out.push_back(
                                static_cast<char16_t>(first_icon_char + n));
                        else
                            out += icon.text;
                        i += names[n].size();
                        matched = true;
                        break;
                    }
            if(!matched)
                out.push_back(text[i]);
        }
        return out;
    }

    void push_glyphs(
        widget_data_t&      data,
        FontItem const&     font,
        std::u16string_view text,
        f32                 start_x,
        f32                 baseline_y,
        Vecf4 const&        color,
        u32                 tex_source)
    {
        font.for_each_glyph(
            text, start_x, baseline_y, [&](GlyphEntry const& g, f32 gx, f32 gy) {
                f32 gx2 = gx + g.bitmap_width;
                f32 gy2 = gy + g.bitmap_height;

                std::array<vertex_t, 6> glyph_verts = {{
                    {{gx, gy}, {0, 0}},
                    {{gx2, gy}, {1, 0}},
                    {{gx2, gy2}, {1, 1}},
                    {{gx, gy}, {0, 0}},
                    {{gx2, gy2}, {1, 1}},
                    {{gx, gy2}, {0, 1}},
                }};
                data.vertex_data.insert(
                    data.vertex_data.end(), glyph_verts.begin(), glyph_verts.end());

                instance_vertex_t inst{};
                inst.color            = color;
                inst.tex_scale_offset = font.glyph_uv(g);
                inst.texture_source.x = tex_source;
                data.instance_data.push_back(inst);
            });
    }

    Vecf2 image_size(generation_idx_t const& im)
    {
        atlas_intermediate_t tmp{};
        auto const*          img = bitm_cache.assign_atlas_data(tmp, im)->image.mip;
        return Vecf2(img->isize.x, img->isize.y);
    }

    /* One textured quad; the bitmap is stretched to the rect. `content` is
     * the used part of a padded image, in texels from its top-left. */
    void push_image(
        widget_data_t&          data,
        Vecf2                   min,
        Vecf2                   max,
        generation_idx_t const& im,
        std::optional<Vecf2>    content = std::nullopt)
    {
        Vecf2 const isize   = image_size(im);
        Vecf2 const imscale = content ? *content / isize
                                      : glm::min((max - min) / isize, Vecf2(1.f));
        push_quad(data, min, max, im, Vecf4(0, 0, imscale.x, imscale.y));
    }

    /* A quad sampling `uv` (left, top, right, bottom) of an image, tinted */
    void push_quad(
        widget_data_t&          data,
        Vecf2                   min,
        Vecf2                   max,
        generation_idx_t const& im,
        Vecf4                   uv,
        Vecf3                   tint = Vecf3(1.f))
    {
        std::array<vertex_t, 6> verts = {{
            {.position = {min.x, min.y}, .tex_coord = {0, 0}},
            {.position = {max.x, min.y}, .tex_coord = {1, 0}},
            {.position = {max.x, max.y}, .tex_coord = {1, 1}},
            {.position = {min.x, min.y}, .tex_coord = {0, 0}},
            {.position = {max.x, max.y}, .tex_coord = {1, 1}},
            {.position = {min.x, max.y}, .tex_coord = {0, 1}},
        }};
        data.vertex_data.insert(
            data.vertex_data.end(), verts.begin(), verts.end());

        atlas_intermediate_t tmp{};
        bitm_cache.assign_atlas_data(tmp, im);
        instance_vertex_t inst{.color = Vecf4(tint, 0.f)};
        inst.tex_scale_offset = Vecf4(
            tmp.atlas_scale * Vecf2(uv.z - uv.x, uv.w - uv.y),
            tmp.atlas_offset + tmp.atlas_scale * Vecf2(uv.x, uv.y));
        inst.texture_source.x = tmp.layer;
        data.instance_data.push_back(inst);
    }

    void draw_keyboard(widget_data_t data, VirtualKeyboard const& keyboard)
    {
        using layout = VirtualKeyboardLayout;

        auto font_it = font_cache.find(m_keyboard.font_id);
        if(font_it == font_cache.end() || font_it->second.glyph_map.empty())
            return;
        FontItem const& font   = font_it->second;
        Vecf2 const     origin = Vecf2(data.box.x, data.box.y);
        f32 const       middle = (static_cast<f32>(font.font->ascend_height) -
                            font.font->descend_height) *
                           0.5f;
        Vecf4 const     white{1, 1, 1, 1};
        Vecf4 const     blue{0.25f, 0.6f, 1.f, 1.f};

        if(m_keyboard.background.valid())
            push_image(
                data,
                origin,
                origin + Vecf2(layout::background_w, layout::background_h),
                m_keyboard.background,
                Vecf2(layout::background_w, layout::background_h));

        if(keyboard.prompt < m_keyboard.labels.size())
            push_text(
                data,
                font,
                m_keyboard.labels[keyboard.prompt],
                origin.x + layout::panel_x,
                origin.y + layout::field_y - 12.f,
                white);

        /* Text field, with the cursor as an underscore */
        f32 const field_baseline =
            origin.y + layout::field_y + layout::field_h * 0.5f + middle;
        f32 const text_x = origin.x + layout::field_x;
        push_text(data, font, keyboard.text, text_x, field_baseline, white);
        push_text(
            data,
            font,
            u"_",
            text_x + font.measure(std::u16string_view(keyboard.text)
                                      .substr(0, keyboard.cursor)),
            field_baseline,
            white);

        for(auto const& [i, slot] : stl_types::const_enumerate(layout::slots()))
        {
            Vecf2 const centre   = origin + Vecf2(slot.x, slot.y);
            bool const  selected = i == keyboard.selected;

            if(slot.key < m_keyboard.keys.size())
            {
                auto const& key = m_keyboard.keys[slot.key];
                auto const& im  = selected ? key.selected
                                  : keyboard.engaged(slot.key) ? key.sticky
                                                               : generation_idx_t{};
                if(im.valid())
                {
                    Vecf2 const half = image_size(im) * 0.5f;
                    push_image(data, centre - half, centre + half, im);
                }
            }

            auto const label = keyboard.label(m_keyboard, slot.key);
            push_text(
                data,
                font,
                label,
                centre.x - font.measure(label) * 0.5f,
                centre.y + middle,
                selected ? white : blue);
        }
    }

    void process_render(
        generation_idx_t const& item,
        widget_data_t           data,
        gsl::span<u16 const>    focus  = {},
        UIScreen const*         screen = nullptr)
    {
        using widget_type = blam::ui_element::widget_type_t;

        Vecf2 const root(data.box.x, data.box.y);
        traverse_widget(
            item,
            layout_data_t{
                .origin      = root,
                .frame       = root,
                .focus       = focus,
                .stack_frame = screen && !screen->stack.empty()
                                   ? &screen->stack.back()
                                   : nullptr,
            },
            [&](UIElementItem& el,
                Vecf2                min,
                Vecf2                max,
                layout_data_t const& layout) -> bool {
                if(!el.background.empty())
                {
                    /* Description images hold one image per list item, as
                     * do a generated item's pictures; elsewhere image 1 is
                     * the focused look */
                    auto const item_image =
                        layout.item && !layout.item_root && layout.list &&
                                layout.list->image
                            ? std::optional<u16>(
                                  layout.list->image(*layout.item))
                            : std::nullopt;
                    auto const& im =
                        item_image
                            ? el.background[std::min<size_t>(
                                  *item_image, el.background.size() - 1)]
                        : layout.index
                            ? (*layout.index < el.background.size()
                                   ? el.background[*layout.index]
                                   : el.background[0])
                        : el.focused && el.background.size() >= 2
                            ? el.background[1]
                            : el.background[0];
                    push_image(data, min, max, im);
                }

                bool const is_spinner = el.ui_element->widget_type ==
                                        widget_type::spinner_list;
                if(is_spinner)
                {
                    /* Arrow bounds share the frame of the spinner's own */
                    auto const& sl    = el.ui_element->spinner_list;
                    auto        arrow = [&](auto const& images, auto const& b) {
                        if(!images.empty())
                            push_image(
                                data,
                                layout.frame + Vecf2(b.y, b.x),
                                layout.frame + Vecf2(b.w, b.z),
                                images[0]);
                    };
                    arrow(el.spinner_header, sl.header_bounds);
                    arrow(el.spinner_footer, sl.footer_bounds);
                    /* Its items are the children, not a string */
                    if(list_of(el))
                        return true;
                }

                if(el.ui_element->widget_type != widget_type::text_box &&
                   !is_spinner)
                    return true;

                auto const& tb = el.ui_element->text_box;
                /* Some generated item templates name no font */
                auto const font_id =
                    el.font_id.valid() ? el.font_id : m_default_font;
                if(!font_id.valid())
                    return false;
                auto font_it = font_cache.find(font_id);
                if(font_it == font_cache.end())
                    return false;
                FontItem const& font_item = font_it->second;
                if(font_item.glyph_map.empty())
                    return false;

                /* A spinner shows its selected value from the same list */
                i32 str_idx = is_spinner ? spinner_get(screen, el)
                                         : tb.string_list_index;
                std::u16string_view text;
                if(str_idx >= 0 &&
                   static_cast<size_t>(str_idx) < el.text_strings.size())
                    text = el.text_strings[static_cast<size_t>(str_idx)];
                /* Engine-filled text: the provider rewrites the tag's */
                std::u16string provided;
                if(m_data)
                    if(auto const* bound =
                           m_data->text(el.tag_name))
                    {
                        provided = (*bound)(text);
                        text     = provided;
                    }
                if(layout.list && layout.item && layout.list->text)
                {
                    provided = layout.list->text(el.tag_name, *layout.item);
                    text     = provided;
                }
                if(text.empty())
                    return false;
                std::u16string const expanded = expand_tokens(text);
                text                          = expanded;

                f32 const box_w = max.x - min.x;
                f32 const line_h =
                    static_cast<f32>(font_item.font->ascend_height) +
                    font_item.font->descend_height +
                    font_item.font->leadin_height;

                using just_t = blam::ui_element::text_box_t::justification_t;
                auto const lines =
                    layout_lines(font_item, text, box_w - tb.horizontal_offset);

                /* The tag's alpha applies as-is, so alpha 0 hides the text;
                 * only a colour that was never set falls back to white */
                Vecf4 color = tb.remapped_color();
                if(color == Vecf4{})
                    color = Vecf4{1, 1, 1, 1};

                /* One quad per glyph, appended straight into the frame's UI
                 * buffers -- for_each_glyph only owns where each one goes. */
                for(auto const& [line_idx, line] :
                    stl_types::const_enumerate(lines))
                {
                    f32 const text_width = this->text_width(font_item, line);
                    f32       start_x    = min.x + tb.horizontal_offset;
                    if(tb.justification == just_t::center)
                        start_x = min.x + (box_w - text_width) * 0.5f;
                    else if(tb.justification == just_t::right)
                        start_x = min.x + box_w - text_width - tb.horizontal_offset;
                    /* Centred text ignores the offsets, as the Xbox
                     * spinner values show */
                    f32 const baseline_y =
                        min.y +
                        (tb.justification == just_t::center
                             ? 0.f
                             : static_cast<f32>(tb.vertical_offset)) +
                        static_cast<f32>(font_item.font->ascend_height) +
                        line_h * static_cast<f32>(line_idx);
                    push_text(data, font_item, line, start_x, baseline_y, color);
                }
                return false;
            });
    }

    /* Split on line breaks, then wrap words to `width` */
    std::vector<std::u16string_view> layout_lines(
        FontItem const& font, std::u16string_view text, f32 width) const
    {
        constexpr auto npos = std::u16string_view::npos;

        std::vector<std::u16string_view> lines;
        while(true)
        {
            auto const          nl   = text.find(u'\n');
            std::u16string_view para = text.substr(0, nl);
            if(!para.empty() && para.back() == u'\r')
                para.remove_suffix(1);

            while(text_width(font, para) > width)
            {
                size_t cut = npos;
                for(auto i = para.find(u' '); i != npos;
                    i      = para.find(u' ', i + 1))
                {
                    if(text_width(font, para.substr(0, i)) > width)
                        break;
                    cut = i;
                }
                if(cut == npos || cut == 0)
                    break;
                lines.push_back(para.substr(0, cut));
                para.remove_prefix(cut + 1);
            }
            lines.push_back(para);

            if(nl == npos)
                break;
            text.remove_prefix(nl + 1);
        }
        return lines;
    }

    void start_restricted(Proxy&, time_point const&)
    {
    }

    void end_restricted(Proxy& e, time_point const&)
    {
        using namespace std::string_view_literals;
        using typing::pixels::CompFmt;

        auto* fb    = e.service<comp_app::GraphicsFramebuffer>();
        screen_size = Vecf2(fb->size().w, fb->size().h);

        f32  aspect = 1.f / fb->size().aspect();
        auto screen_scale =
            glm::scale(Matf3(1), Vecf2{aspect / 320.f * 1.33f, -1.f / 240.f}) *
            glm::translate(Matf3(1), Vecf2{-320.f, -240.f});

        if(!m_bus_subscribed)
        {
            m_bus_subscribed = true;
            using Coffee::Input::CIControllerAtomicEvent;
            using Coffee::Input::CIEvent;
            using Coffee::Input::CIMouseMoveEvent;
            if(auto* bus = e.service<comp_app::BasicEventBus<CIEvent>>())
            {
                bus->addEventFunction<CIMouseMoveEvent>(
                    1024, [this](CIEvent&, CIMouseMoveEvent* mv) {
                        m_mouse_raw    = {mv->origin.x, mv->origin.y};
                        m_mouse_active = true;
                    });
                bus->addEventFunction<CIMouseButtonEvent>(
                    1024, [this](CIEvent&, CIMouseButtonEvent* mb) {
                        m_mouse_active = true;
                        if(mb->mod == CIMouseButtonEvent::Pressed)
                        {
                            mouse_buttons |= mb->btn;
                            if(mb->btn == CIMouseButtonEvent::LeftButton)
                                m_click_pending = true;
                        } else
                            mouse_buttons &= mouse_buttons ^ mb->btn;
                    });
                bus->addEventFunction<CIControllerAtomicEvent>(
                    1024, [this](CIEvent&, CIControllerAtomicEvent*) {
                        m_mouse_active = false;
                    });
            }
            /* Producers may be on any thread; poll() delivers here */
            m_nav_queue =
                e.subsystem<UIEventBus>().addQueuedEventFunction<UINavigation>(
                    0, [this](UIEvent&, UINavigation* nav) {
                        m_nav_events.push_back(*nav);
                    });
            m_done_queue =
                e.subsystem<UIEventBus>().addQueuedEventFunction<UIFunctionDone>(
                    0, [this](UIEvent&, UIFunctionDone* done) {
                        m_done.push_back(*done);
                    });
        }
        mouse_pos = window_to_ui(m_mouse_raw);
        m_data = &e.subsystem<UIDataSource>();
        m_nav_queue->poll();
        m_done_queue->poll();

        std::vector<vertex_t>          vertex_data;
        std::vector<instance_vertex_t> instance_vertex_data;

        /* Runs with the UI hidden too, open has to reach a closed menu */
        bool const render_ui = e.subsystem<RenderingParameters>().render_ui;
        auto&      ui_bus    = e.subsystem<UIEventBus>();

        /* Handlers whose provider has now finished pick up where they
         * stopped; a screen that closed meanwhile is simply skipped */
        for(auto const& done : m_done)
        {
            auto it = std::find_if(
                m_pending.begin(), m_pending.end(), [&done](pending_t const& p) {
                    return p.token == done.token;
                });
            if(it == m_pending.end())
                continue;
            auto const pending = *it;
            m_pending.erase(it);
            for(auto const& entity : e.select<UIScreen>())
                if(entity.id() == pending.screen_id)
                {
                    auto& screen = e.ref<Proxy>(entity.id()).get<UIScreen>();
                    if(!screen.stack.empty())
                        finish_handler(
                            screen,
                            *pending.widget,
                            *pending.handler,
                            done.ok,
                            pending.slot);
                }
        }
        m_done.clear();
        for(auto const& entity : e.select<UIScreen>())
        {
            auto  ref    = e.ref<Proxy>(entity.id());
            auto& screen = ref.get<UIScreen>();
            m_screen_id  = entity.id();

            for(auto const& nav : m_nav_events)
            {
                if(!screen.accepts(nav.seat_idx))
                    continue;
                bool const was_open = !screen.stack.empty();
                apply(screen, nav);
                if(was_open && screen.stack.empty() &&
                   nav.action != UINavigation::close)
                {
                    UIEvent     ev{.type = UIEvent::menu_leave};
                    UIMenuLeave leave{.seat_idx = nav.seat_idx};
                    ui_bus.inject(ev, &leave);
                }
            }
        }
        m_nav_events.clear();

        if(render_ui)
        {
            for(auto const& entity : e.select<UIScreen>())
            {
                auto  ref    = e.ref<Proxy>(entity.id());
                auto& screen = ref.get<UIScreen>();

                if(screen.stack.empty())
                    continue;

                /* The name screens close themselves at once; the keyboard
                 * brings its own full-screen background and header */
                if(auto const& keyboard = screen.stack.back().keyboard)
                {
                    draw_keyboard(
                        widget_data_t{
                            .vertex_data   = vertex_data,
                            .instance_data = instance_vertex_data,
                            .box           = screen.box,
                        },
                        *keyboard);
                    continue;
                }

                /* The focused item is the child of the deepest widget that
                 * tabs through its children; a row's label below it and the
                 * containers above it keep their normal background */
                auto           path = resolve_focus(screen.stack.back());
                UIElementItem* leaf = path.size() > 1 ? path.back() : nullptr;
                for(size_t depth = path.size() - 1; depth-- > 0;)
                    if(tabs_through_children(*path[depth]))
                    {
                        leaf = path[depth + 1];
                        break;
                    }
                if(leaf)
                    leaf->focused = true;
                process_render(
                    screen.stack.back().widget,
                    widget_data_t{
                        .vertex_data   = vertex_data,
                        .instance_data = instance_vertex_data,
                        .box           = screen.box,
                    },
                    screen.stack.back().focus,
                    &screen);
                if(leaf)
                    leaf->focused = false;
            }
            m_click_pending = false;
        }

        u32 ui_vert_count = static_cast<u32>(vertex_data.size());

        bool cursor_visible = m_mouse_active && m_cursor_bitmap.valid();
        if(cursor_visible)
        {
            atlas_intermediate_t tmp{};
            auto const*          bitm =
                bitm_cache.assign_atlas_data(tmp, m_cursor_bitmap);
            auto const*             img = bitm->image.mip;
            Vecf2                   csz{(f32)img->isize.x, (f32)img->isize.y};
            Vecf2                   cmin  = m_mouse_raw;
            Vecf2                   cmax  = m_mouse_raw + csz;
            std::array<vertex_t, 6> verts = {{
                {.position = {cmin.x, cmin.y}, .tex_coord = {0, 0}},
                {.position = {cmax.x, cmin.y}, .tex_coord = {1, 0}},
                {.position = {cmax.x, cmax.y}, .tex_coord = {1, 1}},
                {.position = {cmin.x, cmin.y}, .tex_coord = {0, 0}},
                {.position = {cmax.x, cmax.y}, .tex_coord = {1, 1}},
                {.position = {cmin.x, cmax.y}, .tex_coord = {0, 1}},
            }};
            vertex_data.insert(vertex_data.end(), verts.begin(), verts.end());
            instance_vertex_t inst{};
            inst.color            = {1, 1, 1, 1};
            inst.tex_scale_offset = Vecf4(
                tmp.atlas_scale.x,
                tmp.atlas_scale.y,
                tmp.atlas_offset.x,
                tmp.atlas_offset.y);
            inst.texture_source.x = tmp.layer;
            instance_vertex_data.push_back(inst);
        }

        if(vertex_data.empty())
            return;

        constexpr size_t kUiInstanceSlots = 512;
        if(instance_vertex_data.size() < kUiInstanceSlots)
            instance_vertex_data.resize(kUiInstanceSlots);

        vertices->commit(Bytes::ofContainer(vertex_data).view);
        instance_vertices->commit(
            Bytes::ofContainer(instance_vertex_data).view);

        auto cursor_scale =
            glm::scale(
                Matf3(1), Vecf2{2.f / screen_size.x, -2.f / screen_size.y}) *
            glm::translate(
                Matf3(1), Vecf2{-screen_size.x / 2.f, -screen_size.y / 2.f});

        auto etc2_sampler = [&](typing::pixels::pix_flags flags) {
            if(!api.feature_info().texture.tex.gl.etc2)
                flags = typing::pixels::pix_flags::None;
            return flags == typing::pixels::pix_flags::None
                       ? bitm_cache
                             .template get_bucket<gfx::compat::texture_2da_t>(
                                 CompFmt(pix_fmt::BCn, comp_flags::BC1))
                             .sampler
                       : bitm_cache
                             .template get_bucket<gfx::compat::texture_2da_t>(
                                 CompFmt(pix_fmt::ETC2, flags))
                             .sampler;
        };

        auto do_submit = [&](Matf3 const& matrix, u32 offset, u32 count) {
            api.submit(
                gfx::draw_command{
                    .program  = ui_painter,
                    .vertices = array,
                    .call =
                        {
                            .instanced = false,
                            .mode      = gfx::drawing::primitive::triangle,
                        },
                    .data =
                        {
                            {.arrays = {.count = count, .offset = offset}},
                        },
                },
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair{
                        {"screen_scale"sv, 0}, semantic::SpanOne(matrix)}),
                gfx::make_sampler_list(
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_bc1"sv, 0},
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                CompFmt(pix_fmt::BCn, comp_flags::BC1))
                            .sampler},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_bc2"sv, 1},
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                CompFmt(pix_fmt::BCn, comp_flags::BC2))
                            .sampler},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_bc3"sv, 2},
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                CompFmt(pix_fmt::BCn, comp_flags::BC3))
                            .sampler},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_rgba4"sv, 3},
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                PixDesc(pix_fmt::RGBA4))
                            .sampler},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_rgba8"sv, 4},
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                PixDesc(pix_fmt::RGBA8))
                            .sampler},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_font"sv, 5},
                        font_cache.font_sampler},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_rg8"sv, 6},
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                PixDesc(pix_fmt::RG8))
                            .sampler},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_etc2_rgb"sv, 7},
                        etc2_sampler(typing::pixels::pix_flags::RGB)},
                    gleam::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"source_etc2_rgba"sv, 8},
                        etc2_sampler(typing::pixels::pix_flags::RGBA)}),
                gfx::make_buffer_list(
                    gfx::buffer_definition_t{
                        .stage  = typing::graphics::ShaderStage::Fragment,
                        .key    = {"InstanceData"sv, 0},
                        .buffer = instance_vertices->slice(0),
                        .stride = 0,
                    }),
                gfx::blend_state{.additive = false});
        };

        if(ui_vert_count > 0)
            do_submit(screen_scale, 0, ui_vert_count);
        if(cursor_visible)
            do_submit(cursor_scale, ui_vert_count, 6);
    }
};

void alloc_ui_system(compo::EntityContainer& e)
{
    ProfContext _;
    auto&       gfx = e.subsystem_cast<gfx::system>();
    if(!gfx.workarounds().bugs.adreno_3xx &&
       std::get<0>(gfx.api_version()) != 2)
        e.register_subsystem_inplace<UIRenderer>(
            std::ref(e.subsystem_cast<gfx::system>()),
            std::ref(e.subsystem_cast<UIElementCache<halo_version>>()),
            std::ref(e.subsystem_cast<BitmapCache<halo_version>>()),
            std::ref(e.subsystem_cast<FontCache<halo_version>>()));
    e.register_component_inplace<UIScreen>();
    e.register_subsystem_inplace<UIEventBus>();
    e.register_subsystem_inplace<UIDataSource>();
}

void load_ui_items(
    compo::EntityContainer& e, MapChangedEvent<halo_version>& data)
{
    auto& fonts       = e.subsystem_cast<FontCache<halo_version>>();
    auto& ui_elements = e.subsystem_cast<UIElementCache<halo_version>>();

    fonts.load_from(data.container);
    ui_elements.load_from(data.container);

    blam::tag_index_view<halo_version> tag_view(data.container);

    // Preload large_ui font for text_box widgets
    generation_idx_t large_ui_font;
    for(blam::tag_t const& tag : tag_view)
    {
        if(!tag.matches(blam::tag_class_t::font))
            continue;
        auto name = tag.to_name().to_string(data.container.magic);
        if(name.find("large_ui") != std::string_view::npos)
        {
            large_ui_font = fonts.predict(tag.as_ref());
            break;
        }
    }

    // Parse pause_game_options strings
    std::vector<std::u16string> pause_strings;
    for(blam::tag_t const& tag : tag_view)
    {
        if(!tag.matches(blam::tag_class_t::unicode_string))
            continue;
        auto name = tag.to_name().to_string(data.container.magic);
        if(name.find("pause_game_options") == std::string_view::npos)
            continue;

        auto us_opt =
            tag.template data<blam::ui::unicode_string_list>(data.container.magic);
        if(!us_opt.has_value())
            continue;
        auto subs_opt = us_opt.value()->data.data(data.container.magic);
        if(!subs_opt.has_value() || subs_opt.value().empty())
            continue;

        std::vector<std::u16string> strings;
        for(auto const& ref : subs_opt.value())
        {
            auto s = ref.str(data.container.magic);
            if(!s.has_error() && !s.value().empty())
                strings.emplace_back(s.value());
        }

        bool is_mp = name.find("multiplayer") != std::string_view::npos;
        if(is_mp || pause_strings.empty())
            pause_strings = std::move(strings);
        if(is_mp)
            break;
    }

    // Build widget tree
    std::vector<generation_idx_t> root_widgets;
    for(blam::tag_t const& tag : tag_view)
    {
        if(tag.matches(blam::tag_class_t::Soul))
            root_widgets = ui_elements.explore(tag.as_ref());
    }

    cDebug(
        "UI load: {} pause strings, font valid={}, {} root widgets",
        pause_strings.size(),
        large_ui_font.valid(),
        root_widgets.size());

    // Collect text_boxes with accumulated screen Y, sort, then assign strings
    // in visual top-to-bottom order so the string list index matches screen
    // position.
    if(!pause_strings.empty() && large_ui_font.valid())
    {
        struct Entry
        {
            generation_idx_t id;
            i32              screen_y;
        };

        std::vector<Entry> entries;

        std::function<void(generation_idx_t, i32)> collect =
            [&](generation_idx_t id, i32 parent_y) {
                auto it = ui_elements.find(id);
                if(it == ui_elements.end())
                    return;
                UIElementItem const& item = it->second;
                using wt                  = blam::ui_element::widget_type_t;

                /* bounds stored [y1, x1, y2, x2]; .x = y1 */
                i32 this_y = parent_y + item.ui_element->bounds.x;

                if(item.ui_element->widget_type == wt::text_box &&
                   it->second.text_strings.empty())
                    entries.push_back({id, this_y});

                auto child_meta =
                    item.ui_element->child_widgets.data(data.container.magic);
                if(!child_meta.has_value())
                    return;
                for(size_t i = 0; i < item.children.size(); i++)
                {
                    i32 child_y =
                        this_y + child_meta.value()[i].vertical_offset;
                    collect(item.children[i], child_y);
                }
            };

        if(!root_widgets.empty())
            collect(root_widgets[0], 0);

        std::stable_sort(
            entries.begin(), entries.end(), [](Entry const& a, Entry const& b) {
                return a.screen_y < b.screen_y;
            });

        /* Map pause menu button names to their string index in the
         * pause_strings list.  The game engine assigns strings by game logic,
         * not tag data; button names are the only stable identifier available
         * to us. */
        static constexpr std::array<std::pair<std::string_view, i32>, 4>
            kButtonStringMap{{
                {"resume_game_button", 0},
                {"quit_netgame_button", 1},
                {"change_settings_button", 2},
                {"game_options_button",
                 3}, /* repurposed as team select in MP */
            }};

        for(auto& [id, y] : entries)
        {
            auto it = ui_elements.find(id);
            if(it == ui_elements.end())
                continue;
            UIElementItem& item  = it->second;
            auto           wname = item.ui_element->name.str();
            i32            sidx  = -1;
            for(auto const& [bname, bidx] : kButtonStringMap)
                if(bname == wname)
                {
                    sidx = bidx;
                    break;
                }
            if(sidx < 0 || static_cast<size_t>(sidx) >= pause_strings.size())
                continue;
            item.text_strings.push_back(
                pause_strings[static_cast<size_t>(sidx)]);
            item.font_id = large_ui_font;
        }
        cDebug("  {} text_boxes found", entries.size());
    }

    u32 player_count = 0;
    for(auto ent : e.select<PlayerCamera>())
    {
        auto* cam = e.get<PlayerCamera>(ent.id());
        if(cam && cam->is_active())
            ++player_count;
    }
    if(player_count == 0)
        player_count = 1;

    struct PauseVariant
    {
        u32              players;
        generation_idx_t id;
    };

    std::vector<PauseVariant> pause_variants;
    for(auto const& id : root_widgets)
    {
        auto it = ui_elements.find(id);
        if(it == ui_elements.end())
            continue;
        auto wname = it->second.ui_element->name.str();
        if(wname.size() >= 12 && wname.substr(1) == "p_pause_game" &&
           std::isdigit(static_cast<unsigned char>(wname[0])))
            pause_variants.push_back({u32(wname[0] - '0'), id});
    }
    std::sort(
        pause_variants.begin(),
        pause_variants.end(),
        [](PauseVariant const& a, PauseVariant const& b) {
            return a.players < b.players;
        });

    generation_idx_t selected_id{};
    for(auto const& v : pause_variants)
    {
        selected_id = v.id;
        if(v.players >= player_count)
            break;
    }
    if(!selected_id.valid() && !root_widgets.empty())
        selected_id = root_widgets[0];

    compo::EntityRecipe rec;
    rec.tags       = ObjectGC;
    rec.components = {compo::type_hash_v<UIScreen>()};
    if(selected_id.valid())
    {
        auto  ref    = e.create_entity(rec);
        auto& screen = ref.get<UIScreen>();
        screen.home  = selected_id;
        /* The UI map is the main menu, so it starts open */
        if(data.container.map->map_type == blam::maptype_t::ui)
        {
            screen.stack.push_back({.widget = selected_id});
            e.subsystem_cast<RenderingParameters>().render_ui = true;
        }
    }

    auto& bitmaps = e.subsystem_cast<BitmapCache<halo_version>>();

    /* A map ships one vcky, which the engine uses for all text entry */
    VirtualKeyboardData keyboard;
    for(blam::tag_t const& tag : tag_view)
    {
        if(!tag.matches(blam::tag_class_t::vcky))
            continue;
        auto vk_opt =
            tag.template data<blam::virtual_keyboard>(data.container.magic);
        if(!vk_opt.has_value())
            break;
        auto const* vk    = vk_opt.value();
        auto        first = [&bitmaps](auto const& ref) {
            generation_idx_t out;
            if(ref.valid())
                if(auto all = bitmaps.resolve_all(ref); !all.empty())
                    out = all.front();
            return out;
        };

        if(vk->display_font.valid())
            keyboard.font_id = fonts.predict(vk->display_font);
        keyboard.background = first(vk->background);
        if(auto labels = tag_view.template data<blam::ui::unicode_string_list>(
               vk->special_key_labels_string_list);
           labels.has_value())
            if(auto subs = labels.value()->data.data(data.container.magic);
               subs.has_value())
                for(auto const& ref : subs.value())
                {
                    auto s = ref.str(data.container.magic);
                    if(s.has_error())
                        keyboard.labels.emplace_back();
                    else
                        keyboard.labels.emplace_back(s.value());
                }
        if(auto keys = vk->virtual_keys.data(data.container.magic);
           keys.has_value())
            for(auto const& key : keys.value())
            {
                if(key.key >= keyboard.keys.size())
                    keyboard.keys.resize(key.key + 1u);
                keyboard.keys[key.key] = {
                    .key      = &key,
                    .selected = first(key.selected_bg),
                    .active   = first(key.active_bg),
                    .sticky   = first(key.sticky_bg),
                };
            }
        break;
    }

    /* Button icons from the HUD globals, for "%a-button" in menu text */
    std::vector<UIRenderer::button_icon_t> button_icons;
    for(blam::tag_t const& tag : tag_view)
    {
        if(!tag.matches(blam::tag_class_t::hudg))
            continue;
        auto hg_opt = tag.template data<blam::hud_globals>(data.container.magic);
        if(!hg_opt.has_value())
            break;
        auto const* hg    = hg_opt.value();
        auto const  magic = data.container.magic;
        using flags_t     = blam::hud_globals::button_icon_t::flags_t;

        std::vector<std::u16string> texts;
        if(auto list = tag_view.template data<blam::ui::unicode_string_list>(
               hg->alternate_icon_text);
           list.has_value())
            if(auto subs = list.value()->data.data(magic); subs.has_value())
                for(auto const& ref : subs.value())
                {
                    auto s = ref.str(magic);
                    texts.emplace_back(
                        s.has_error() ? std::u16string{}
                                      : std::u16string(s.value()));
                }

        /* Menus use the small sheet, which shares the HUD sheet's sequence
         * order; the sprite sheet's header holds the sequences */
        blam::tagref_t icon_sheet = hg->icon_bitmap;
        for(blam::tag_t const& bitm_tag : tag_view)
            if(bitm_tag.matches(blam::tag_class_t::bitm) &&
               bitm_tag.to_name().to_string(data.container.magic) ==
                   "ui\\hud\\bitmaps\\hud_msg_icons_sm")
            {
                icon_sheet = bitm_tag.as_ref();
                break;
            }
        blam::bitm::header_t const* sheet = nullptr;
        if(icon_sheet.valid())
            if(auto first = bitmaps.resolve(icon_sheet, 0); first.valid())
                sheet = bitmaps.find(first)->second.header;

        auto const entries = hg->button_icons.data(magic);
        if(!entries.has_value())
            break;
        for(auto const& entry : entries.value())
        {
            auto const flags = static_cast<u8>(entry.flags);
            auto has = [flags](flags_t f) { return (flags & static_cast<u8>(f)) != 0; };
            UIRenderer::button_icon_t icon;
            if(has(flags_t::use_text_from_string_list))
            {
                if(entry.text_index >= 0 &&
                   static_cast<size_t>(entry.text_index) < texts.size())
                    icon.text = texts[static_cast<size_t>(entry.text_index)];
                button_icons.push_back(std::move(icon));
                continue;
            }
            if(sheet)
                if(auto const sequences = sheet->sequences.data(magic);
                   sequences.has_value() && entry.sequence_index >= 0 &&
                   static_cast<size_t>(entry.sequence_index) <
                       sequences.value().size())
                {
                    auto const& sequence = sequences.value()[static_cast<size_t>(
                        entry.sequence_index)];
                    auto const sprites = sequence.sprites.data(magic);
                    auto const images  = sheet->images.data(magic);
                    if(sprites.has_value() && !sprites.value().empty() &&
                       images.has_value())
                    {
                        auto const& sprite = sprites.value()[0];
                        auto const& image  = images.value()[sprite.bitmap_index];
                        icon.image =
                            bitmaps.resolve(icon_sheet, sprite.bitmap_index);
                        icon.uv = Vecf4(
                            sprite.left, sprite.top, sprite.right, sprite.bottom);
                        icon.size = Vecf2(
                            (sprite.right - sprite.left) * image.isize.x,
                            (sprite.bottom - sprite.top) * image.isize.y);
                    }
                }
            /* hudg's width and placement offsets are tuned for the HUD's
             * larger sheet; menu text spaces the small icon like a glyph */
            icon.advance = icon.size.x;
            if(has(flags_t::override_default_color))
                icon.color = Vecf4(
                    entry.color.r / 255.f,
                    entry.color.g / 255.f,
                    entry.color.b / 255.f,
                    1.f);
            button_icons.push_back(std::move(icon));
        }
        break;
    }

    fonts.allocate_font_texture();
    for(blam::tag_t const& tag : tag_view)
    {
        if(!tag.matches(blam::tag_class_t::bitm))
            continue;
        if(tag.to_name().to_string(data.container.magic) !=
           "ui\\shell\\bitmaps\\cursor")
            continue;
        try
        {
            e.subsystem_cast<UIRenderer>().m_cursor_bitmap =
                bitmaps.predict(tag.as_ref(), 0);
        } catch(...)
        {
        }
        break;
    }
    try
    {
        e.subsystem_cast<UIRenderer>().m_default_font = large_ui_font;
        e.subsystem_cast<UIRenderer>().m_keyboard     = std::move(keyboard);
        e.subsystem_cast<UIRenderer>().m_button_icons = std::move(button_icons);
    } catch(...)
    {
    }
}
