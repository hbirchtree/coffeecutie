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

using typing::pixels::CompFmt;
using typing::pixels::PixDesc;

namespace detail {

inline std::tuple<PixFmt, CompFlags> get_bitm_hash(BitmapItem const& bitm)
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
    using type        = MeshRenderer;
    using Proxy       = Components::proxy_of<MeshRendererManifest<Version>>;
    using draw_data_t = gfx::draw_command::data_t;

    struct Pass
    {
        Pass()
        {
            draws.emplace_back();
        }

        gfx::draw_command                     command;
        std::vector<std::vector<draw_data_t>> draws;

        gfx::buffer_slice_t material_buffer;
        gfx::buffer_slice_t matrix_buffer;
        Span<char>          material_mapping;
        Span<Matf4>         matrix_mapping;

        model_tracker_t insert_draw(draw_data_t const& draw)
        {
            size_t num_draws = 0;
            for(auto const& draw : draws.back())
                num_draws += draw.instances.count;

            if(num_draws >= 128 || (num_draws + draw.instances.count) > 128)
                draws.emplace_back();

            auto& bucket_ = draws.back();
            auto  it      = std::find_if(
                bucket_.begin(), bucket_.end(), [&draw](draw_data_t const& d) {
                    return d.elements.offset == draw.elements.offset;
                });
            if(it != bucket_.end())
            {
                return model_tracker_t{
                    .bucket   = static_cast<u16>(draws.size() - 1),
                    .draw     = static_cast<u16>(it - bucket_.begin()),
                    .instance = static_cast<u16>(it->instances.count++),
                    .enabled  = true,
                };
            } else
            {
                bucket_.push_back(draw);
                return model_tracker_t{
                    .bucket   = static_cast<u16>(draws.size() - 1),
                    .draw     = static_cast<u16>(bucket_.size() - 1),
                    .instance = 0,
                    .enabled  = true,
                };
            }
        }

        inline void clear()
        {
            //            command.data.clear();
            draws.clear();
            draws.emplace_back();
        }

        template<typename T>
        inline T& material_of(size_t idx)
        {
            auto material
                = semantic::mem_chunk<T>::ofContainer(material_mapping);
            if(idx >= material.size)
                Throw(std::out_of_range("material index out of range"));
            return material[idx];
        }

        inline u32 num_objects() const
        {
            u32 total = 0;
            for(auto const& bucket : draws)
                for(auto const& draw : bucket)
                    total += draw.instances.count;
            return total;
        }
    };

    time_point last_update{};

    gfx::api*      m_api;
    BlamResources& m_resources;
    BlamCamera&    m_camera;

    ShaderCache<Version>& shader_cache;
    BitmapCache<Version>& bitm_cache;
    BSPCache<Version>&    bsp_cache;

    std::array<Pass, Pass_Count> m_bsp;
    std::array<Pass, Pass_Count> m_model;

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

        const auto bsp_batch_size       = 1024_kB;
        const auto model_batch_size     = 1024_kB;
        const auto model_mat_batch_size = 256_kB;

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
                = matrix_buf.slice(base_mat, model_mat_batch_size);

            base += model_batch_size;
            base_mat += model_mat_batch_size;
        }
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
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::RGB565))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc1"sv, 0},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC1))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc2"sv, 1},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC2))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_bc3"sv, 2},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC3))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgb565"sv, 3},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::RGB565))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_r8"sv, 5},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::R8))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rg8"sv, 6},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::RG8))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgba4"sv, 7},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::RGBA4))
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_rgba8"sv, 8},
            bitm_cache
                .template get_bucket<gfx::compat::texture_2da_t>(
                    PixDesc(PixFmt::RGBA8))
                .sampler});
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION_ES >= 0x320
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_cube_bc1"sv, 9},
            bitm_cache
                .template get_bucket<gfx::texture_cube_array_t>(
                    CompFmt(PixFmt::BCn, CompFlags::BC1),
                    blam::bitm::type_t::tex_cube)
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_cube_rgb565"sv, 10},
            bitm_cache
                .template get_bucket<gfx::texture_cube_array_t>(
                    PixDesc(PixFmt::RGB565), blam::bitm::type_t::tex_cube)
                .sampler});
        samplers.push_back(gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {"source_cube_rgba8"sv, 11},
            bitm_cache
                .template get_bucket<gfx::texture_cube_array_t>(
                    PixDesc(PixFmt::RGBA8), blam::bitm::type_t::tex_cube)
                .sampler});
