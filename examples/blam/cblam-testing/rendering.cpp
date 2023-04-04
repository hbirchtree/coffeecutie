#include "rendering.h"

#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <peripherals/stl/tuple_hash.h>

#include "caching.h"
#include "data.h"
#include "resource_creation.h"
#include "selected_version.h"

using namespace libc_types::size_literals;
using namespace std::string_view_literals;

namespace detail {

inline Tup<PixFmt, CompFlags> get_bitm_hash(BitmapItem const& bitm)
{
    return std::make_tuple(bitm.image.fmt.pixfmt, bitm.image.fmt.cmpflg);
}

template<typename MapType>
inline void assign_map(MapType& map, BitmapItem const* bitm)
{
    map.atlas_scale  = {0};
    map.atlas_offset = {0};
    map.layer        = -1;

    if(!bitm)
        return;

    map.atlas_scale  = bitm->image.scale;
    map.atlas_offset = bitm->image.offset;
    map.layer        = bitm->image.layer;
}

} // namespace detail

template<typename Version>
struct MeshRenderer : Components::RestrictedSubsystem<
                          MeshRenderer<Version>,
                          MeshRendererManifest<Version>>
{
    using type = MeshRenderer;

    //    using parent_type = Components::RestrictedSubsystem<
    //        Components::TagType<MeshRenderer<Version>>,
    //        type_list_t<
    //            BspTag,
    //            SubModelTag,
    //            ModelTag,
    //            ObjectSpawnTag,
    //            MultiplayerSpawnTag>,
    //        empty_list_t>;

    using Proxy = Components::proxy_of<MeshRendererManifest<Version>>;

    using draw_data_t = gfx::draw_command::data_t;

    struct Pass
    {
        Pass()
        {
        }

        //        GFX::RenderPass     source;
        //        GFX::OPT_DRAW       draw;
        //        PIP_PARAM*          pipeline;
        //        Map<u64, PIP_PARAM> format_states;
        gfx::draw_command                       command;
        std::map<u64, std::vector<draw_data_t>> draws;

        gfx::buffer_slice_t material_buffer;
        gfx::buffer_slice_t matrix_buffer;
        Span<char>          material_mapping;
        Span<Matf4>         matrix_mapping;

        auto& draw(u64 fmt)
        {
            return draws[fmt];
        }

        model_tracker_t insert_draw(draw_data_t const& draw, u64 bucket = 0)
        {
            auto& bucket_ = draws[bucket];
            auto  it      = std::find_if(
                bucket_.begin(), bucket_.end(), [&draw](draw_data_t const& d) {
                    return d.elements.offset == draw.elements.offset;
                });
            if(it != bucket_.end())
            {
                return model_tracker_t{
                    .draw     = static_cast<u16>(it - bucket_.begin()),
                    .instance = static_cast<u16>(it->instances.count++),
                };
            } else
            {
                bucket_.push_back(draw);
                return model_tracker_t{
                    .draw     = static_cast<u16>(bucket_.size() - 1),
                    .instance = 0,
                };
            }
        }

        inline void clear()
        {
            //            command.data.clear();
            draws.clear();
        }

        template<typename T>
        inline T& material_of(size_t idx)
        {
            auto material = mem_chunk<T>::ofContainer(material_mapping);
            return material[idx];
        }
    };

    time_point last_update{};

    gfx::api*      m_api;
    BlamResources& m_resources;
    BlamCamera&    m_camera;

    ShaderCache<Version>& shader_cache;
    BitmapCache<Version>& bitm_cache;
    BSPCache<Version>&    bsp_cache;

    Array<Pass, Pass_Count> m_bsp;
    Array<Pass, Pass_Count> m_model;

    MeshRenderer(
        gfx::api*             api,
        BlamResources&        resources,
        BlamCamera&           camera,
        ShaderCache<Version>& shader_cache,
        BitmapCache<Version>& bitm_cache,
        BSPCache<Version>&    bsp_cache) :
        m_api(api),
        m_resources(resources), m_camera(camera), shader_cache(shader_cache),
        bitm_cache(bitm_cache), bsp_cache(bsp_cache)
    {
        this->priority = 3072;

        for(auto& pass : m_bsp)
            pass.command.program = resources.bsp_pipeline;
        for(auto& pass : m_model)
            pass.command.program = resources.model_pipeline;

        gfx::buffer_t& material_buf = *resources.material_store;
        gfx::buffer_t& matrix_buf   = *resources.model_matrix_store;

        const auto uses_ubo         = !api->feature_info().buffer.ssbo;
        const auto bsp_batch_size   = uses_ubo ? 64_kB : 1024_kB;
        const auto model_batch_size = uses_ubo ? 64_kB : 2048_kB;

        u32 base = 0;
        for(Pass& pass : m_bsp)
        {
            pass.material_buffer = material_buf.slice(base, bsp_batch_size);
            base += bsp_batch_size;
        }
        base         = m_bsp.size() * bsp_batch_size;
        u32 base_mat = 0;
        for(Pass& pass : m_model)
        {
            pass.material_buffer = material_buf.slice(base, model_batch_size);
            pass.matrix_buffer
                = matrix_buf.slice(base_mat, model_batch_size / 2);

            base += model_batch_size;
            base_mat += model_batch_size / 2;
        }
    }

    ShaderItem const* get_shader(generation_idx_t shader)
    {
        if(!shader.valid())
            return nullptr;
        auto it = shader_cache.find(shader);
        if(it == shader_cache.m_cache.end())
            return nullptr;
        return &it->second;
    }
    BitmapItem const* get_bitm(generation_idx_t bitm)
    {
        if(!bitm.valid())
            return nullptr;
        auto it = bitm_cache.find(bitm);
        if(it == bitm_cache.m_cache.end())
            return nullptr;
        return &it->second;
    }
    PixDesc get_bitm_format(generation_idx_t bitm)
    {
        BitmapItem const* item = get_bitm(bitm);
        if(!item)
            return PixDesc();
        else
            return item->image.fmt;
    }
    template<class T>
    std::shared_ptr<gfx::sampler_t> get_bitm_bucket(generation_idx_t bitm)
    {
        return bitm_cache.template get_bucket<T>(get_bitm(bitm)->image.fmt)
            .sampler;
    }
    BSPItem const* get_bsp(generation_idx_t bsp)
    {
        if(!bsp.valid())
            return nullptr;
        auto it = bsp_cache.find(bsp);
        if(it == bsp_cache.m_cache.end())
            return nullptr;
        return &it->second;
    }

    void setup_textures(std::vector<gfx::sampler_definition_t>& samplers)
    {
        samplers.clear();

        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"lightmaps"sv, 4},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(
                    PixDesc(PixFmt::RGB565))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc1"sv, 0},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC1))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc2"sv, 1},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC2))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc3"sv, 2},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC3))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgb565"sv, 3},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(
                    PixDesc(PixFmt::RGB565))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_r8"sv, 5},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(PixDesc(PixFmt::R8))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rg8"sv, 6},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(PixDesc(PixFmt::RG8))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgba4"sv, 7},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(PixDesc(PixFmt::RGBA4))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgba8"sv, 8},
            bitm_cache
                .template get_bucket<gfx::texture_2da_t>(PixDesc(PixFmt::RGBA8))
                .sampler});
    }

    auto get_view_state()
    {
        using typing::vector_types::Vecd2;
        using typing::vector_types::Veci4;
        return gfx::view_state{
            .view  = Veci4{
                0, 0,
                m_resources.offscreen_size.w,
                m_resources.offscreen_size.h,
            },
            .depth = Vecd2{0.000001, 100.},
        };
    }

    template<typename... Args>
    void render_pass(Proxy&, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 0},
                semantic::SpanOne<const Matf4>(m_camera.camera_matrix)});
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MatrixStore"sv, 0},
                pass.matrix_buffer},
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MaterialProperties"sv, 1},
                pass.material_buffer});

        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        for(auto const& [fmt, draw] : pass.draws)
        {
            m_api->submit(
                {
                    .program       = pass.command.program,
                    .vertices      = pass.command.vertices,
                    .render_target = m_resources.offscreen,
                    .call          = pass.command.call,
                    .data          = draw,
                },
                vertex_u,
                buffers,
                get_view_state(),
                samplers,
                std::forward<Args&&>(extra)...);
        }
    }

    template<typename... Args>
    void render_bsp_pass(Proxy&, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        /* Step 1: Set up shared uniform state + buffers */
        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1},
                semantic::SpanOne<const Matf4>(m_camera.camera_matrix)});
        [[maybe_unused]] auto fragment_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"camera_position", 20},
                semantic::SpanOne<const Vecf3>(m_camera.camera.position)});
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MatrixStore"sv, 0},
                pass.matrix_buffer},
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MaterialProperties"sv, 1},
                pass.material_buffer});

        /* Step 2: Set up all the textures */
        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        for(auto const& [fmt, draw] : pass.draws)
        {
            /* Step 3: DRAW */
            auto res = m_api->submit(
                {
                    .program       = pass.command.program,
                    .vertices      = pass.command.vertices,
                    .render_target = m_resources.offscreen,
                    .call          = pass.command.call,
                    .data          = draw,
                },
                vertex_u,
                //                fragment_u,
                buffers,
                get_view_state(),
                gfx::cull_state{
                    .front_face = true,
                },
                samplers,
                std::forward<Args&&>(extra)...);

            if(res.has_value())
            {
                //                auto [error, msg] = res.value();
                //                cFatal(
                //                    "Draw error: {0}: {1}: {2}",
                //                    0
                //                    /*pass.shader->tag->to_name().to_string(
                //                        m_data.map_container.magic)*/
                //                    ,
                //                    magic_enum::enum_name(error),
                //                    msg);
            }
        }
        //        if(!pass.source.pipeline.lock())
        //            return;

        //        GFX::ERROR ec;

        //        if(pass.matrix_buffer_range.second)
        //            m_data.model_matrix_store->bindrange(
        //                0,
        //                pass.matrix_buffer_range.first,
        //                pass.matrix_buffer_range.second,
        //                ec);
        //        C_ERROR_CHECK(ec);
        //        m_data.material_store->bindrange(
        //            1,
        //            pass.material_buffer_range.first,
        //            pass.material_buffer_range.second,
        //            ec);
        //        C_ERROR_CHECK(ec);

        //        GFX::SetRasterizerState(pass.source.raster);
        //        GFX::SetBlendState(pass.source.blend);
        //        GFX::MultiDraw(*pass.source.pipeline.lock(), pass.draw);
    }

    void render_bsp_portals(Proxy& e)
    {
        for(auto& ent : e.select(ObjectBsp))
        {
            auto           ref  = e.template ref<Proxy>(ent);
            BspReference&  bsp_ = ref.template get<BspReference>();
            BSPItem const* bsp  = get_bsp(bsp_.bsp);

            m_api->submit({
                    .program = m_resources.debug_lines_pipeline,
                    .vertices = m_resources.debug_attr,
                    .render_target = m_resources.offscreen,
                    .call = gfx::draw_command::call_spec_t{
                        .indexed = false,
                        .mode = gfx::drawing::primitive::line_strip,
                    },
                    .data = bsp->portals,
                },
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair{
                        {"camera"sv, 0},
                        semantic::SpanOne<const Matf4>(
                            m_camera.camera_matrix),
                    }),
                get_view_state());
        }
    }

    bool test_visible(Matf4 const& mat)
    {
        Vecf4 probe = {0, 0, 0, 1};
        Vecf4 out   = probe * (m_camera.camera_matrix * mat);

        Vecf3 norm = {out.x() / out.w(), out.y() / out.w(), out.z() / out.w()};

        scalar margin = 1.f;

        return norm.x() < margin && norm.y() < margin && norm.z() < margin
               && norm.x() > -margin && norm.y() > -margin
               && norm.z() > -margin;
    }

    void start_restricted(Proxy& p, time_point const& time)
    {
        ProfContext _;
        bool        invalidated
            = p.template subsystem<BlamFiles>().last_updated > last_update;
        if(time - last_update > Chrono::seconds(5) || invalidated)
        {
            generate_draws(p);
            last_update = time;
        }

        RenderingParameters const* rendering_props;
        p.subsystem(rendering_props);

        for(auto const& pass : slice_num(m_bsp, Pass_LastOpaque + 1))
        {
            render_bsp_pass(p, pass);
        }

        if(rendering_props->render_scenery)
            for(auto const& pass : slice_num(m_model, Pass_LastOpaque + 1))
                render_pass(p, pass);

        if(rendering_props->render_scenery)
            render_pass(
                p,
                m_model[Pass_Lights],
                gfx::blend_state{
                    .additive = true,
                });
        if(rendering_props->render_scenery)
            render_pass(p, m_model[Pass_Glass], gfx::blend_state{});
        render_bsp_pass(
            p,
            m_bsp[Pass_Lights],
            gfx::blend_state{
                .additive = true,
            });
        render_bsp_pass(p, m_bsp[Pass_Glass], gfx::blend_state{});

        render_bsp_pass(p, m_bsp[Pass_Wireframe]);

        //        render_bsp_portals(p);
    }

    void end_restricted(Proxy& /*p*/, time_point const& /*time*/)
    {
    }

    void generate_draws(Proxy& p)
    {
        ProfContext _;
        //        BitmapCache<Version>& bitm
        //            = p.template subsystem<BitmapCache<Version>>();
        //        ShaderCache<Version>& shaders
        //            = p.template subsystem<ShaderCache<Version>>();

        i32 instance_offset = 0;

        for(Pass& pass : m_bsp)
        {
            pass.clear();
            pass.material_mapping
                = pass.material_buffer.template buffer_cast<char>();
        }
        for(Pass& pass : m_model)
        {
            pass.clear();
            pass.material_mapping
                = pass.material_buffer.template buffer_cast<char>();
            pass.matrix_mapping
                = pass.matrix_buffer.template buffer_cast<Matf4>();
        }

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref = p.template ref<Proxy>(ent);
            BspReference& bsp = ref.template get<BspReference>();

            if(!bsp.visible)
                continue;

            //            auto& shader = *shaders.find(bsp.shader);
            auto bucket = 0;

            Pass& wf            = m_bsp[bsp.current_pass];
            wf.command.vertices = m_resources.bsp_attr;
            wf.command.call     = {
                    .indexed   = true,
                    .instanced = true,
                    .mode      = gfx::drawing::primitive::triangle,
            };
            populate_bsp_material(bsp, instance_offset);
            wf.draw(bucket).push_back(bsp.draw.data.front());
            draw_data_t& draw     = wf.draw(bucket).back();
            draw.instances.offset = instance_offset;
            instance_offset += draw.instances.count;
        }

        for(auto& ent : p.select(ObjectMod2))
        {
            auto              ref   = p.template ref<Proxy>(ent);
            SubModel&         model = ref.template get<SubModel>();
            MeshTrackingData& track = ref.template get<MeshTrackingData>();

            //            auto& shader = *shaders.find(model.shader);

            Pass& wf            = m_model[model.current_pass];
            wf.command.vertices = m_resources.model_attr;
            wf.command.call     = {
                    .indexed   = true,
                    .instanced = true,
                    .mode      = gfx::drawing::primitive::triangle_strip,
            };
            track.model_id = wf.insert_draw(model.draw.data.front(), 0);
        }

        cDebug("Models:");
        for(Pass const& pass : m_model)
        {
            cDebug(" - Pass: {0} buckets", pass.draws.size());
            for(auto const& [_, bucket] : pass.draws)
                for(draw_data_t const& draw : bucket)
                    cDebug(
                        "    - offset={0} count={1}",
                        draw.instances.offset,
                        draw.instances.count);
        }

        instance_offset = 0;

        for(Pass& pass : m_model)
            for(auto& [fmt, bucket] : pass.draws)
                for(draw_data_t& draw : bucket)
                {
                    draw.instances.offset = instance_offset;
                    instance_offset += draw.instances.count;
                }

        for(auto& ent : p.select(ObjectMod2))
        {
            auto      ref    = p.template ref<Proxy>(ent);
            SubModel& smodel = ref.template get<SubModel>();
            Model&    model
                = p.template ref<Proxy>(smodel.parent).template get<Model>();
            MeshTrackingData&  track = ref.template get<MeshTrackingData>();
            Pass&              pass  = m_model[smodel.current_pass];
            draw_data_t const& draw  = pass.draws[0].at(track.model_id.draw);
            auto instance_id = draw.instances.offset + track.model_id.instance;
            pass.matrix_mapping[instance_id] = model.transform;
            populate_mod2_material(smodel, instance_id);
        }

        m_resources.material_store->unmap();

        cDebug("Summary of generation:");
        for(Pass const& pass : m_bsp)
        {
            cDebug(" - Pass: {0} buckets", pass.draws.size());
        }

        //        m_bsp[Pass_EnvMicro].draws().resize(10);
    }

    void populate_bsp_material(BspReference& ref, size_t i = 0)
    {
        Pass&                  pass = m_bsp[ref.current_pass];
        materials::senv_micro& material
            = pass.template material_of<materials::senv_micro>(i);
        shader_cache.populate_material(material, ref.shader);
        bitm_cache.assign_atlas_data(material.lightmap, ref.lightmap);
    }

    void populate_mod2_material(SubModel& sub, size_t i = 0)
    {
        Pass&                  pass = m_model[sub.current_pass];
        materials::senv_micro& material
            = pass.template material_of<materials::senv_micro>(i);
        shader_cache.populate_material(material, sub.shader);
    }
};

