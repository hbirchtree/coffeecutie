#pragma once

#include "bitmap_cache.h"
#include "caching_item.h"
#include "data.h"
#include "data_cache.h"
#include "materials.h"

using ShaderManifest
    = compo::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct ShaderCache : DataCache<ShaderItem, u32, blam::tagref_t const&>,
                     compo::RestrictedSubsystem<ShaderCache<V>, ShaderManifest>
{
    using type  = ShaderCache<V>;
    using Proxy = compo::proxy_of<ShaderManifest>;

    ShaderCache(BitmapCache<V>& bitmap_cache) : bitm_cache(bitmap_cache)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index = blam::tag_index_view(map);
        magic = map.magic;
        evict_all();
    }

    BitmapCache<V>&         bitm_cache;
    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;

    template<blam::tag_class_t Tag>
    generation_idx_t get_bitm_idx(blam::tagref_typed_t<Tag> const& bitm)
    {
        if(!bitm.valid())
            return generation_idx_t();

        return bitm_cache.predict(bitm.to_plain(), 0);
    }

    template<typename T>
    T const* extract_shader(typename blam::tag_index_view<V>::iterator it)
    {
        return (*it).template data<T>(magic).value();
    }

    ShaderItem predict_impl(blam::tagref_t const& shader)
    {
        auto _ = bitm_cache.allocator->debug().scope("ShaderCache");

        using blam::tag_class_t;
        using namespace blam::shader;

        if(!shader.valid())
            return {};

        auto it                    = index.find(shader);
        C_UNUSED(auto shader_name) = shader.to_name().to_string(magic);

        if(it == index.end())
            return {};

        ShaderItem out = {};
        out.tag        = &(*it);
        out.tag_class  = shader.tag_class;
        out.header     = extract_shader<blam::shader::radiosity_properties>(it);

        switch(shader.tag_class)
        {
        case tag_class_t::senv: {
            auto const& shader_model = *extract_shader<shader_env>(it);

            out.senv.base_bitm = get_bitm_idx(shader_model.diffuse.base);
            out.senv.primary_bitm
                = get_bitm_idx(shader_model.diffuse.primary.map);
            out.senv.secondary_bitm
                = get_bitm_idx(shader_model.diffuse.secondary.map);
            out.senv.micro_bitm = get_bitm_idx(shader_model.diffuse.micro.map);

            //            out.senv.self_illum =
            //            get_bitm_idx(shader_model.self_illum.map.map);
            out.senv.bump = get_bitm_idx(shader_model.bump.map);
            out.senv.reflection_bitm
                = get_bitm_idx(shader_model.reflection.reflection);

            break;
        }
        case tag_class_t::soso: {
            blam::shader::shader_model const& shader_model
                = *extract_shader<blam::shader::shader_model>(it);

            out.soso.base_bitm   = get_bitm_idx(shader_model.maps.base);
            out.soso.multi_bitm  = get_bitm_idx(shader_model.maps.multipurpose);
            out.soso.detail_bitm = get_bitm_idx(shader_model.maps.detail.map);
            out.soso.reflection_bitm
                = get_bitm_idx(shader_model.reflection.reflection);

            break;
        }
        case tag_class_t::schi: {
            auto const& shader_model = *extract_shader<shader_chicago<V>>(it);

            if(auto maps = shader_model.maps.data(magic); maps.has_value())
            {
                u8 i = 0;
                for(chicago::map_t const& map : maps.value())
                {
                    out.schi.maps.at(i++) = get_bitm_idx(map.map.map);
                }
            }
            if(auto layers = shader_model.layers.data(magic);
               layers.has_value())
            {
                u8 i = 0;
                for(auto const& layer : layers.value())
                    out.schi.layers.at(i++) = predict(layer.to_plain());
            }

            break;
        }
        case tag_class_t::scex: {
            auto const& shader_model
                = *extract_shader<shader_chicago_extended<V>>(it);

            if(auto maps4 = shader_model.maps_4stage.data(magic);
               maps4.has_value())
            {
                u8 i = 0;
                for(chicago::map_t const& map : maps4.value())
                    out.scex.maps.at(i++) = get_bitm_idx(map.map.map);
            }
            if(auto layers = shader_model.layers.data(magic);
               layers.has_value())
            {
                u8 i = 0;
                for(auto const& layer : layers.value())
                    out.scex.layers.at(i++) = predict(layer.to_plain());
            }

            break;
        }
        case tag_class_t::sgla: {
            shader_glass const& shader_model
                = *extract_shader<shader_glass>(it);

            out.color_bitm = get_bitm_idx(shader_model.diffuse.map.map);

            break;
        }
        case tag_class_t::swat: {
            shader_water const& shader_model
                = *extract_shader<shader_water>(it);

            out.swat.base       = get_bitm_idx(shader_model.base);
            out.swat.reflection = get_bitm_idx(shader_model.reflection_map);
            out.swat.bump       = get_bitm_idx(shader_model.ripple.map);

            break;
        }
        case tag_class_t::spla: {
            auto const& shader_model = *extract_shader<shader_plasma>(it);

            out.color_bitm = get_bitm_idx(shader_model.primary_noise.noise.map);

            break;
        }
        case tag_class_t::smet: {
            auto const& shader_model = *extract_shader<shader_meter>(it);

            out.color_bitm = get_bitm_idx(shader_model.map);

            break;
        }
        case tag_class_t::sotr: {
            shader_transparent const& shader_model
                = *extract_shader<shader_transparent>(it);

            if(auto maps = shader_model.maps.data(magic); maps.has_value())
            {
                u32 i = 0;
                for(shader_transparent::map_t const& map : maps.value())
                    out.sotr.maps.at(i++) = get_bitm_idx(map.map.map);
            }

            break;
        }
        default:
            Throw(undefined_behavior("unhandled shader type"));
        }

        return out;
    }

    void populate_material(
        materials::shader_data& mat,
        generation_idx_t const& shader_id,
        Vecf2 const&            base_map_scale)
    {
        using blam::tag_class_t;
        using namespace blam::shader;

        ShaderItem const& shader = find(shader_id)->second;

        mat = {};

        switch(shader.tag_class)
        {
        case tag_class_t::scex: {
            shader_chicago_extended<V> const* info
                = shader.header->as<blam::shader::shader_chicago_extended<V>>();

            auto maps = info->maps_4stage.data(magic).value();
            for(auto i : range<>(4))
            {
                auto id = shader.scex.maps.at(i);
                if(!shader.schi.maps.at(i).valid())
                    continue;
                BitmapItem const& bitm
                    = *bitm_cache.assign_atlas_data(mat.maps[i], id);
                chicago::map_t const& map = maps[i];
                mat.maps[i].uv_scale      = map.map.uv_scale * base_map_scale;
                mat.maps[i].bias          = bitm.image.bias;

                u16 flags = static_cast<u8>(map.color_func)
                            | (static_cast<u8>(map.alpha_func) << 4);
                mat.lightmap.meta1 |= flags << (i * 8);
            }

            mat.material.material = materials::id::scex;
            break;
        }
        case tag_class_t::schi: {
            shader_chicago<V> const* info
                = shader.header->as<blam::shader::shader_chicago<V>>();

            auto maps = info->maps.data(magic).value();
            for(auto i : range<>(4))
            {
                auto id = shader.schi.maps.at(i);
                if(!shader.schi.maps.at(i).valid())
                    continue;
                BitmapItem const& bitm
                    = *bitm_cache.assign_atlas_data(mat.maps[i], id);
                chicago::map_t const& map = maps[i];
                mat.maps[i].uv_scale      = map.map.uv_scale * base_map_scale;
                mat.maps[i].bias          = bitm.image.bias;

                u16 flags = static_cast<u8>(map.color_func)
                            | (static_cast<u8>(map.alpha_func) << 4);
                mat.lightmap.meta1 |= flags << (i * 8);
            }

            mat.material.material = materials::id::schi;
            break;
        }
        case tag_class_t::senv: {
            shader_env const* info
                = shader.header->as<blam::shader::shader_env>();

            auto base = bitm_cache.assign_atlas_data(
                mat.maps[0], shader.senv.base_bitm);
            if(base)
            {
                mat.maps[0].uv_scale = Vecf2{1};
                mat.maps[0].bias     = base->image.bias;
            }

            auto* micro = bitm_cache.assign_atlas_data(
                mat.maps[1], shader.senv.micro_bitm);
            if(micro)
            {
                mat.maps[1].uv_scale = Vecf2(info->diffuse.micro.scale);
                mat.maps[1].bias     = micro->image.bias;
            }

            mat.material.flags |= (micro ? 1 : 0) << 10;

            auto* primary = bitm_cache.assign_atlas_data(
                mat.maps[2], shader.senv.primary_bitm);
            if(primary)
            {
                mat.maps[2].uv_scale = Vecf2(info->diffuse.primary.scale);
                mat.maps[2].bias     = primary->image.bias;
            }

            auto* secondary = bitm_cache.assign_atlas_data(
                mat.maps[3], shader.senv.secondary_bitm);
            if(secondary)
            {
                mat.maps[3].uv_scale = Vecf2(info->diffuse.secondary.scale);
                mat.maps[3].bias     = secondary->image.bias;
            }

            mat.material.flags |= (primary && secondary ? 1 : 0) << 9;

            mat.lightmap.meta1
                = bitm_cache.get_atlas_layer(shader.senv.self_illum);

            auto* bump
                = bitm_cache.assign_atlas_data(mat.maps[4], shader.senv.bump);
            if(bump)
            {
                mat.maps[4].uv_scale = Vecf2(info->bump.scale);
                mat.maps[4].bias     = bump->image.bias;
            }

            mat.material.material = materials::id::senv;
            mat.material.flags    = static_cast<u32>(info->flags)
                                 | static_cast<u32>(info->shader_type) << 4;
            mat.material.inputs1[0] = info->reflection.lightmap_brightness;
            mat.material.inputs1[1] = info->specular.brightness;

            auto& reflection = info->reflection;
            if(shader.senv.reflection_bitm.valid())
            {
                mat.material.inputs[0] = Vecf4(
                    info->specular.perpendicular_color,
                    reflection.perpendicular_brightness);
                mat.material.inputs[1] = Vecf4(
                    info->specular.parallel_color,
                    reflection.parallel_brightness);
                mat.lightmap.reflection
                    = bitm_cache.get_atlas_layer(shader.senv.reflection_bitm);
                mat.material.flags |= 1 << 6; /* Flag for reflection */
                mat.material.flags |= static_cast<u32>(reflection.type) << 7;
            }

            /* Allocation of flag bits:
             * 0-3:   Top-level flags (senv::flags)
             * 4-5:   Shader type
             *
             * Reflection properties:
             * 6:     Reflection toggle
             * 7-8:   Reflection map type
             *
             * Blending properties:
             * 9-10:  Detail map function
             * 10-11: Micro map function
             *
             * Texture scrolling animation:
             * 12-15: U-animation function
             * 16-19: V-animation function
             *
             *
             */

            break;
        }
        case tag_class_t::swat: {
            shader_water const* info = shader.header->as<shader_water>();
            bitm_cache.assign_atlas_data(mat.maps[0], shader.swat.base);
            mat.maps[0].uv_scale = base_map_scale;
            mat.maps[0].bias     = 0;

            bitm_cache.assign_atlas_data(mat.maps[1], shader.swat.reflection);
            mat.lightmap.reflection = mat.maps[1].layer;

            bitm_cache.assign_atlas_data(mat.maps[1], shader.swat.bump);
            mat.maps[1].uv_scale = base_map_scale * info->ripple.scale;
            mat.maps[1].bias     = 2.f;

            mat.material.material = materials::id::swat;
            mat.material.flags    = static_cast<u32>(info->flags);
            mat.material.inputs1  = Vecf2{
                glm::radians(info->ripple.anim_angle),
                info->ripple.anim_velocity};
            mat.material.inputs[0]
                = Vecf4(info->parallel.tint_color, info->parallel.brightness);
            mat.material.inputs[1] = Vecf4(
                info->perpendicular.tint_color, info->perpendicular.brightness);
            break;
        }
        case tag_class_t::sgla: {
            //            auto const* info =
            //            shader.header->as<blam::shader::shader_glass>();
            bitm_cache.assign_atlas_data(mat.maps[0], shader.color_bitm);
            mat.maps[0].uv_scale = Vecf2(1);
            mat.maps[0].bias     = 0;

            mat.material.material = materials::id::sgla;
            break;
        }
        case tag_class_t::smet: {
            mat.material.material = materials::id::smet;
            break;
        }
        case tag_class_t::sotr: {
            shader_transparent const* info
                = shader.header->as<shader_transparent>();
            auto maps = info->maps.data(magic).value();
            //            auto stages = info->stages.data(magic).value();
            //            auto layers = info->layers.data(magic).value();
            //            cDebug(
            //                "{}: {} maps, {} stages, {} layers",
            //                shader.tag->to_name().to_string(magic),
            //                maps.size(),
            //                stages.size(),
            //                layers.size());

            for(auto i : range<>(maps.size()))
            {
                bitm_cache.assign_atlas_data(mat.maps[i], shader.sotr.maps[i]);
                mat.maps[i].uv_scale = maps[i].map.uv_scale;
                mat.maps[i].bias     = maps[i].map.mip_bias;
            }

            mat.material.material = materials::id::sotr;
            break;
        }
        case tag_class_t::soso: {
            shader_model const* info
                = shader.header->as<blam::shader::shader_model>();
            bitm_cache.assign_atlas_data(mat.maps[0], shader.soso.base_bitm);
            mat.maps[0].uv_scale = base_map_scale;
            mat.maps[0].bias     = 2;

            bitm_cache.assign_atlas_data(mat.maps[1], shader.soso.multi_bitm);
            mat.maps[1].uv_scale = Vecf2(1);
            mat.maps[1].bias     = 2;

            auto* detail = bitm_cache.assign_atlas_data(
                mat.maps[2], shader.soso.detail_bitm);
            if(detail)
            {
                mat.maps[2].uv_scale = Vecf2(info->maps.detail.scale);
                mat.maps[2].bias     = detail->image.bias;
            }

            mat.lightmap.reflection
                = bitm_cache.get_atlas_layer(shader.soso.reflection_bitm);
            if(mat.lightmap.reflection != 0)
                mat.material.flags |= 0x1;

            mat.material.material = materials::id::soso;
            break;
        }
        default: {
            cDebug(
                "Material not set up: {0}/{1}",
                shader.tag->tagclass.at(0).str(),
                shader.tag->to_name().to_string(magic));
            break;
        }
        }
    }

    void populate_transparent_material(
        materials::transparent_data& mat, generation_idx_t const& shader_id)
    {
        using namespace blam::shader;

        ShaderItem const&         shader = find(shader_id)->second;
        shader_transparent const* info
            = shader.header->as<shader_transparent>();

        auto stages = info->stages.data(magic).value();
        u32  i      = 0;
        for(shader_transparent::stage_t const& stage : stages)
            mat.stages[i++] = materials::transparent_data::from_blam(stage);
    }

    template<typename PropertyAnim>
    requires stl_types::is_any_of<
        PropertyAnim,
        blam::shader::texture_property_anim,
        blam::shader::simple_tex_property_anim>
        //
        f32 tex_animation(PropertyAnim const& anim, f32 const& time)
    {
        using namespace blam::shader;
        switch(anim.function)
        {
        case animation_function::slide:
        case animation_function::slide_variable:
            return std::fmod(time, anim.period);
        case animation_function::cosine:
        case animation_function::cosine_variable:
            return glm::cos(time * anim.period + anim.phase) * anim.scale;
        default:
            return 0;
        }
    }

    template<typename Anim>
    requires stl_types::is_any_of<
        Anim,
        blam::shader::texture_uv_rotation_animation,
        blam::shader::simple_texture_uv_animation>
        //
        Vecf2 uv_animation(Anim const& anim, f32 const& time)
    {
        return Vecf2{tex_animation(anim.u, time), tex_animation(anim.v, time)};
    }

    void populate_chicago_uv_anims(
        materials::shader_data&                         mat,
        Span<blam::shader::chicago::map_t const> const& maps,
        f32                                             t)
    {
        using namespace blam::shader;

        u32 i = 0;
        for(chicago::map_t const& map : maps)
        {
            auto        uv = uv_animation(map.anim_2d, t);
            auto const& i2 = mat.material.inputs[0];
            switch(i)
            {
            case 0:
                mat.material.inputs1 = uv;
                break;
            case 1:
                mat.material.inputs[0] = Vecf4(uv, 0, 0);
                break;
            case 2:
                mat.material.inputs[0] = Vecf4(i2.x, i2.y, uv.x, uv.y);
                break;
            case 3:
                mat.material.inputs[1] = Vecf4(uv, 0, 0);
                break;
            }
            i++;
        }
    }

    void update_uv_animations(
        materials::shader_data& mat,
        generation_idx_t const& shader_id,
        time_point const&       time)
    {
        using namespace blam::shader;

        ShaderItem const& shader = find(shader_id)->second;

        f32 t = stl_types::Chrono::to_float(time) / 10.f;

        switch(shader.tag_class)
        {
        case blam::tag_class_t::scex: {
            shader_chicago_extended<V> const* info
                = shader.header->as<blam::shader::shader_chicago_extended<V>>();
            auto maps = info->maps_4stage.data(magic).value();
            populate_chicago_uv_anims(mat, maps, t);
            break;
        }
        case blam::tag_class_t::schi: {
            shader_chicago<V> const* info
                = shader.header->as<shader_chicago<V>>();
            auto maps = info->maps.data(magic).value();
            populate_chicago_uv_anims(mat, maps, t);
            break;
        }
        case blam::tag_class_t::senv: {
            shader_env const* info = shader.header->as<shader_env>();
            (void)info;
            break;
        }
        default:
            break;
        }
    }

    u32 get_id(blam::tagref_t const& args)
    {
        return args.tag_id;
    }

    void start_restricted(Proxy&, time_point const&)
    {
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }
};
