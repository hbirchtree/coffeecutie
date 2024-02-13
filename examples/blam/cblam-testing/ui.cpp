#include "ui.h"

#include "components.h"
#include "data.h"
#include "graphics_api.h"
#include "shader_compiler.h"
#include "ui_caching.h"

#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <peripherals/semantic/chunk.h>

using semantic::RSCA;

struct UIElement
{
    using value_type = UIElement;
    using type       = compo::alloc::VectorContainer<value_type>;
    using tag_type   = type;

    generation_idx_t element;
    bool             visible{false};
};

using UIRendererManifest = compo::SubsystemManifest<
    type_list_t<UIElement>,
    type_list_t<gfx::system, RenderingParameters>,
    type_list_t<comp_app::DisplayInfo, comp_app::GraphicsFramebuffer>>;

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
        BitmapCache<halo_version>&    bitm_cache) :
        api(api),
        ui_cache(ui_cache), bitm_cache(bitm_cache)
    {
        using namespace std::string_view_literals;

        priority = 1534;

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

    std::shared_ptr<gfx::program_t>      ui_painter;
    std::shared_ptr<gfx::buffer_t>       vertices;
    std::shared_ptr<gfx::buffer_t>       instance_vertices;
    std::shared_ptr<gfx::vertex_array_t> array;

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

    void process_widget(
        generation_idx_t const& item,
        widget_data_t           data,
        layout_data_t           layout = {})
    {
        using widget_type       = blam::ui_element::widget_type_t;
        UIElementItem const& el = ui_cache.find(item)->second;

        [[maybe_unused]] auto widget_name = el.ui_element->name.str();

        auto& bounds = el.ui_element->bounds;
        // Coords + dimensions we can derive purely from the bounding box
        // The bounding box coordinates are in [y1, x1, y2, x2]
        auto dimensions   = Vecf2(bounds.w - bounds.y, bounds.z - bounds.x);
        auto local_origin = Vecf2(bounds.y, bounds.x);
        // Now calculate the global screen position,
        // depending on the parent-provided box on screen
        auto global_origin
            = Vecf2(data.box.x, data.box.y) + layout.offset + local_origin;

        auto const& min = global_origin;
        auto        max = global_origin + dimensions;

        std::array<vertex_t, 6> vertices = {{
            {
                .position  = Vecf2{min.x, min.y},
                .tex_coord = Vecf2{0, 0},
            },
            {
                .position  = Vecf2{max.x, min.y},
                .tex_coord = Vecf2{1, 0},
            },
            {
                .position  = Vecf2{max.x, max.y},
                .tex_coord = Vecf2{1, 1},
            },
            {
                .position  = Vecf2{min.x, min.y},
                .tex_coord = Vecf2{0, 0},
            },
            {
                .position  = Vecf2{max.x, max.y},
                .tex_coord = Vecf2{1, 1},
            },
            {
                .position  = Vecf2{min.x, max.y},
                .tex_coord = Vecf2{0, 1},
            },
        }};

        if(el.background.valid())
        {
            data.vertex_data.insert(
                data.vertex_data.end(), vertices.begin(), vertices.end());
            data.instance_data.push_back({
                .color = Vecf4{1, 1, 1, 0},
            });

            auto& inst = data.instance_data.back();

            atlas_intermediate_t tmp{};
            bitm_cache.assign_atlas_data(tmp, el.background);
            inst.tex_scale_offset = Vecf4(
                tmp.atlas_scale.x,
                tmp.atlas_scale.y,
                tmp.atlas_offset.x,
                tmp.atlas_offset.y);
            inst.texture_source.x = tmp.layer;
        }

        switch(el.ui_element->widget_type)
        {
        case widget_type::column_list:
        case widget_type::container: {
            widget_data_t child_data = {
                .vertex_data   = data.vertex_data,
                .instance_data = data.instance_data,
                .box           = {min.x, min.y, max.x, max.y},
            };

            auto children
                = el.ui_element->child_widgets.data(bitm_cache.magic).value();
            u32 i = 0;
            for(auto const& child : el.children)
            {
                auto const& meta = children[i];
                process_widget(
                    child,
                    child_data,
                    layout_data_t{
                        .offset
                        = Vecf2(meta.horizontal_offset, meta.vertical_offset),
                    });
                i++;
            }
            break;
        }
        case widget_type::text_box:
            // TODO: Render the text, for now just render the background
            break;
        default:
            break;
        }
    }

    void start_restricted(Proxy& /*e*/, time_point const&)
    {
    }

    void end_restricted(Proxy& e, time_point const&)
    {
        using namespace std::string_view_literals;
        using typing::pixels::CompFmt;

        if(!e.subsystem<RenderingParameters>().render_ui)
            return;

        auto* fb = e.service<comp_app::GraphicsFramebuffer>();

        /* Used for anything centered on screen, in the typical 640x480
         * screenspace
         * This is what most of the widgets are aimed at
         */
        f32  aspect = 1.f / fb->size().aspect();
        auto screen_scale
            = glm::scale(Matf3(1), Vecf2{aspect / 320.f, -.5f / 240.f})
              * glm::translate(Matf3(1), Vecf2{-320.f * aspect, -240.f});
        /* Used for anything that docks to any of the corners/sides
         * TODO: Correct scale for aspect ratio
         */
        // f32  full_screen_basis = (480.f * aspect) / 2.f;
        // auto full_screen_scale = glm::translate(
        //     glm::scale(
        //         Matf3(1), Vecf2{aspect / full_screen_basis, 1.f / 240.f}),
        //     Vecf2{-full_screen_basis, -240.f});

        std::vector<vertex_t>          vertex_data;
        std::vector<instance_vertex_t> instance_vertex_data;

        for(auto const& widget : e.select<UIElement>())
        {
            auto        ref     = e.ref<Proxy>(widget);
            auto const& element = ref.get<UIElement>();
            if(!element.visible)
                continue;
            process_widget(
                element.element,
                widget_data_t{
                    .vertex_data   = vertex_data,
                    .instance_data = instance_vertex_data,
                    .box           = blam::vec4i16{0, 0, 640, 480},
                });
        }

        vertices->commit(Bytes::ofContainer(vertex_data).view);
        instance_vertices->commit(
            Bytes::ofContainer(instance_vertex_data).view);

        api.submit(gfx::draw_command{
            .program = ui_painter,
            .vertices = array,
            .call = {
                .instanced = false,
                .mode = gfx::drawing::primitive::triangle,
            },
            .data = {
                {
                    .arrays = {
                        .count = static_cast<u32>(
                            instance_vertex_data.size() * 6),
                    },
                    // .instances = {
                    //     .count = static_cast<u32>(
                    //         instance_vertex_data.size()),
                    // },
                },
            },
        },
        gfx::make_uniform_list(typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{{"screen_scale"sv, 0}, semantic::SpanOne(screen_scale)}),
        gfx::make_sampler_list(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc1"sv, 0},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC1))
                .sampler},
                gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc2"sv, 1},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC2))
                .sampler},
            gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc3"sv, 2},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC3))
                .sampler},
            gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgba4"sv, 3},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::RGBA4))
                .sampler},
            gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgba8"sv, 4},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::RGBA8))
                .sampler}),
        gfx::make_buffer_list(gfx::buffer_definition_t{
            .stage = typing::graphics::ShaderStage::Fragment,
            .key = {"InstanceData"sv, 0},
            .buffer = instance_vertices->slice(0),
            .stride = 0,
        }),
        gfx::blend_state{ .additive = false });
    }
};

