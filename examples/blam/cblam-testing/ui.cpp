#include "ui.h"

#include <cctype>

#include "coffee/core/types/input/event_types.h"
#include "components.h"
#include "data.h"
#include "graphics_api.h"
#include "shader_compiler.h"
#include "ui_caching.h"

#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/enumerate.h>

using semantic::RSCA;

struct UIElement
{
    using value_type = UIElement;
    using type       = compo::alloc::VectorContainer<value_type>;
    using tag_type   = type;

    generation_idx_t element;
};

using UIRendererManifest = compo::SubsystemManifest<
    type_list_t<UIElement>,
    type_list_t<gfx::system, RenderingParameters>,
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
    std::vector<generation_idx_t>   m_widget_history;

    struct widget_data_t
    {
        std::vector<vertex_t>&          vertex_data;
        std::vector<instance_vertex_t>& instance_data;
        blam::vec4i16                   box;
    };

    struct layout_data_t
    {
        Vecf2 offset;
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

    auto& create_element(compo::EntityContainer& e, generation_idx_t id)
    {
        for(auto el : e.select<UIElement>())
        {
            auto  ref        = el;
            auto& ui_element = ref.get<UIElement>();
            if(ui_element.element == id)
                return ui_element;
        }

        compo::EntityRecipe rec;
        rec.tags           = ObjectGC;
        rec.components     = {compo::type_hash_v<UIElement>()};
        auto  ref          = e.create_entity(rec);
        auto& ui_element   = ref.get<UIElement>();
        ui_element.element = id;
        return ui_element;
    }

    struct event_result
    {
        bool             consumed{false};
        generation_idx_t open_widget{};
        bool             go_back{false};
    };

    /* Walk the widget tree depth-first.
     * visit(el, min, max) -> bool: return false to skip children. */
    template<typename Fn>
    void traverse_widget(
        generation_idx_t const& item,
        blam::vec4i16           box,
        layout_data_t           layout,
        Fn&&                    visit)
    {
        UIElementItem& el = ui_cache.find(item)->second;
        if(!el.visible)
            return;

        auto& bounds = el.ui_element->bounds;
        // bounds stored [y1, x1, y2, x2] as [.x, .y, .z, .w]
        Vecf2 global_origin =
            Vecf2(box.x, box.y) + layout.offset + Vecf2(bounds.y, bounds.x);
        Vecf2 min = global_origin;
        Vecf2 max =
            global_origin + Vecf2(bounds.w - bounds.y, bounds.z - bounds.x);

        if(!visit(el, min, max) || el.children.empty())
            return;

        auto children_opt = el.ui_element->child_widgets.data(bitm_cache.magic);
        if(!children_opt.has_value())
            return;
        auto const    children  = children_opt.value();
        blam::vec4i16 child_box = {
            (i16)min.x, (i16)min.y, (i16)max.x, (i16)max.y};
        for(auto const& [i, child] : stl_types::const_enumerate(el.children))
        {
            auto const& meta = children[i];
            traverse_widget(
                child,
                child_box,
                layout_data_t{
                    .offset =
                        Vecf2(meta.horizontal_offset, meta.vertical_offset)},
                std::forward<Fn>(visit));
        }
    }

    event_result process_input(
        generation_idx_t const& item,
        widget_data_t           data,
        layout_data_t           layout = {})
    {
        using eh_t = blam::ui_element::event_handler_t;
        event_result result;

        traverse_widget(
            item,
            data.box,
            layout,
            [&](UIElementItem& el, Vecf2 min, Vecf2 max) -> bool {
                el.focused = mouse_in_bounds(min, max);
                if(!el.focused || !m_click_pending)
                    return true;

                cDebug(
                    "Widget clicked: {} pending={}",
                    el.ui_element->name.str(),
                    m_click_pending);

                auto handlers_opt =
                    el.ui_element->event_handlers.data(bitm_cache.magic);
                if(!handlers_opt.has_value())
                    return true;

                for(auto const& eh : handlers_opt.value())
                {
                    /* left mouse acts as both direct mouse event and confirm
                     * (a_btn), since PC maps mouse click to controller confirm
                     */
                    bool mouse_event =
                        eh.event_type == eh_t::type_t::left_mouse ||
                        eh.event_type == eh_t::type_t::a_btn;
                    cDebug(
                        "handler: type={} flags={} match={}",
                        magic_enum::enum_name(eh.event_type),
                        magic_enum::enum_name(eh.flags),
                        mouse_event);
                    if(!mouse_event)
                        continue;
                    auto flags = static_cast<u32>(eh.flags);
                    if(flags & static_cast<u32>(eh_t::flags_t::open_widget))
                    {
                        auto widget_ = ui_cache.predict(eh.widget);
                        if(widget_.valid())
                        {
                            cDebug(
                                "open_widget: {}",
                                eh.widget.name.to_string(bitm_cache.magic));
                            result.open_widget = widget_;
                            result.consumed    = true;
                        }
                    }
                    if(flags & static_cast<u32>(
                                   eh_t::flags_t::go_back_to_previous_widget))
                    {
                        result.go_back  = true;
                        result.consumed = true;
                    }
                }
                return true;
            });

        return result;
    }

    void process_render(
        generation_idx_t const& item,
        widget_data_t           data,
        layout_data_t           layout = {})
    {
        using widget_type = blam::ui_element::widget_type_t;

        traverse_widget(
            item,
            data.box,
            layout,
            [&](UIElementItem& el, Vecf2 min, Vecf2 max) -> bool {
                auto dimensions = max - min;

                if(el.background.valid())
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
                    data.instance_data.push_back({.color = Vecf4{1, 1, 1, 0}});

                    auto&                inst = data.instance_data.back();
                    atlas_intermediate_t tmp{};
                    auto const& im   = el.focused && el.background_alt.valid()
                                           ? el.background_alt
                                           : el.background;
                    auto const* bitm = bitm_cache.assign_atlas_data(tmp, im);
                    auto const* img  = bitm->image.mip;
                    auto        imscale = Vecf2(
                        dimensions.x / img->isize.x,
                        dimensions.y / img->isize.y);
                    inst.tex_scale_offset = Vecf4(
                        tmp.atlas_scale.x * std::min(imscale.x, 1.f),
                        tmp.atlas_scale.y * std::min(imscale.y, 1.f),
                        tmp.atlas_offset.x,
                        tmp.atlas_offset.y);
                    inst.texture_source.x = tmp.layer;
                }

                if(el.ui_element->widget_type != widget_type::text_box)
                    return true;

                auto const& tb = el.ui_element->text_box;
                if(!el.font_id.valid())
                    return false;
                auto font_it = font_cache.find(el.font_id);
                if(font_it == font_cache.end())
                    return false;
                FontItem const& font_item = font_it->second;
                if(font_item.glyph_map.empty())
                    return false;

                i32 str_idx = tb.string_list_index;
                if(str_idx < 0 ||
                   static_cast<size_t>(str_idx) >= el.text_strings.size())
                    return false;
                std::u16string const& text =
                    el.text_strings[static_cast<size_t>(str_idx)];
                if(text.empty())
                    return false;

                constexpr f32 kAtlasSize  = 256.f;
                constexpr u32 kFontSource = 9u;
                u32 tex_source = (kFontSource << 24) | font_item.atlas_layer;

                f32 text_width = 0.f;
                for(char16_t c : text)
                {
                    auto git = font_item.glyph_map.find(static_cast<u16>(c));
                    if(git != font_item.glyph_map.end())
                        text_width += git->second.advance;
                }

                f32 box_w   = max.x - min.x;
                f32 start_x = min.x + tb.horizontal_offset;
                f32 baseline_y =
                    min.y + tb.vertical_offset +
                    static_cast<f32>(font_item.font->ascend_height);

                using just_t = blam::ui_element::text_box_t::justification_t;
                if(tb.justification == just_t::center)
                    start_x = min.x + (box_w - text_width) * 0.5f;
                else if(tb.justification == just_t::right)
                    start_x = min.x + box_w - text_width - tb.horizontal_offset;

                f32 cursor_x = start_x;
                for(char16_t c : text)
                {
                    auto git = font_item.glyph_map.find(static_cast<u16>(c));
                    if(git == font_item.glyph_map.end())
                        continue;
                    GlyphEntry const& g = git->second;
                    if(g.bitmap_width <= 0 || g.bitmap_height <= 0)
                    {
                        cursor_x += g.advance;
                        continue;
                    }

                    f32 gx  = cursor_x + g.origin_x;
                    f32 gy  = baseline_y - g.origin_y;
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
                        data.vertex_data.end(),
                        glyph_verts.begin(),
                        glyph_verts.end());

                    instance_vertex_t inst{};
                    inst.color            = (tb.color.a > 0.f) ? Vecf4(tb.color)
                                                               : Vecf4{1, 1, 1, 1};
                    inst.tex_scale_offset = Vecf4(
                        g.bitmap_width / kAtlasSize,
                        g.bitmap_height / kAtlasSize,
                        g.atlas_x / kAtlasSize,
                        g.atlas_y / kAtlasSize);
                    inst.texture_source.x = tex_source;
                    data.instance_data.push_back(inst);
                    cursor_x += g.advance;
                }
                return false;
            });
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
        }
        mouse_pos = window_to_ui(m_mouse_raw);

        std::vector<vertex_t>          vertex_data;
        std::vector<instance_vertex_t> instance_vertex_data;

        if(e.subsystem<RenderingParameters>().render_ui)
        {
            for(auto const& widget : e.select<UIElement>())
            {
                auto          ref     = e.ref<Proxy>(widget.id());
                auto&         element = ref.get<UIElement>();
                widget_data_t root_data{
                    .vertex_data   = vertex_data,
                    .instance_data = instance_vertex_data,
                    .box           = blam::vec4i16{0, 0, 640, 480},
                };
                auto result = process_input(element.element, root_data);
                process_render(element.element, root_data);

                if(result.open_widget.valid())
                {
                    m_widget_history.push_back(element.element);
                    element.element = result.open_widget;
                } else if(result.go_back && !m_widget_history.empty())
                {
                    element.element = m_widget_history.back();
                    m_widget_history.pop_back();
                }
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
                        font_cache.font_sampler}),
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
    e.register_component_inplace<UIElement>();
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
            tag.template data<blam::ui::unicode_string>(data.container.magic);
        if(!us_opt.has_value())
            continue;
        auto subs_opt = us_opt.value()->sub_strings.data(data.container.magic);
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
    rec.components = {compo::type_hash_v<UIElement>()};
    if(selected_id.valid())
    {
        auto ref                     = e.create_entity(rec);
        ref.get<UIElement>().element = selected_id;
    }

    fonts.allocate_font_texture();

    auto& bitmaps = e.subsystem_cast<BitmapCache<halo_version>>();
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
}
