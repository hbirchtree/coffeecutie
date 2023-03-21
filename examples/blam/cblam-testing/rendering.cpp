#include "rendering.h"

#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <peripherals/stl/tuple_hash.h>

#include "caching.h"
#include "data.h"
#include "selected_version.h"

using namespace libc_types::size_literals;
using namespace std::string_view_literals;

constexpr auto bsp_batch_size
    = compile_info::platform::is_32bit ? 64_kB : 512_kB;
constexpr auto model_batch_size
    = compile_info::platform::is_32bit ? 64_kB : 1024_kB;

namespace detail {

inline Tup<PixFmt, CompFlags> get_bitm_hash(BitmapItem const& bitm)
{
    return std::make_tuple(bitm.image.fmt.pixfmt, bitm.image.fmt.cmpflg);
}

template<typename Version>
inline u64 shader_hash(ShaderItem const& shader, ShaderCache<Version>& cache)
{
    using blam::tag_class_t;

    auto& bitm = cache.bitm_cache;

    //    switch(shader.tag->tagclass_e[0])
    switch(tag_class_t::senv)
    {
    case tag_class_t::senv: {
        BitmapItem const& base = bitm.find(shader.senv.base_bitm)->second;

        Array<Tup<PixFmt, CompFlags>, 3> maps = {};

        if(shader.senv.micro_bitm.valid())
        {
            BitmapItem const& map = bitm.find(shader.senv.micro_bitm)->second;
            maps[0]               = get_bitm_hash(map);
        }
        if(shader.senv.primary_bitm.valid())
        {
            BitmapItem const& map = bitm.find(shader.senv.primary_bitm)->second;
            maps[1]               = get_bitm_hash(map);
        }
        if(shader.senv.secondary_bitm.valid())
        {
            BitmapItem const& map
                = bitm.find(shader.senv.secondary_bitm)->second;
            maps[2] = get_bitm_hash(map);
        }

        auto fmt_hash
            = std::make_tuple(get_bitm_hash(base), maps[0], maps[1], maps[2]);
        return std::hash<decltype(fmt_hash)>()(fmt_hash);
    }
    default:
        break;
    }

    /* Default: use format of color bitmap */
    {
        auto fmt_hash = std::make_tuple(
            get_bitm_hash(bitm.find(shader.color_bitm)->second));
        return std::hash<decltype(fmt_hash)>()(fmt_hash);
    }
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
        std::map<u64, ShaderItem const*>        shaders;

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
            shaders.clear();
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

        //        {
        //            GFX::RASTSTATE& rast = bsp[Pass_Opaque].source.raster;
        //            rast.m_doCull        = true;
        //            rast.m_culling       =
        //            (u32)typing::graphics::VertexFace::Front;
        //        }
        //        {
        //            GFX::RASTSTATE& rast = model[Pass_Opaque].source.raster;
        //            rast.m_doCull        = true;
        //            rast.m_culling       =
        //            (u32)typing::graphics::VertexFace::Front;
        //        }

        //        {
        //            GFX::BLNDSTATE& blend = bsp[Pass_Glass].source.blend;
        //            blend.m_doBlend       = true;
        //        }
        //        {
        //            GFX::BLNDSTATE& blend = model[Pass_Glass].source.blend;
        //            blend.m_doBlend       = true;
        //        }
        //        {
        //            GFX::BLNDSTATE& blend = bsp[Pass_Lights].source.blend;
        //            blend.m_doBlend       = true;
        //            blend.m_lighten       = true;
        //        }
        //        {
        //            GFX::BLNDSTATE& blend = model[Pass_Lights].source.blend;
        //            blend.m_doBlend       = true;
        //            blend.m_lighten       = true;
        //        }

        //        {
        //            GFX::BLNDSTATE& blend = bsp[Pass_Wireframe].source.blend;
        //            blend.m_doBlend       = true;
        //        }

        gfx::buffer_t& material_buf = *resources.material_store;
        gfx::buffer_t& matrix_buf   = *resources.model_matrix_store;

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
                = matrix_buf.slice(base_mat, model_batch_size / 4);

            base += model_batch_size;
            base_mat += model_batch_size / 4;
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
        return bitm_cache.template get_bucket<T>(get_bitm_format(bitm)).sampler;
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

    template<class T>
    BitmapItem const* assign_bitm(generation_idx_t bitm, T& map)
    {
        BitmapItem const* bitmap = get_bitm(bitm);
        detail::assign_map(map, bitmap);
        return bitmap;
    }
    u32 get_bitm_fmt_id(generation_idx_t bitm)
    {
        BitmapItem const* bitmap = get_bitm(bitm);
        if(!bitmap)
            return 0x0;
        switch(bitmap->image.fmt.pixfmt)
        {
        case PixFmt::BCn: {
            switch(bitmap->image.fmt.cmpflg)
            {
            case typing::pixels::CompFlags::BC1:
                return 0x01000000;
            case typing::pixels::CompFlags::BC2:
                return 0x02000000;
            case typing::pixels::CompFlags::BC3:
                return 0x03000000;
            default:
                break;
            }
            break;
        }
        case PixFmt::RGB565:
            return 0x04000000;
        case PixFmt::R8:
            return 0x05000000;
        case PixFmt::RG8:
            return 0x06000000;
        case PixFmt::RGBA4:
            return 0x07000000;
        case PixFmt::RGBA8:
            return 0x08000000;
        default:
            break;
        }
        return 0x0;
    }

    void setup_textures(
        std::vector<gfx::sampler_definition_t>& samplers,
        ShaderItem const*                       shader)
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

        //        switch(shader->tag->tagclass_e[0])
        //        {
        //        case blam::tag_class_t::senv: {
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"base"sv, 0},
        //                get_bitm_bucket<gfx::texture_2da_t>(shader->senv.base_bitm),
        //            });
        //            // For micro textures
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"micro"sv, 1},
        //                get_bitm_bucket<gfx::texture_2da_t>(shader->senv.micro_bitm),
        //            });
        //            // Additional diffuse textures
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"primary"sv, 2},
        //                get_bitm_bucket<gfx::texture_2da_t>(shader->senv.primary_bitm),
        //            });
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"secondary"sv, 3},
        //                get_bitm_bucket<gfx::texture_2da_t>(
        //                    shader->senv.secondary_bitm),
        //            });

        //            break;
        //        }
        //        case blam::tag_class_t::schi: {
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"base"sv, 0},
        //                get_bitm_bucket<gfx::texture_2da_t>(shader->schi.map1),
        //            });
        //            // samplers.push_back(gleam::sampler_definition_t{
        //            //                typing::graphics::ShaderStage::Fragment,
        //            //                {"micro"sv},
        //            // get_bitm_bucket<gfx::texture_2da_t>(shader->schi.map2),
        //            //            });
        //            break;
        //        }
        //        case blam::tag_class_t::scex: {
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"base"sv, 0},
        //                get_bitm_bucket<gfx::texture_2da_t>(shader->scex.layers.at(0)),
        //            });
        //            // samplers.push_back(gleam::sampler_definition_t{
        //            //                typing::graphics::ShaderStage::Fragment,
        //            //                {"micro"sv},
        //            //
        //            get_bitm_bucket<gfx::texture_2da_t>(shader->scex.layers.at(1)),
        //            //            });
        //            break;
        //        }
        //        case blam::tag_class_t::swat: {
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"base"sv, 0},
        //                get_bitm_bucket<gfx::texture_2da_t>(shader->color_bitm),
        //            });
        //            // samplers.push_back(gleam::sampler_definition_t{
        //            //                typing::graphics::ShaderStage::Fragment,
        //            //                {"micro"sv},
        //            //
        //            get_bitm_bucket<gfx::texture_2da_t>(shader->scex.layers.at(1)),
        //            //            });
        //            break;
        //        }
        //        case blam::tag_class_t::sgla: {
        //            samplers.push_back(gleam::sampler_definition_t{
        //                typing::graphics::ShaderStage::Fragment,
        //                {"base"sv, 0},
        //                get_bitm_bucket<gfx::texture_2da_t>(shader->color_bitm),
        //            });
        //            // samplers.push_back(gleam::sampler_definition_t{
        //            //                typing::graphics::ShaderStage::Fragment,
        //            //                {"micro"sv},
        //            //
        //            get_bitm_bucket<gfx::texture_2da_t>(shader->scex.layers.at(1)),
        //            //            });
        //            break;
        //        }
        //        default:
        //            cDebug("Unknown tag type: {0}",
        //            shader->tag->tagclass[0].str()); break;
        //        }
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

    void render_pass(Proxy&, Pass const& pass)
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
                get_view_state());
        }
    }

    template<typename... Args>
    void render_bsp_pass(Proxy&, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        std::vector<gfx::sampler_definition_t> samplers;

        /* Step 1: Set up shared uniform state + buffers */
        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1},
                semantic::SpanOne<const Matf4>(m_camera.camera_matrix)});
        //        auto fragment_u = gfx::make_uniform_list(
        //            typing::graphics::ShaderStage::Fragment,
        //            gfx::uniform_pair{
        //                {"render_distance", 2},
        //                semantic::SpanOne<const
        //                f32>(m_camera.wireframe_distance)});
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MatrixStore"sv, 0},
                pass.matrix_buffer},
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MaterialProperties"sv, 1},
                pass.material_buffer});

        for(auto const& [fmt, draw] : pass.draws)
        {
            /* Step 2: Set up the shader's required textures
             * These are sorted into buckets by generate_draws()
             */
            auto shader_it = pass.shaders.find(fmt);
            if(shader_it == pass.shaders.end())
            {
                cDebug("Failed to draw: shader not found");
                continue;
            }
            ShaderItem const* shader = shader_it->second;
            setup_textures(samplers, shader);
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

        for(auto const& pass : slice_num(m_bsp, Pass_LastOpaque))
        {
            render_bsp_pass(p, pass);
        }

        for(auto const& pass : slice_num(m_model, Pass_LastOpaque))
            render_pass(p, pass);

        render_pass(p, m_model[Pass_Glass]);
        render_bsp_pass(p, m_bsp[Pass_Glass], gfx::blend_state{});
        render_pass(p, m_model[Pass_Lights]);
        render_bsp_pass(p, m_bsp[Pass_Lights]);

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
        ShaderCache<Version>& shaders
            = p.template subsystem<ShaderCache<Version>>();

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

            auto& shader = *shaders.find(bsp.shader);
            auto  bucket = detail::shader_hash(shader.second, shader_cache);

            Pass& wf            = m_bsp[bsp.current_pass];
            wf.command.vertices = m_resources.bsp_attr;
            wf.command.call     = {
                    .indexed   = true,
                    .instanced = true,
                    .mode      = gfx::drawing::primitive::triangle,
            };
            wf.shaders[bucket] = &shader.second;
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

            auto& shader = *shaders.find(model.shader);

            Pass& wf            = m_model[model.current_pass];
            wf.command.vertices = m_resources.model_attr;
            wf.command.call     = {
                    .indexed   = true,
                    .instanced = true,
                    .mode      = gfx::drawing::primitive::line_strip,
            };
            wf.shaders[0]  = &shader.second;
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

        BlamFiles const& files = p.template subsystem<BlamFiles>();

        cDebug("Summary of generation:");
        for(Pass const& pass : m_bsp)
        {
            cDebug(
                " - Pass: {0} buckets, {1} shaders",
                pass.draws.size(),
                pass.shaders.size());
            for(auto [_, shader] : pass.shaders)
            {
                auto name = shader->tag->to_name().to_string(files.map_magic);
                cDebug("    - Shader: {0}", name);
            }
        }

        //        m_bsp[Pass_EnvMicro].draws().resize(10);
    }

    void populate_bsp_material(BspReference& ref, size_t i = 0)
    {
        Pass&             pass   = m_bsp[ref.current_pass];
        ShaderItem const* shader = get_shader(ref.shader);

        switch(shader->tag->tagclass_e.front())
        {
        case blam::tag_class_t::senv: {
            materials::senv_micro& mat
                = pass.template material_of<materials::senv_micro>(i);
            auto const* info = shader->header->as<blam::shader::shader_env>();

            auto base         = assign_bitm(shader->senv.base_bitm, mat.base);
            mat.base.uv_scale = {1};
            mat.base.bias     = base->image.bias;
            mat.base.layer |= get_bitm_fmt_id(shader->senv.base_bitm);

            auto primary = assign_bitm(shader->senv.primary_bitm, mat.primary);
            if(primary)
            {
                mat.primary.uv_scale = {info->diffuse.primary.scale};
                mat.primary.bias     = primary->image.bias;
                mat.primary.layer |= get_bitm_fmt_id(shader->senv.primary_bitm);
            }

            auto secondary
                = assign_bitm(shader->senv.secondary_bitm, mat.secondary);
            if(secondary)
            {
                mat.secondary.uv_scale = {info->diffuse.secondary.scale};
                mat.secondary.bias     = secondary->image.bias;
                mat.secondary.layer
                    |= get_bitm_fmt_id(shader->senv.secondary_bitm);
            }

            auto micro = assign_bitm(shader->senv.micro_bitm, mat.micro);
            mat.micro.uv_scale = {info->diffuse.micro.scale};
            if(micro)
                mat.micro.bias = micro->image.bias;
            mat.micro.layer |= get_bitm_fmt_id(shader->senv.micro_bitm);

            assign_bitm(ref.lightmap, mat.lightmap);

            mat.lightmap.material = 0x1;

            break;
        }
        case blam::tag_class_t::soso:
        case blam::tag_class_t::schi:
        case blam::tag_class_t::scex:
        case blam::tag_class_t::swat: {
            materials::senv_micro& mat
                = pass.template material_of<materials::senv_micro>(i);
            //            auto const* info  =
            //            shader->header->as<blam::shader::shader_glass>();
            assign_bitm(shader->color_bitm, mat.base);
            assign_bitm(shader->color_bitm, mat.micro);
            mat.base.uv_scale  = {1};
            mat.base.bias      = 0;
            mat.base.layer |= get_bitm_fmt_id(shader->color_bitm);
            mat.micro.uv_scale = {1};
            mat.micro.bias     = 0;

            mat.lightmap.material = 0x2;

            break;
        }
        case blam::tag_class_t::sgla: {
            materials::senv_micro& mat
                = pass.template material_of<materials::senv_micro>(i);
            assign_bitm(shader->color_bitm, mat.base);
            mat.base.uv_scale  = {1};
            mat.base.bias      = 0;
            mat.base.layer |= get_bitm_fmt_id(shader->color_bitm);

            mat.lightmap.material = 0x3;
            break;
        }
        default:
            cDebug(
                "Unhandled bsp shader type: {0}",
                shader->tag->tagclass[0].str());
            break;
        }
    }

    void populate_mod2_material(SubModel& sub, size_t /*i*/ = 0)
    {
        //        Pass&             pass   = m_model[sub.current_pass];
        ShaderItem const* shader = get_shader(sub.shader);

        switch(shader->tag->tagclass_e.front())
        {
        default:
            cDebug(
                "Unhandled mod2 shader type: {0}",
                shader->tag->tagclass[0].str());
            break;
        }
    }

    //    void setup_state(
    //        ShaderItem const& shader, PIP_PARAM& source, PIP_PARAM& params)
    //    {
    //        using blam::tag_class_t;

    //        BitmapCache& bitm = m_data.bitm_cache;

    //        params.set_constant("camera", Bytes::From(m_data.camera_matrix));

    //        auto dist_constant = std::find_if(
    //            params.constants_begin(),
    //            params.constants_end(),
    //            params.constant_by_name("render_distance"));

    //        if(dist_constant != params.constants_end())
    //            params.set_constant(
    //                *dist_constant, Bytes::From(m_data.wireframe_distance));

    //        switch(shader.tag->tagclass_e[0])
    //        {
    //        case tag_class_t::senv: {
    //            BitmapItem const& base =
    //            bitm.find(shader.senv.base_bitm)->second;

    //            params.set_sampler(
    //                "base",
    //                bitm.get_bucket(base.image.fmt).sampler->handle().bind(0));

    //            if(shader.senv.micro_bitm.valid())
    //            {
    //                BitmapItem const& micro =
    //                    bitm.find(shader.senv.micro_bitm)->second;

    //                params.set_sampler(
    //                    "micro",
    //                    bitm.get_bucket(micro.image.fmt).sampler->handle().bind(1));
    //                params.set_sampler(
    //                    "lightmaps",
    //                    bitm.get_bucket(PixDesc(PixFmt::RGB565))
    //                        .sampler->handle()
    //                        .bind(2));
    //            }

    //            if(shader.senv.primary_bitm.valid())
    //            {
    //                BitmapItem const& primary =
    //                    bitm.find(shader.senv.primary_bitm)->second;
    //                params.set_sampler(
    //                    "primary",
    //                    bitm.get_bucket(primary.image.fmt)
    //                        .sampler->handle()
    //                        .bind(3));
    //            }

    //            if(shader.senv.secondary_bitm.valid())
    //            {
    //                BitmapItem const& secondary =
    //                    bitm.find(shader.senv.secondary_bitm)->second;
    //                params.set_sampler(
    //                    "secondary",
    //                    bitm.get_bucket(secondary.image.fmt)
    //                        .sampler->handle()
    //                        .bind(4));
    //            }

    //            break;
    //        }
    //        default: {
    //            params.set_sampler(
    //                "bc1_tex",
    //                bitm.get_bucket(PixDesc(CompFmt(PixFmt::DXTn,
    //                CompFlags::DXT1)))
    //                    .sampler->handle()
    //                    .bind(0));
    //            params.set_sampler(
    //                "bc3_tex",
    //                bitm.get_bucket(PixDesc(CompFmt(PixFmt::DXTn,
    //                CompFlags::DXT3)))
    //                    .sampler->handle()
    //                    .bind(1));
    //            params.set_sampler(
    //                "bc5_tex",
    //                bitm.get_bucket(PixDesc(CompFmt(PixFmt::DXTn,
    //                CompFlags::DXT5)))
    //                    .sampler->handle()
    //                    .bind(2));
    //            break;
    //        }
    //        }

    //        params.build_state();
    //    }

    //    void update_draws(Proxy& p, time_point const&)
    //    {
    //        ProfContext _("Updating draw calls");

    //        for(Pass& pass : bsp)
    //            pass.clear();
    //        for(Pass& pass : model)
    //            pass.clear();

    //        BitmapCache& bitm_cache = m_data.bitm_cache;

    //        /* Update draws */
    //        for(auto& ent : p.select(ObjectBsp))
    //        {
    //            auto          ref     = p.template ref<Proxy>(ent);
    //            BspReference& bsp_ref = ref.template get<BspTag>();

    //            if(!bsp_ref.visible)
    //                continue;

    //            Pass& pass        = bsp[bsp_ref.current_pass];
    //            auto  target_pass = &pass.draws();

    //            ShaderItem const& shader =
    //                m_data.shader_cache.find(bsp_ref.shader)->second;

    //            auto state_hash = detail::shader_hash(shader,
    //            m_data.shader_cache); auto state      =
    //            pass.format_states.find(state_hash);

    //            if(state == pass.format_states.end())
    //            {
    //                state = pass.format_states.insert({state_hash,
    //                *pass.pipeline})
    //                            .first;

    //                PIP_PARAM& state_ = state->second;
    //                setup_state(shader, *pass.pipeline, state_);
    //            }

    //            bsp_ref.draw_idx = target_pass->size();
    //            target_pass->push_back(
    //                {m_data.bsp_attr,
    //                 &state->second.get_state(),
    //                 bsp_ref.draw.call,
    //                 bsp_ref.draw.draw});

    //            bsp[Pass_Wireframe].draws().push_back(
    //                {m_data.bsp_attr,
    //                 &bsp[Pass_Wireframe].pipeline->get_state(),
    //                 GFX::D_CALL().withIndexing().withInstancing().withLineStrip(),
    //                 bsp_ref.draw.draw});
    //        }

    //        for(auto& ent : p.select(ObjectMod2))
    //        {
    //            auto      ref     = p.template ref<Proxy>(ent);
    //            SubModel& mod_ref = ref.template get<SubModelTag>();

    //            if(!test_visible(mod_ref.parent.get<ModelTag>().transform))
    //                continue;

    //            auto target_pass = &model[mod_ref.current_pass].draws();

    //            mod_ref.draw_idx = target_pass->size();
    //            target_pass->push_back(
    //                {m_data.model_attr,
    //                 &m_data.model_pipeline->get_state(),
    //                 mod_ref.draw.call,
    //                 mod_ref.draw.draw});
    //        }

    //        for(auto i : Range<>(Pass_Count))
    //        {
    //            GFX::OptimizeRenderPass(bsp[i].source, bsp[i].draw);
    //            GFX::OptimizeRenderPass(model[i].source, model[i].draw);
    //        }

    //        /* Update transforms and texture references */

    //        auto material_store = m_data.material_store->map();
    //        auto matrix_store   = m_data.model_matrix_store->map();

    //        auto material_store_base = *material_store.at(0);
    //        material_store           = *material_store.at(0, 4_MB);

    //        for(Pass& pass : bsp)
    //            pass.material_buffer = *material_store.at(
    //                pass.material_buffer_range.first,
    //                pass.material_buffer_range.second);

    //        for(Pass& pass : model)
    //        {
    //            pass.material_buffer = *material_store_base.at(
    //                pass.material_buffer_range.first,
    //                pass.material_buffer_range.second);

    //            pass.matrix_buffer = (*matrix_store.at(
    //                                      pass.matrix_buffer_range.first,
    //                                      pass.matrix_buffer_range.second))
    //                                     .template as<Matf4>();
    //        }

    //        for(auto& ent : p.select(ObjectMod2))
    //        {
    //            auto      ref     = p.template ref<Proxy>(ent);
    //            SubModel& mod_ref = ref.template get<SubModelTag>();

    //            if(!test_visible(mod_ref.parent.get<ModelTag>().transform))
    //                continue;

    //            auto& draw_data =
    //                model[mod_ref.current_pass].draws().at(mod_ref.draw_idx).d_data;

    //            Pass& pass = model[mod_ref.current_pass];
    //            pass.matrix_buffer[draw_data.m_ioff] =
    //                mod_ref.parent.get<ModelTag>().transform;

    //            ModelItem& mod2 =
    //            m_data.model_cache.find(mod_ref.model)->second; auto mod_name
    //            =
    //                mod2.tag->to_name().to_string(m_data.map_container.magic);

    //            if(!mod_ref.shader.valid())
    //                continue;

    //            ShaderItem const* shader = get_shader(mod_ref.shader);

    //            if(!shader->color_bitm.valid())
    //                continue;

    //            BitmapItem& bitmap =
    //                m_data.bitm_cache.find(shader->color_bitm)->second;

    //            materials::basic& mat =
    //                pass.material_buffer
    //                    .template as<materials::basic>()[draw_data.m_ioff];

    //            mat.layer        = bitmap.image.layer;
    //            mat.atlas_offset = bitmap.image.offset;
    //            mat.atlas_scale  = bitmap.image.scale;
    //            mat.uv_scale     = mod2.uvscale;
    //            mat.bias         = bitmap.image.bias;

    //            auto comp_flags = std::get<4>(bitmap.image.bucket);
    //            switch(comp_flags)
    //            {
    //            case CompFlags::DXT1:
    //                mat.source = 0;
    //                break;
    //            case CompFlags::DXT3:
    //                mat.source = 1;
    //                break;
    //            case CompFlags::DXT5:
    //                mat.source = 2;
    //                break;
    //            default:
    //                break;
    //            }
    //        }

    //        for(auto& ent : p.select(ObjectBsp))
    //        {
    //            auto          ref     = p.template ref<Proxy>(ent);
    //            BspReference& bsp_ref = ref.template get<BspTag>();

    //            if(!bsp_ref.visible)
    //                continue;

    //            auto& draw_data =
    //                bsp[bsp_ref.current_pass].draws().at(bsp_ref.draw_idx).d_data;

    //            if(!bsp_ref.shader.valid())
    //                continue;

    //            ShaderItem const* shader = get_shader(bsp_ref.shader);

    //            if(!shader->color_bitm.valid())
    //                continue;

    //            Pass& pass = bsp[bsp_ref.current_pass];
    //            switch(bsp_ref.current_pass)
    //            {
    //            case Pass_EnvMicro: {
    //                auto shader_data =
    //                    C_RCAST<blam::shader_env const*>(shader->header);
    //                materials::senv_micro& mat =
    //                    pass.template
    //                    material_of<materials::senv_micro>(draw_data);

    //                BitmapItem const* base  =
    //                get_bitm(shader->senv.base_bitm); BitmapItem const* micro
    //                = get_bitm(shader->senv.micro_bitm); BitmapItem const*
    //                prim  = get_bitm(shader->senv.primary_bitm); BitmapItem
    //                const* seco  = get_bitm(shader->senv.secondary_bitm);
    //                BitmapItem const* lightmap = get_bitm(bsp_ref.lightmap);

    //                detail::assign_map(mat.base, base);
    //                mat.base.uv_scale = {1};
    //                mat.base.bias     = base->image.bias;

    //                detail::assign_map(mat.micro, micro);
    //                mat.micro.uv_scale = {shader_data->diffuse.micro.scale};
    //                if(micro)
    //                    mat.micro.bias = micro->image.bias;

    //                detail::assign_map(mat.primary, prim);
    //                mat.primary.uv_scale =
    //                {shader_data->diffuse.primary.scale}; if(prim)
    //                    mat.primary.bias = prim->image.bias;

    //                detail::assign_map(mat.secondary, seco);
    //                mat.secondary.uv_scale =
    //                {shader_data->diffuse.secondary.scale}; if(seco)
    //                    mat.secondary.bias = seco->image.bias;

    //                detail::assign_map(mat.lightmap, lightmap);

    //                break;
    //            }
    //            default: {
    //                materials::basic& mat =
    //                    pass.material_buffer
    //                        .template
    //                        as<materials::basic>()[draw_data.m_ioff];

    //                BitmapItem& bitmap =
    //                    m_data.bitm_cache.find(shader->color_bitm)->second;
    //                mat.layer        = bitmap.image.layer;
    //                mat.atlas_offset = bitmap.image.offset;
    //                mat.atlas_scale  = bitmap.image.scale;
    //                mat.uv_scale     = {1};

    //                auto comp_flags = std::get<4>(bitmap.image.bucket);
    //                switch(comp_flags)
    //                {
    //                case CompFlags::DXT1:
    //                    mat.source = 0;
    //                    break;
    //                case CompFlags::DXT3:
    //                    mat.source = 1;
    //                    break;
    //                case CompFlags::DXT5:
    //                    mat.source = 2;
    //                    break;
    //                default:
    //                    break;
    //                }
    //                break;
    //            }
    //            }
    //        }

    //        m_data.material_store->unmap();
    //        m_data.model_matrix_store->unmap();
    //    }
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
        auto& api       = e.subsystem<gfx::system>();
        auto& resources = e.subsystem<BlamResources>();

        if(api.default_rendertarget() == resources.offscreen)
            return;
        if(!quad_program)
        {
            load_resources(api, e.subsystem<BlamResources>());
        }

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
                {"source"sv, 0},
                offscreen_sampler
            }));
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
void main()
{
    gl_FragColor = vec4(pow(texture2D(source, in_tex).rgb, vec3(1.0 / 1.5)), 1.0);
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