#endif
    }

    auto get_view_state()
    {
        using typing::vector_types::Vecd2;
        using typing::vector_types::Veci4;

        const auto depth = gfx::depth_state{
            .range    = Vecd2{0.1, 1000.},
            .reversed = true,
        };

        if constexpr(compile_info::platform::is_emscripten)
            return gfx::view_state{
                .depth = gfx::depth_state{
                    .range    = depth.range,
                    .reversed = true,
                }};

        return gfx::view_state{
            .view  = Veci4{
                0, 0,
                m_resources.offscreen_size.x,
                m_resources.offscreen_size.y,
            },
            .depth = depth,
        };
    }

    template<typename... Args>
    void render_pass(Proxy&, f32 t, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1},
                semantic::SpanOne<const Matf4>(m_camera.camera_matrix)});
        auto fragment_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"camera_position", 21},
                semantic::SpanOne<const Vecf3>(m_camera.camera.position),
            },
            gfx::uniform_pair{
                {"time", 22},
                semantic::SpanOne<const f32>(t),
            });
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MatrixStore"sv, 0},
                pass.matrix_buffer,
                sizeof(Matf4),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MaterialProperties"sv, 1},
                pass.material_buffer,
                sizeof(materials::senv_micro),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"WorldProperties"sv, 2},
                m_resources.world_store->slice(0),
                0,
            });

        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        for(auto const& draw : pass.draws)
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
                fragment_u,
                buffers,
                get_view_state(),
                samplers,
                std::forward<Args&&>(extra)...);
        }
    }

    template<typename... Args>
    void render_bsp_pass(Proxy&, f32 t, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        /* Step 1: Set up shared uniform state + buffers */
        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1},
                semantic::SpanOne<const Matf4>(m_camera.camera_matrix)});
        auto fragment_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"camera_position", 21},
                semantic::SpanOne<const Vecf3>(m_camera.camera.position),
            },
            gfx::uniform_pair{
                {"time", 22},
                semantic::SpanOne<const f32>(t),
            });
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"MaterialProperties"sv, 1},
                pass.material_buffer,
                sizeof(materials::senv_micro),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"WorldProperties"sv, 2},
                m_resources.world_store->slice(0),
                0,
            });

        /* Step 2: Set up all the textures */
        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        for(auto const& draw : pass.draws)
        {
            /* Step 3: DRAW */
            m_api->submit(
                {
                    .program       = pass.command.program,
                    .vertices      = pass.command.vertices,
                    .render_target = m_resources.offscreen,
                    .call          = pass.command.call,
                    .data          = draw,
                },
                vertex_u,
                fragment_u,
                buffers,
                get_view_state(),
                gfx::cull_state{
                    .front_face = true,
                },
                samplers,
                std::forward<Args&&>(extra)...);
        }
    }

    void render_debug_lines(Proxy& e)
    {
        std::vector<gfx::draw_command::data_t> groups;
        RenderingParameters*                   params;
        e.subsystem(params);

        Span<Vecf3> colors = m_resources.debug_line_colors->map<Vecf3>(0);

        for(auto& ent : e.select(ObjectBsp))
        {
            if(!params->debug_portals)
                break;
            auto           ref  = e.template ref<Proxy>(ent);
            BspReference&  bsp_ = ref.template get<BspReference>();
            BSPItem const* bsp  = get_bsp(bsp_.bsp);

            groups.insert(
                groups.end(), bsp->portals.begin(), bsp->portals.end());
        }
        for(auto& ent : e.template select<DebugDraw>())
        {
            if(!params->debug_triggers && ent.tags & ObjectTriggerVolume)
                continue;
            auto             ref  = e.template ref<Proxy>(ent);
            DebugDraw const& draw = ref.template get<DebugDraw>();
            groups.push_back(draw.data);
            groups.back().instances.offset = draw.color_ptr;
            if(ent.tags & ObjectTriggerVolume)
                colors[draw.color_ptr]
                    = draw.selected ? Vecf3{0, 1, 0} : Vecf3{1};
        }

        m_resources.debug_line_colors->unmap();

        m_api->submit({
                .program = m_resources.debug_lines_pipeline,
                .vertices = m_resources.debug_attr,
                .render_target = m_resources.offscreen,
                .call = gfx::draw_command::call_spec_t{
                    .indexed = false,
                    .mode = gfx::drawing::primitive::line_strip,
                },
                .data = groups,
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

    bool test_visible(Matf4 const& mat)
    {
        Vecf4 probe = {0, 0, 0, 1};
        Vecf4 out   = probe * (m_camera.camera_matrix * mat);

        Vecf3 norm = {out.x / out.w, out.y / out.w, out.z / out.w};

        scalar margin = 1.f;

        return norm.x < margin && norm.y < margin && norm.z < margin
               && norm.x > -margin && norm.y > -margin && norm.z > -margin;
    }

    void start_restricted(Proxy& p, time_point const& time)
    {
        ProfContext _;

        bool invalidated
            = p.template subsystem<BlamFiles>().last_updated > last_update;
        invalidated = true;
        if(time - last_update > std::chrono::seconds(5) || invalidated)
        {
            generate_draws(p);
            last_update = time;
        }

        update_materials(p, time);

        m_resources.material_store->unmap();

        RenderingParameters const* rendering_props;
        p.subsystem(rendering_props);

        if(rendering_props->debug_clear)
            m_resources.offscreen->clear(Vecf4(0, 0, 0, 1));

        f32 t = stl_types::Chrono::to_float(time);

        for(auto const& pass : stl_types::slice_num(m_bsp, Pass_LastOpaque + 1))
        {
            render_bsp_pass(p, t, pass, gfx::cull_state{.front_face = true});
        }

        for(auto const& pass :
            stl_types::slice_num(m_model, Pass_LastOpaque + 1))
            render_pass(p, t, pass, gfx::cull_state{.front_face = true});

        gfx::depth_extended_state nowrite = {.depth_write = true};

        render_pass(
            p,
            t,
            m_model[Pass_Additive],
            gfx::blend_state{.additive = true},
            nowrite);
        render_pass(
            p,
            t,
            m_model[Pass_Multiply],
            gfx::blend_state{.multiply = true},
            nowrite);
        render_pass(p, t, m_model[Pass_Glass], gfx::blend_state{}, nowrite);
        render_bsp_pass(
            p,
            t,
            m_bsp[Pass_Additive],
            gfx::blend_state{.additive = true},
            nowrite);
        render_bsp_pass(p, t, m_bsp[Pass_Glass], gfx::blend_state{}, nowrite);

        //        render_bsp_pass(p, m_bsp[Pass_Wireframe]);

        render_debug_lines(p);
    }

    void end_restricted(Proxy& /*p*/, time_point const& /*time*/)
    {
    }

    void generate_static_draws(Proxy& p)
    {
        for(Pass& pass : m_bsp)
        {
            pass.clear();
            pass.material_mapping
                = pass.material_buffer.template buffer_cast<char>();
        }

        std::map<Passes, i32> instance_offsets;
        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref = p.template ref<Proxy>(ent);
            BspReference& bsp = ref.template get<BspReference>();

            if(!bsp.visible)
                continue;

            Pass& wf              = m_bsp[bsp.current_pass];
            i32&  instance_offset = instance_offsets[bsp.current_pass];
            wf.command.vertices   = m_resources.bsp_attr;
            wf.command.call       = {
                      .indexed   = true,
                      .instanced = true,
                      .mode      = gfx::drawing::primitive::triangle,
            };
            populate_bsp_material(bsp, instance_offset);
            bsp.draw.data.front().instances.offset = instance_offset;
            instance_offset += bsp.draw.data.front().instances.count;
            wf.draws[0].push_back(bsp.draw.data.front());
        }
    }

    void generate_draws(Proxy& p)
    {
        ProfContext _;
        //        BitmapCache<Version>& bitm
        //            = p.template subsystem<BitmapCache<Version>>();
        //        ShaderCache<Version>& shaders
        //            = p.template subsystem<ShaderCache<Version>>();

        RenderingParameters* rendering_params;
        p.subsystem(rendering_params);

        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        generate_static_draws(p);

        for(Pass& pass : m_model)
        {
            pass.clear();
            pass.material_mapping
                = pass.material_buffer.template buffer_cast<char>();
            pass.matrix_mapping
                = pass.matrix_buffer.template buffer_cast<Matf4>();
        }

        for(compo::Entity const& ent : p.select(ObjectMod2))
        {
            auto              ref    = p.template ref<Proxy>(ent);
            SubModel&         model  = ref.template get<SubModel>();
            MeshTrackingData& track  = ref.template get<MeshTrackingData>();
            auto              parent = p.template ref<Proxy>(model.parent);
            Model&            mod    = parent.template get<Model>();

            if(!mod.visible
               || (!rendering_params->render_scenery
                   && (ent.tags & ObjectSkybox) == 0))
            {
                track.model_id = {};
                continue;
            }

            Pass& wf            = m_model[model.current_pass];
            wf.command.vertices = m_resources.model_attr;
            wf.command.call     = {
                    .indexed   = true,
                    .instanced = true,
                    .mode      = gfx::drawing::primitive::triangle_strip,
            };
            track.model_id = wf.insert_draw(model.draw.data.front());
        }

        cDebug("Models:");
        for(Pass const& pass : m_model)
        {
            cDebug(" - Pass: {0} buckets", pass.draws.size());
            for(auto const& bucket : pass.draws)
                for(draw_data_t const& draw : bucket)
                    cDebug(
                        "    - offset={0} count={1}",
                        draw.instances.offset,
                        draw.instances.count);
        }

        for(Pass& pass : m_model)
        {
            i32 instance_offset = 0;
            for(auto& bucket : pass.draws)
                for(draw_data_t& draw : bucket)
                {
                    draw.instances.offset = instance_offset;
                    instance_offset += draw.instances.count;
                }
        }

        for(auto& ent : p.select(ObjectMod2))
        {
            if(!rendering_params->render_scenery
               && (ent.tags & ObjectSkybox) == 0)
                continue;
            auto      ref    = p.template ref<Proxy>(ent);
            SubModel& smodel = ref.template get<SubModel>();
            Model&    model
                = p.template ref<Proxy>(smodel.parent).template get<Model>();
            MeshTrackingData& track = ref.template get<MeshTrackingData>();

            if(!track.model_id.enabled)
                continue;

            Pass&              pass = m_model[smodel.current_pass];
            draw_data_t const& draw
                = pass.draws[track.model_id.bucket].at(track.model_id.draw);
            auto instance_id = draw.instances.offset + track.model_id.instance;
            pass.matrix_mapping[instance_id] = model.transform;
            populate_mod2_material(
                smodel, model_cache->find(model.model)->second, instance_id);
        }

        m_resources.model_matrix_store->unmap();

        cDebug("Summary of generation:");
        for(Pass const& pass : m_bsp)
        {
            cDebug(" - Pass: {0} buckets", pass.draws.size());
        }

        //        m_bsp[Pass_EnvMicro].draws().resize(10);
    }

    void update_materials(Proxy& p, time_point const& time)
    {
        for(Pass& pass : m_bsp)
        {
            pass.material_mapping
                = pass.material_buffer.template buffer_cast<char>();
        }
        for(Pass& pass : m_model)
        {
            pass.material_mapping
                = pass.material_buffer.template buffer_cast<char>();
        }

        RenderingParameters* rendering_params;
        p.subsystem(rendering_params);

        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        for(auto& ent : p.select(ObjectMod2))
        {
            if(!rendering_params->render_scenery
               && (ent.tags & ObjectSkybox) == 0)
                continue;
            auto              ref    = p.template ref<Proxy>(ent);
            SubModel&         smodel = ref.template get<SubModel>();
            MeshTrackingData& track  = ref.template get<MeshTrackingData>();
            Pass&             pass   = m_model[smodel.current_pass];
            auto&             bucket = pass.draws[track.model_id.bucket];
            if(bucket.empty())
                continue;
            draw_data_t const& draw = bucket.at(track.model_id.draw);
            auto instance_id = draw.instances.offset + track.model_id.instance;
            update_animations(
                material_of(smodel, instance_id), smodel.shader, time);
        }

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref = p.template ref<Proxy>(ent);
            BspReference& bsp = ref.template get<BspReference>();

            if(!bsp.visible)
                continue;

            i32 instance_offset = bsp.draw.data.front().instances.offset;
            update_animations(
                material_of(bsp, instance_offset), bsp.shader, time);
        }
    }

    materials::senv_micro& material_of(SubModel& sub, size_t i)
    {
        Pass& pass = m_model[sub.current_pass];
        return pass.template material_of<materials::senv_micro>(i);
    }

    materials::senv_micro& material_of(BspReference& bsp, size_t i)
    {
        Pass& pass = m_bsp[bsp.current_pass];
        return pass.template material_of<materials::senv_micro>(i);
    }

    void populate_bsp_material(BspReference& ref, size_t i = 0)
    {
        Pass&                  pass = m_bsp[ref.current_pass];
        materials::senv_micro& material
            = pass.template material_of<materials::senv_micro>(i);
        shader_cache.populate_material(material, ref.shader, Vecf2{1, 1});
        bitm_cache.assign_atlas_data(material.lightmap, ref.lightmap);
    }

    void populate_mod2_material(
        SubModel const& sub, ModelItem<Version> const& model, size_t i = 0)
    {
        Pass&                  pass = m_model[sub.current_pass];
        materials::senv_micro& material
            = pass.template material_of<materials::senv_micro>(i);
        shader_cache.populate_material(
            material, sub.shader, model.header->uvscale);
    }

    void update_animations(
        materials::senv_micro&  material,
        generation_idx_t const& shader,
        time_point const&       time)
    {
        shader_cache.update_uv_animations(material, shader, time);
    }
};