void alloc_ui_system(compo::EntityContainer& e)
{
    if(!e.subsystem_cast<gfx::system>().workarounds().bugs.adreno_3xx)
            e.register_subsystem_inplace<UIRenderer>(
                std::ref(e.subsystem_cast<gfx::system>()),
                std::ref(e.subsystem_cast<UIElementCache<halo_version>>()),
                std::ref(e.subsystem_cast<BitmapCache<halo_version>>()));
    e.register_component_inplace<UIElement>();
}

void load_ui_items(
    compo::EntityContainer& e, MapChangedEvent<halo_version>& data)
{
    auto& fonts       = e.subsystem_cast<FontCache<halo_version>>();
    auto& ui_elements = e.subsystem_cast<UIElementCache<halo_version>>();
    // auto& ui_renderer = e.subsystem_cast<UIRenderer>();

    fonts.load_from(data.container);
    ui_elements.load_from(data.container);

    std::vector<generation_idx_t> root_widgets;
    for(blam::tag_t const& tag : blam::tag_index_view(data.container))
    {
        if(tag.matches(blam::tag_class_t::font))
        {
            fonts.predict(tag.as_ref());
        } else if(tag.matches(blam::tag_class_t::Soul))
        {
            root_widgets = ui_elements.explore(tag.as_ref());
        }
    }

    compo::EntityRecipe rec;
    rec.tags       = ObjectGC;
    rec.components = {compo::type_hash_v<UIElement>()};
    for(auto const& id : root_widgets)
    {
        auto ref                     = e.create_entity(rec);
        ref.get<UIElement>().element = id;
        ref.get<UIElement>().visible = true;
        break;
    }

    fonts.allocate_font_texture();
}
