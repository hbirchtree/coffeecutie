#pragma once

#include <peripherals/stl/tuple_hash.h>

#include "data.h"

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

    switch(shader.tag->tagclass_e[0])
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

template<typename V>
using MeshRendererManifest = Components::SubsystemManifest<
    type_list_t<BspReference, SubModel, Model>,
    type_list_t<BitmapCache<V>, ModelCache<V>, BSPCache<V>, ShaderCache<V>>,
    empty_list_t>;

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
        gfx::draw_command command;

        gfx::buffer_slice_t material_buffer;
        gfx::buffer_slice_t matrix_buffer;

        ShaderItem const* shader;

        decltype(command.data)& draws()
        {
            return command.data;
        }

        inline void clear()
        {
            command.data.clear();
        }

        template<typename T>
        inline T& material_of(draw_data_t const& draw)
        {
            return material_buffer.buffer_cast<T>().subspan(
                draw.instances.offset, draw.instances.count);
        }
    };

    time_point last_update;

    gfx::api*          m_api;
    BlamData<Version>& m_data;

    //    LinkList<PIP_PARAM>     pipelines;
    Array<Pass, Pass_Count> m_bsp;
    Array<Pass, Pass_Count> m_model;

    MeshRenderer(gfx::api* api, BlamData<Version>& data) :
        m_api(api), m_data(data)
    {
        for(auto& pass : m_bsp)
            pass.command.program = data.bsp_pipeline;
        m_bsp[Pass_EnvMicro].command.program  = data.senv_micro_pipeline;
        m_bsp[Pass_Wireframe].command.program = data.wireframe_pipeline;
        for(auto& pass : m_model)
            pass.command.program = data.model_pipeline;

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

        gfx::buffer_t& material_buf = *m_data.material_store;
        gfx::buffer_t& matrix_buf   = *m_data.model_matrix_store;

        u32 base = 0;
        for(Pass& pass : m_bsp)
        {
            pass.material_buffer = material_buf.slice(base, 512_kB);
            base += 512_kB;
        }
        base         = 4_MB;
        u32 base_mat = 0;
        for(Pass& pass : m_model)
        {
            pass.material_buffer = material_buf.slice(base, 1024_kB);
            pass.matrix_buffer   = matrix_buf.slice(base_mat, 1024_kB);

            base += 1024_kB;
            base_mat += 1024_kB;
        }
    }

    ShaderItem const* get_shader(generation_idx_t shader)
    {
        if(!shader.valid())
            return nullptr;

        auto it = m_data.shader_cache.find(shader);

        if(it == m_data.shader_cache.m_cache.end())
            return nullptr;

        return &it->second;
    }
    BitmapItem const* get_bitm(generation_idx_t bitm)
    {
        if(!bitm.valid())
            return nullptr;

        auto it = m_data.bitm_cache.find(bitm);

        if(it == m_data.bitm_cache.m_cache.end())
            return nullptr;

        return &it->second;
    }
    BSPItem const* get_bsp(generation_idx_t bsp)
    {
        if(!bsp.valid())
            return nullptr;

        auto it = m_data.bsp_cache.find(bsp);

        if(it == m_data.bsp_cache.m_cache.end())
            return nullptr;

        return &it->second;
    }

    void render_pass(Proxy& p, Pass const& pass)
    {
        using namespace std::string_view_literals;

        std::vector<gfx::sampler_definition_t> samplers;

        BitmapCache<Version>& bcache
            = p.template subsystem<BitmapCache<Version>>();

        switch(blam::tag_class_t::senv)
        {
        case blam::tag_class_t::senv: {
            samplers = gfx::make_sampler_list(gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"base"sv},
                bcache
                    .template get_bucket<gfx::texture_2da_t>(
                        PixDesc(PixFmt::RGB565))
                    .sampler});

            // For micro textures
            samplers.push_back(gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"lightmaps"sv},
                bcache
                    .template get_bucket<gfx::texture_2da_t>(
                        PixDesc(PixFmt::RGB565))
                    .sampler});
            samplers.push_back(gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"micro"sv},
                bcache
                    .template get_bucket<gfx::texture_2da_t>(
                        PixDesc(PixFmt::RGB565))
                    .sampler});

            samplers.push_back(gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"primary"sv},
                bcache
                    .template get_bucket<gfx::texture_2da_t>(
                        PixDesc(PixFmt::RGB565))
                    .sampler});
            samplers.push_back(gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"secondary"sv},
                bcache
                    .template get_bucket<gfx::texture_2da_t>(
                        PixDesc(PixFmt::RGB565))
                    .sampler});

            break;
        }
        default:
            samplers = gfx::make_sampler_list(
                gleam::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"bc1_tex"sv},
                    bcache
                        .template get_bucket<gfx::texture_2da_t>(
                            CompFmt(PixFmt::BCn, CompFlags::BC1))
                        .sampler},
                gleam::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"bc3_tex"sv},
                    bcache
                        .template get_bucket<gfx::texture_2da_t>(
                            CompFmt(PixFmt::BCn, CompFlags::BC3))
                        .sampler},
                gleam::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"bc5_tex"sv},
                    bcache
                        .template get_bucket<gfx::texture_2da_t>(
                            CompFmt(PixFmt::BCn, CompFlags::BC5))
                        .sampler});
            break;
        }

        auto res = m_api->submit(
            pass.command,
            gfx::make_buffer_list(
                gfx::buffer_definition_t{
                    typing::graphics::ShaderStage::Vertex,
                    {"matrix_buffer"sv, 0},
                    pass.matrix_buffer},
                gfx::buffer_definition_t{
                    typing::graphics::ShaderStage::Vertex,
                    {"material_buffer"sv, 1},
                    pass.material_buffer}),
            gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv},
                    semantic::SpanOne<const Matf4>(m_data.camera_matrix)}),
            gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{
                    {"render_distance"},
                    semantic::SpanOne<const f32>(m_data.wireframe_distance)}),
            samplers);

        if(res.has_value())
        {
            auto [error, msg] = res.value();
            cFatal(
                "Draw error: {0}: {1}: {2}",
                pass.shader->tag->to_name().to_string(
                    m_data.map_container.magic),
                magic_enum::enum_name(error),
                msg);
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

    bool test_visible(Matf4 const& mat)
    {
        Vecf4 probe = {0, 0, 0, 1};
        Vecf4 out   = probe * (m_data.camera_matrix * mat);

        Vecf3 norm = {out.x() / out.w(), out.y() / out.w(), out.z() / out.w()};

        scalar margin = 1.f;

        return norm.x() < margin && norm.y() < margin && norm.z() < margin
               && norm.x() > -margin && norm.y() > -margin
               && norm.z() > -margin;
    }

    void start_restricted(Proxy& p, time_point const& time)
    {
    }

    void end_restricted(Proxy& p, time_point const& time)
    {
        if(time - last_update > Chrono::seconds(5))
        {
            generate_draws(p);
            last_update = time;
        }

        u32 i = 0;
        for(auto const& pass : slice_num(m_bsp, Pass_LastOpaque))
        {
            render_pass(p, pass);
            i++;
        }

        for(auto const& pass : slice_num(m_model, Pass_LastOpaque))
            render_pass(p, pass);

        render_pass(p, m_model[Pass_Glass]);
        render_pass(p, m_bsp[Pass_Glass]);
        render_pass(p, m_model[Pass_Lights]);
        render_pass(p, m_bsp[Pass_Lights]);

        render_pass(p, m_bsp[Pass_Wireframe]);
    }

    void generate_draws(Proxy& p)
    {
        BitmapCache<Version>& bitm
            = p.template subsystem<BitmapCache<Version>>();
        ShaderCache<Version>& shaders
            = p.template subsystem<ShaderCache<Version>>();

        for(auto& ent : p.select(ObjectBsp))
        {
            auto  ref = p.template ref<Proxy>(ent);
            auto& bsp = ref.template get<BspReference>();

//            if(!bsp.visible)
//                continue;

            auto& shader = *shaders.find(bsp.shader);

            Pass& wf            = m_bsp[Pass_Wireframe];
            wf.command.vertices = m_data.bsp_attr;
            wf.command.call = {
                .indexed = true,
                .instanced = true,
                .mode = gfx::drawing::primitive::triangle,
            };
            wf.draws().push_back(bsp.draw.data.front());
            draw_data_t& draw = wf.draws().back();
        }

        m_bsp[Pass_Wireframe].draws().resize(40);
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