void ScreenClear::start_restricted(Proxy& e, const time_point&)
{
    auto fb = e.subsystem<gfx::system>().default_rendertarget();
    //        fb->clear(Vecf4{Vecf3{0.1f}, 1.f}, 1.0, 0);

    e.subsystem<BlamResources>().offscreen->clear(0.0);
}

void ScreenClear::end_restricted(Proxy& e, const time_point&)
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

    f32 display_scale = postprocess.scale;

    Matf4 transform = glm::scale(
        glm::translate(glm::identity<Matf4>(), Vecf3{-1, -1, 0}),
        Vecf3{2, 2, 1});
    transform = glm::scale(transform, Vecf3{display_scale, display_scale, 1.f});
    Vecf2 uvscale{1.f, 1.f};
    Vecf2 offset{0, 0};

    auto params_v = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Vertex,
        gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)});
    auto params_f = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Fragment,
        gfx::uniform_pair{{"gamma"sv}, semantic::SpanOne(postprocess.gamma)},
        gfx::uniform_pair{{"scale"sv}, semantic::SpanOne(uvscale)},
        gfx::uniform_pair{{"offset"sv}, semantic::SpanOne(offset)},
        gfx::uniform_pair{
            {"exposure"sv}, semantic::SpanOne(postprocess.exposure)});

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
                    offscreen_sampler,
                }),
                params_v,
                std::move(params_f));
    // clang-format on

    comp_app::interfaces::GraphicsFramebuffer* framebuffer
        = e.service<comp_app::GraphicsFramebuffer>();

    Vecf2 item_scale{2.f / framebuffer->size().w, 2.f / framebuffer->size().h};
    f32   one = 1.f;

    params_f = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Fragment,
        gfx::uniform_pair{{"gamma"sv}, semantic::SpanOne(one)},
        gfx::uniform_pair{{"scale"sv}, semantic::SpanOne(uvscale)},
        gfx::uniform_pair{{"offset"sv}, semantic::SpanOne(offset)},
        gfx::uniform_pair{{"exposure"sv}, semantic::SpanOne(one)});

    for(screen_quad_t const& draw : extra_quads)
    {
        if(draw.sampler.expired())
            continue;

        Vecf3 translation(Vecf2(draw.position) * item_scale - 1.f, 0.f);
        Vecf3 scale(
            draw.size.x * item_scale.x, draw.size.y * item_scale.y, 1.f);

        uvscale = draw.atlas_scale;
        offset  = draw.atlas_offset;

        transform = glm::scale(
            glm::translate(glm::identity<Matf4>(), translation), scale);
        // clang-format off
        api.submit(gfx::draw_command{
                    .program = quad_program,
                    .vertices = quad_vao,
                    .call = {
                         .indexed = false,
                         .mode = gfx::drawing::primitive::triangle_fan,
                    },
                    .data = {{ .arrays = { .count = 4 }}},
                },
                gfx::make_sampler_list(gfx::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"source"sv},
                    draw.sampler.lock()
                }),
                params_v,
                params_f,
                gfx::blend_state{});
        // clang-format on
    }

    extra_quads.clear();
}

