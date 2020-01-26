#pragma once

#include <peripherals/stl/tuple_hash.h>

#include "data.h"

namespace detail {

inline Tup<PixFmt, CompFlags> get_bitm_hash(BitmapItem const& bitm)
{
    return std::make_tuple(bitm.image.fmt.pixfmt, bitm.image.fmt.cmpflg);
}

inline u64 shader_hash(ShaderItem const& shader, ShaderCache& cache)
{
    using blam::tag_class_t;

    auto& bitm = cache.bitm_cache;

    switch(shader.tag->tagclass_e[0])
    {
    case tag_class_t::senv:
    {
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
            BitmapItem const& map =
                bitm.find(shader.senv.secondary_bitm)->second;
            maps[2] = get_bitm_hash(map);
        }

        auto fmt_hash =
            std::make_tuple(get_bitm_hash(base), maps[0], maps[1], maps[2]);
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
                          Components::TagType<MeshRenderer<Version>>,
                          type_list_t<
                              BspTag,
                              SubModelTag,
                              ModelTag,
                              ObjectSpawnTag,
                              MultiplayerSpawnTag>,
                          empty_list_t>
{
    using parent_type = Components::RestrictedSubsystem<
        Components::TagType<MeshRenderer<Version>>,
        type_list_t<
            BspTag,
            SubModelTag,
            ModelTag,
            ObjectSpawnTag,
            MultiplayerSpawnTag>,
        empty_list_t>;

    using Proxy = typename parent_type::Proxy;

    struct Pass
    {
        Pass()
        {
        }

        GFX::RenderPass     source;
        GFX::OPT_DRAW       draw;
        PIP_PARAM*          pipeline;
        Map<u64, PIP_PARAM> format_states;

        Pair<u32, u32> material_buffer_range;
        Bytes          material_buffer;

        decltype(source.draws)& draws()
        {
            return source.draws;
        }

        inline void clear()
        {
            source.draws.clear();
            draw = {};
        }

        template<typename T>
        inline T& material_of(GFX::D_DATA const& draw)
        {
            return material_buffer.template as<T>()[draw.instanceOffset()];
        }
    };

    MeshRenderer(BlamData<Version>& data) : m_data(data)
    {
        for(auto& pass : bsp)
        {
            pass.pipeline        = data.bsp_pipeline;
            pass.source.pipeline = data.bsp_pipeline->pipeline_ptr();
        }
        bsp[Pass_EnvMicro].pipeline = data.senv_micro_pipeline;
        bsp[Pass_EnvMicro].source.pipeline =
            data.senv_micro_pipeline->pipeline_ptr();

        bsp[Pass_Wireframe].pipeline = data.wireframe_pipeline;
        bsp[Pass_Wireframe].source.pipeline =
            data.wireframe_pipeline->pipeline_ptr();

        model[Pass_Opaque].source.pipeline =
            data.model_pipeline->pipeline_ptr();
        model[Pass_Metal].source.pipeline = data.model_pipeline->pipeline_ptr();
        model[Pass_Glass].source.pipeline = data.model_pipeline->pipeline_ptr();
        model[Pass_Lights].source.pipeline =
            data.model_pipeline->pipeline_ptr();

        for(auto& pass : model)
            pass.pipeline = data.model_pipeline;

        {
            GFX::RASTSTATE& rast = bsp[Pass_Opaque].source.raster;
            rast.m_doCull        = true;
            rast.m_culling       = (u32)typing::graphics::VertexFace::Front;
        }
        {
            GFX::RASTSTATE& rast = model[Pass_Opaque].source.raster;
            rast.m_doCull        = true;
            rast.m_culling       = (u32)typing::graphics::VertexFace::Front;
        }

        {
            GFX::BLNDSTATE& blend = bsp[Pass_Glass].source.blend;
            blend.m_doBlend       = true;
        }
        {
            GFX::BLNDSTATE& blend = model[Pass_Glass].source.blend;
            blend.m_doBlend       = true;
        }
        {
            GFX::BLNDSTATE& blend = bsp[Pass_Lights].source.blend;
            blend.m_doBlend       = true;
            blend.m_lighten       = true;
        }
        {
            GFX::BLNDSTATE& blend = model[Pass_Lights].source.blend;
            blend.m_doBlend       = true;
            blend.m_lighten       = true;
        }

        {
            GFX::BLNDSTATE& blend = bsp[Pass_Wireframe].source.blend;
            blend.m_doBlend       = true;
        }

        u32 base = 0;
        for(Pass& pass : bsp)
        {
            pass.material_buffer_range = {base, 128_kB};
            base += pass.material_buffer_range.second;
        }
        base = 0;
        for(Pass& pass : model)
        {
            pass.material_buffer_range = {base, 128_kB};
            base += pass.material_buffer_range.second;
        }
    }

    MeshRenderer<Version> const& get() const
    {
        return *this;
    }
    MeshRenderer<Version>& get()
    {
        return *this;
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

    void render_pass(Pass const& pass)
    {
        if(!pass.source.pipeline.lock())
            return;

        GFX::ERROR ec;

        m_data.material_store->bindrange(
            0,
            pass.material_buffer_range.first,
            pass.material_buffer_range.second,
            ec);

        GFX::SetRasterizerState(pass.source.raster);
        GFX::SetBlendState(pass.source.blend);
        GFX::MultiDraw(*pass.source.pipeline.lock(), pass.draw);
    }

    bool test_visible(Matf4 const& mat)
    {
        return true;

        Vecf4 probe = {0, 0, 0, 1};
        Vecf4 out   = probe * (m_data.camera_matrix * mat);

        Vecf3 norm = {out.x() / out.w(), out.y() / out.w(), out.z() / out.w()};

        scalar margin = 1.f;

        return norm.x() < margin && norm.y() < margin && norm.z() < margin &&
               norm.x() > -margin && norm.y() > -margin && norm.z() > -margin;
    }

    virtual void end_restricted(Proxy& p, time_point const& time) override
    {
        if(time - last_update > Chrono::seconds(5))
        {
            update_draws(p, time);
            last_update = time;
        }

        for(auto const& pass : slice_num(bsp, Pass_LastOpaque))
            render_pass(pass);

        for(auto const& pass : slice_num(model, Pass_LastOpaque))
            render_pass(pass);

        render_pass(model[Pass_Glass]);
        render_pass(bsp[Pass_Glass]);
        render_pass(model[Pass_Lights]);
        render_pass(bsp[Pass_Lights]);

        //        render_pass(bsp[Pass_Wireframe]);
    }

    void setup_state(
        ShaderItem const& shader, PIP_PARAM& source, PIP_PARAM& params)
    {
        using blam::tag_class_t;

        BitmapCache& bitm = m_data.bitm_cache;

        params.set_constant("camera", Bytes::From(m_data.camera_matrix));

        auto dist_constant = std::find_if(
            params.constants_begin(),
            params.constants_end(),
            params.constant_by_name("render_distance"));

        if(dist_constant != params.constants_end())
            params.set_constant(
                *dist_constant, Bytes::From(m_data.wireframe_distance));

        switch(shader.tag->tagclass_e[0])
        {
        case tag_class_t::senv:
        {
            BitmapItem const& base = bitm.find(shader.senv.base_bitm)->second;

            params.set_sampler(
                "base",
                bitm.get_bucket(base.image.fmt).sampler->handle().bind(0));

            if(shader.senv.micro_bitm.valid())
            {
                BitmapItem const& micro =
                    bitm.find(shader.senv.micro_bitm)->second;

                params.set_sampler(
                    "micro",
                    bitm.get_bucket(micro.image.fmt).sampler->handle().bind(1));
                params.set_sampler(
                    "lightmaps",
                    bitm.get_bucket(PixDesc(PixFmt::RGB565))
                        .sampler->handle()
                        .bind(2));
            }

            if(shader.senv.primary_bitm.valid())
            {
                BitmapItem const& primary =
                    bitm.find(shader.senv.primary_bitm)->second;
                params.set_sampler(
                    "primary",
                    bitm.get_bucket(primary.image.fmt)
                        .sampler->handle()
                        .bind(3));
            }

            if(shader.senv.secondary_bitm.valid())
            {
                BitmapItem const& secondary =
                    bitm.find(shader.senv.secondary_bitm)->second;
                params.set_sampler(
                    "secondary",
                    bitm.get_bucket(secondary.image.fmt)
                        .sampler->handle()
                        .bind(4));
            }

            break;
        }
        default:
        {
            params.set_sampler(
                "bc1_tex",
                bitm.get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT1)))
                    .sampler->handle()
                    .bind(0));
            params.set_sampler(
                "bc3_tex",
                bitm.get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT3)))
                    .sampler->handle()
                    .bind(1));
            params.set_sampler(
                "bc5_tex",
                bitm.get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT5)))
                    .sampler->handle()
                    .bind(2));
            break;
        }
        }

        params.build_state();
    }

    void update_draws(Proxy& p, time_point const&)
    {
        for(Pass& pass : bsp)
            pass.clear();
        for(Pass& pass : model)
            pass.clear();

        BitmapCache& bitm_cache = m_data.bitm_cache;

        /* Update draws */
        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref     = p.template ref<Proxy>(ent);
            BspReference& bsp_ref = ref.template get<BspTag>();

            if(!bsp_ref.visible)
                continue;

            Pass& pass        = bsp[bsp_ref.current_pass];
            auto  target_pass = &pass.draws();

            ShaderItem const& shader =
                m_data.shader_cache.find(bsp_ref.shader)->second;

            auto state_hash = detail::shader_hash(shader, m_data.shader_cache);
            auto state      = pass.format_states.find(state_hash);

            if(state == pass.format_states.end())
            {
                state = pass.format_states.insert({state_hash, *pass.pipeline})
                            .first;

                PIP_PARAM& state_ = state->second;
                setup_state(shader, *pass.pipeline, state_);
            }

            bsp_ref.draw_idx = target_pass->size();
            target_pass->push_back({m_data.bsp_attr,
                                    &state->second.get_state(),
                                    bsp_ref.draw.call,
                                    bsp_ref.draw.draw});

            bsp[Pass_Wireframe].draws().push_back(
                {m_data.bsp_attr,
                 &bsp[Pass_Wireframe].pipeline->get_state(),
                 GFX::D_CALL().withIndexing().withInstancing().withLineStrip(),
                 bsp_ref.draw.draw});
        }

        for(auto& ent : p.select(ObjectMod2))
        {
            auto      ref     = p.template ref<Proxy>(ent);
            SubModel& mod_ref = ref.template get<SubModelTag>();

            if(!test_visible(mod_ref.parent.get<ModelTag>().transform))
                continue;

            auto target_pass = &model[mod_ref.current_pass].draws();

            mod_ref.draw_idx = target_pass->size();
            target_pass->push_back({m_data.model_attr,
                                    &m_data.model_pipeline->get_state(),
                                    mod_ref.draw.call,
                                    mod_ref.draw.draw});
        }

        for(auto i : Range<>(Pass_LastOpaque))
        {
            GFX::OptimizeRenderPass(bsp[i].source, bsp[i].draw);
            GFX::OptimizeRenderPass(model[i].source, model[i].draw);
        }

        GFX::OptimizeRenderPass(
            bsp[Pass_Wireframe].source, bsp[Pass_Wireframe].draw);

        GFX::OptimizeRenderPass(
            model[Pass_Glass].source,
            model[Pass_Glass].draw,
            model[Pass_Opaque].draws().size());
        GFX::OptimizeRenderPass(bsp[Pass_Glass].source, bsp[Pass_Glass].draw);

        GFX::OptimizeRenderPass(
            model[Pass_Lights].source,
            model[Pass_Lights].draw,
            model[Pass_Opaque].draws().size() +
                model[Pass_Glass].draws().size());
        GFX::OptimizeRenderPass(bsp[Pass_Lights].source, bsp[Pass_Lights].draw);

        /* Update transforms and texture references */

        auto material_store = m_data.material_store->map();

        auto model_material_store = material_store.at(2_MB);
        material_store            = material_store.at(0, 2_MB);

        for(Pass& pass : bsp)
            pass.material_buffer = material_store.at(
                pass.material_buffer_range.first,
                pass.material_buffer_range.second);

        for(Pass& pass : model)
            pass.material_buffer = model_material_store.at(
                pass.material_buffer_range.first,
                pass.material_buffer_range.second);

        auto matrix_store =
            m_data.model_matrix_store->map().template as<Matf4>();

        for(auto& ent : p.select(ObjectMod2))
        {
            auto      ref     = p.template ref<Proxy>(ent);
            SubModel& mod_ref = ref.template get<SubModelTag>();

            if(!test_visible(mod_ref.parent.get<ModelTag>().transform))
                continue;

            auto& draw_data =
                model[mod_ref.current_pass].draws().at(mod_ref.draw_idx).d_data;

            matrix_store[draw_data.m_ioff] =
                mod_ref.parent.get<ModelTag>().transform;

            ModelItem& mod2 = m_data.model_cache.find(mod_ref.model)->second;
            auto       mod_name =
                mod2.tag->to_name().to_string(m_data.map_container.magic);

            if(!mod_ref.shader.valid())
                continue;

            ShaderItem const* shader = get_shader(mod_ref.shader);

            if(!shader->color_bitm.valid())
                continue;

            BitmapItem& bitmap =
                m_data.bitm_cache.find(shader->color_bitm)->second;

            materials::basic& mat =
                model_material_store
                    .template as<materials::basic>()[draw_data.m_ioff];

            mat.layer        = bitmap.image.layer;
            mat.atlas_offset = bitmap.image.offset;
            mat.atlas_scale  = bitmap.image.scale;
            mat.uv_scale     = mod2.uvscale;

            auto comp_flags = std::get<3>(bitmap.image.bucket);
            switch(comp_flags)
            {
            case CompFlags::DXT1:
                mat.source = 0;
                break;
            case CompFlags::DXT3:
                mat.source = 1;
                break;
            case CompFlags::DXT5:
                mat.source = 2;
                break;
            }
        }

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref     = p.template ref<Proxy>(ent);
            BspReference& bsp_ref = ref.template get<BspTag>();

            auto& draw_data =
                bsp[bsp_ref.current_pass].draws().at(bsp_ref.draw_idx).d_data;

            if(!bsp_ref.shader.valid())
                continue;

            ShaderItem const* shader = get_shader(bsp_ref.shader);

            if(!shader->color_bitm.valid())
                continue;

            Pass& pass = bsp[bsp_ref.current_pass];
            switch(bsp_ref.current_pass)
            {
            case Pass_EnvMicro:
            {
                auto shader_data =
                    C_RCAST<blam::shader_env const*>(shader->header);
                materials::senv_micro& mat =
                    pass.template material_of<materials::senv_micro>(draw_data);

                BitmapItem const* base  = get_bitm(shader->senv.base_bitm);
                BitmapItem const* micro = get_bitm(shader->senv.micro_bitm);
                BitmapItem const* prim  = get_bitm(shader->senv.primary_bitm);
                BitmapItem const* seco  = get_bitm(shader->senv.secondary_bitm);
                BitmapItem const* lightmap = get_bitm(bsp_ref.lightmap);

                detail::assign_map(mat.base, base);
                mat.base.uv_scale = {1};

                detail::assign_map(mat.micro, micro);
                mat.micro.uv_scale = {shader_data->diffuse.micro.scale};

                detail::assign_map(mat.primary, prim);
                mat.primary.uv_scale = {shader_data->diffuse.primary.scale};

                detail::assign_map(mat.secondary, seco);
                mat.secondary.uv_scale = {shader_data->diffuse.secondary.scale};

                detail::assign_map(mat.lightmap, lightmap);

                break;
            }
            default:
            {
                materials::basic& mat =
                    pass.material_buffer
                        .template as<materials::basic>()[draw_data.m_ioff];

                BitmapItem& bitmap =
                    m_data.bitm_cache.find(shader->color_bitm)->second;
                mat.layer        = bitmap.image.layer;
                mat.atlas_offset = bitmap.image.offset;
                mat.atlas_scale  = bitmap.image.scale;
                mat.uv_scale     = {1};

                auto comp_flags = std::get<3>(bitmap.image.bucket);
                switch(comp_flags)
                {
                case CompFlags::DXT1:
                    mat.source = 0;
                    break;
                case CompFlags::DXT3:
                    mat.source = 1;
                    break;
                case CompFlags::DXT5:
                    mat.source = 2;
                    break;
                }
                break;
            }
            }
        }

        m_data.material_store->unmap();
        m_data.model_matrix_store->unmap();
    }

    time_point last_update;

    BlamData<Version>& m_data;

    LinkList<PIP_PARAM>     pipelines;
    Array<Pass, Pass_Count> bsp;
    Array<Pass, Pass_Count> model;
};
