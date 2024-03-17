#pragma once

#include "bitmap_cache.h"
#include "caching_item.h"
#include "data.h"
#include "data_cache.h"
#include "materials.h"

using ShaderManifest =
    compo::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct ShaderCache
    : DataCache<ShaderItem, u32, blam::tagref_t const&>
    , compo::RestrictedSubsystem<ShaderCache<V>, ShaderManifest>
{
    using type  = ShaderCache<V>;
    using Proxy = compo::proxy_of<ShaderManifest>;

    ShaderCache(BitmapCache<V>& bitmap_cache)
        : bitm_cache(bitmap_cache)
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

    ShaderItem predict_impl(blam::tagref_t const& shader);

    void populate_material(
        materials::shader_data& mat,
        generation_idx_t const& shader_id,
        Vecf2 const&            base_map_scale);

    void populate_transparent_material(
        materials::transparent_data& mat, generation_idx_t const& shader_id)
    {
        using namespace blam::shader;

        ShaderItem const&         shader = find(shader_id)->second;
        shader_transparent const* info =
            shader.header->as<shader_transparent>();

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

        f32 t = stl_types::Chrono::to_f32(time) / 10.f;

        switch(shader.tag_class)
        {
        case blam::tag_class_t::scex: {
            shader_chicago_extended<V> const* info =
                shader.header->as<blam::shader::shader_chicago_extended<V>>();
            auto maps = info->maps_4stage.data(magic).value();
            populate_chicago_uv_anims(mat, maps, t);
            break;
        }
        case blam::tag_class_t::schi: {
            shader_chicago<V> const* info =
                shader.header->as<shader_chicago<V>>();
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