void ScreenClear::load_resources(gleam::system& api, BlamResources& resources)
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
        vecb4{0, 0, 0, 0},
        vecb4{127, 0, 127, 0},
        vecb4{127, 127, 127, 127},
        vecb4{0, 127, 0, 127},

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
    quad_vao->force_attribute_names();

    constexpr std::string_view vertex_shader   = R"(#version 100
precision highp float;
attribute vec2 pos;
attribute vec2 tex;
varying vec2 in_tex;
uniform mat4 transform;
void main()
{
    in_tex = tex;
    gl_Position = transform * vec4(pos.x, pos.y, 0.0, 1.0);
}
)";
    constexpr std::string_view fragment_shader = R"(#version 100
precision highp float;
precision highp sampler2D;
varying vec2 in_tex;
uniform sampler2D source;
uniform float gamma;
uniform float exposure;
uniform vec2 offset;
uniform vec2 scale;
void main()
{
    vec4 color = texture2D(source, offset + in_tex * scale).rgba;
    color.rgb = color.rgb / (color.rgb + vec3(1.0));
    color.rgb = pow(exposure * color.rgb, vec3(1.0 / gamma));
    gl_FragColor = color;
}
)";

    quad_program = api.alloc_program();
    quad_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(vertex_shader)));
    quad_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(fragment_shader)));
    if(auto res = quad_program->compile(); res.has_error())
        cDebug("Error compiling quad shader: {0}", res.error());

    offscreen_sampler = resources.color->sampler();
    offscreen_sampler->alloc();
}

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