struct ScreenClear
    : compo::RestrictedSubsystem<ScreenClear, ScreenClearManifest>
{
    using type  = ScreenClear;
    using Proxy = compo::proxy_of<ScreenClearManifest>;

    ScreenClear()
    {
        priority = 1536;
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        auto fb = e.subsystem<gfx::system>().default_rendertarget();
        //        fb->clear(Vecf4{Vecf3{0.1f}, 1.f}, 1.0, 0);

        e.subsystem<BlamResources>().offscreen->clear(1.0);
    }

    void end_restricted(Proxy& e, time_point const&)
    {
        auto& api         = e.subsystem<gfx::system>();
        auto& resources   = e.subsystem<BlamResources>();
        auto& postprocess = e.subsystem<PostProcessParameters>();

        if(api.default_rendertarget() == resources.offscreen)
            return;
        if(!quad_program)
        {
            load_resources(api, e.subsystem<BlamResources>());
        }

        auto params = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{{"gamma"sv}, SpanOne(postprocess.gamma)},
            gfx::uniform_pair{{"exposure"sv}, SpanOne(postprocess.exposure)});

        // clang-format off
        api.submit(gfx::draw_command{
                .program = quad_program,
                .vertices = quad_vao,
                .call = {
                    .indexed = false,
                    .mode = gfx::drawing::primitive::triangle_fan,
                },
                .data = {{ .arrays = { .count = 4, }}},
            },
            gfx::make_sampler_list(gfx::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source"sv},
                offscreen_sampler
            }),
            std::move(params));
        // clang-format on
    }

    void load_resources(gfx::system& api, BlamResources& resources)
    {
        using vecb4 = typing::vectors::tvector<i8, 4>;
        using vecb2 = typing::vectors::tvector<i8, 2>;
        struct vertex_t
        {
            vecb2 pos;
            vecb2 tex;
        };

        quad_vbo = api.alloc_buffer(gfx::buffers::vertex, RSCA::ReadOnly);
        quad_vbo->alloc();
        std::array<vecb4, 6> vbo = {{
            vecb4{-127, -127, 0, 0},
            vecb4{127, -127, 127, 0},
            vecb4{127, 127, 127, 127},
            vecb4{-127, 127, 0, 127},

        }};
        quad_vbo->commit(vbo);
        quad_vao = api.alloc_vertex_array();
        quad_vao->alloc();
        quad_vao->add(gfx::vertex_attribute::from_member(
            &vertex_t::pos, gfx::vertex_float_type));
        auto tex = gfx::vertex_attribute::from_member(
            &vertex_t::tex, gfx::vertex_float_type);
        tex.index = 1;
        quad_vao->add(tex);
        quad_vao->set_buffer(gfx::buffers::vertex, quad_vbo, 0);
        quad_vao->set_attribute_names({
            {"pos", 0},
            {"tex", 1},
        });

        constexpr std::string_view vertex_shader   = R"(#version 100
precision highp float;
attribute vec2 pos;
attribute vec2 tex;
varying vec2 in_tex;
void main()
{
    in_tex = tex;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
)";
        constexpr std::string_view fragment_shader = R"(#version 100
precision highp float;
precision highp sampler2D;
varying vec2 in_tex;
uniform sampler2D source;
uniform float gamma;
uniform float exposure;
void main()
{
    vec3 color = texture2D(source, in_tex).rgb;
    color = color / (color + vec3(1.0));
    color = pow(exposure * color, vec3(1.0 / gamma));
    gl_FragColor = vec4(color, 1.0);
}
)";

        quad_program = api.alloc_program();
        quad_program->add(
            gfx::program_t::stage_t::Vertex,
            api.alloc_shader(
                mem_chunk<const char>::ofContainer(vertex_shader)));
        quad_program->add(
            gfx::program_t::stage_t::Fragment,
            api.alloc_shader(
                mem_chunk<const char>::ofContainer(fragment_shader)));
        if(auto res = quad_program->compile(); res.has_error())
            cDebug("Error compiling quad shader: {0}", res.error());

        offscreen_sampler = resources.color->sampler();
        offscreen_sampler->alloc();
    }

    std::shared_ptr<gfx::buffer_t>       quad_vbo;
    std::shared_ptr<gfx::vertex_array_t> quad_vao;
    std::shared_ptr<gfx::program_t>      quad_program;
    std::shared_ptr<gfx::sampler_t>      offscreen_sampler;
};

void alloc_renderer(EntityContainer& container)
{
    container.register_subsystem_inplace<MeshRenderer<halo_version>>(
        &container.subsystem_cast<gfx::system>(),
        std::ref(container.subsystem_cast<BlamResources>()),
        std::ref(container.subsystem_cast<BlamCamera>()),
        std::ref(container.subsystem_cast<ShaderCache<halo_version>>()),
        std::ref(container.subsystem_cast<BitmapCache<halo_version>>()),
        std::ref(container.subsystem_cast<BSPCache<halo_version>>()));

    container.register_subsystem_inplace<ScreenClear>();
}
