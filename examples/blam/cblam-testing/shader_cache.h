#pragma once

#include <set>
#include <tuple>

#include "bitmap_cache.h"
#include "caching_item.h"
#include "data.h"
#include "data_cache.h"
#include "materials.h"
#include <blam/volta/blam_scenario.h>

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
    blam::map_ptr           magic;
    stl_types::math::rng    random;

    template<blam::tag_class_t Tag>
    generation_idx_t get_bitm_idx(blam::tagref_typed_t<Tag> const& bitm)
    {
        if(!bitm.valid() || bitm.tag_class != blam::tag_class_t::bitm)
            return generation_idx_t();

        return bitm_cache.predict(bitm, 0);
    }

    template<typename T>
    T const* extract_shader(typename blam::tag_index_view<V>::iterator it)
    {
        return (*it).template data<T>(magic).value();
    }

    ShaderItem predict_impl(blam::tagref_t const& shader);

    using material_context = std::variant<blam::scn::unit const*>;

    void populate_material(
        materials::shader_data&         mat,
        generation_idx_t const&         shader_id,
        Vecf2 const&                    base_map_scale,
        std::optional<material_context> context = {});

    void populate_transparent_material(
        materials::transparent_data& mat, generation_idx_t const& shader_id)
    {
        using namespace blam::shader;

        ShaderItem const&         shader = find(shader_id)->second;
        shader_transparent const* info =
            shader.header->as<shader_transparent>();

        auto stages_ = info->stages.data(magic);
        if(stages_.has_error())
        {
            mat.num_stages = 0;
            return;
        }
        auto stages    = stages_.value();
        mat.num_stages = static_cast<u32>(std::min(stages.size(), size_t(7)));
        mat.blend_mode = static_cast<u32>(info->transparent.blend_function);
        for(u32 i = 0; i < mat.num_stages; i++)
            mat.stages[i] =
                materials::transparent_data::stage_t::from_blam(stages[i]);
        /* constant_color0's RGB is GLOBAL: every stage's constant_color0
         * input references stage 0's animated constant color (the shield's
         * blue), while the ALPHA stays per-stage (each stage's own animated
         * weight — the shield's stage 3 idles at 0, leaving v1 pure blue).
         * constant_color1 is fully per-stage. */
        for(u32 i = 1; i < mat.num_stages; i++)
        {
            mat.stages[i].color0 =
                Vecf4(Vecf3(mat.stages[0].color0), mat.stages[i].color0.w);
            mat.stages[i].color0_up = Vecf4(
                Vecf3(mat.stages[0].color0_up), mat.stages[i].color0_up.w);
        }
    }

    /* Periodic factor for a sotr stage's constant_color0 animation
     * (color0_func/color0_period on the stage). Same waveforms as
     * color_anim_factor. period==0 → no animation → keep the midpoint
     * that from_blam wrote. */
    f32 transparent_color0_factor(
        blam::shader::shader_transparent::stage_t const& s, f32 const& time)
    {
        using namespace blam::shader;
        switch(s.color0_func)
        {
        case animation_function::one:
            return 1.f;
        case animation_function::zero:
            return 0.f;
        case animation_function::jitter:
            return random.frand();
        default:
            break;
        }
        if(s.color0_period == 0.f)
            return 0.5f;
        f32 phase = glm::fract(time / s.color0_period);
        switch(s.color0_func)
        {
        case animation_function::slide:
        case animation_function::slide_variable:
            return 1.f - glm::abs(2.f * phase - 1.f);
        case animation_function::cosine:
        case animation_function::cosine_variable:
            return glm::cos(phase * glm::two_pi<f32>()) * 0.5f + 0.5f;
        default:
            return 0.5f;
        }
    }

    /* Per-frame: animate each stage's constant_color0 between its
     * lower/upper bounds (the generator shield's shimmer lives here —
     * a static midpoint saturates its alpha chain to a white blob). */
    void update_transparent_animations(
        materials::transparent_data& mat,
        generation_idx_t const&      shader_id,
        time_point const&            time)
    {
        using namespace blam::shader;

        if(!shader_id.valid())
            return;
        auto shader_it = find(shader_id);
        if(shader_it == end())
            return;
        ShaderItem const& shader = shader_it->second;
        if(shader.tag_class != blam::tag_class_t::sotr || !shader.valid())
            return;
        shader_transparent const* info =
            shader.header->as<shader_transparent>();
        auto stages_ = info->stages.data(magic);
        if(stages_.has_error())
            return;
        auto stages = stages_.value();

        double t64 = std::chrono::duration_cast<stl_types::Chrono::seconds_f64>(
                         time.time_since_epoch())
                         .count();
        f32 t = static_cast<f32>(std::fmod(t64, 3600.0));

        /* constant_color0 is global, defined by stage 0 (see
         * populate_transparent_material). */
        if(mat.num_stages == 0 || stages.empty())
            return;
        auto const& s0      = stages[0];
        auto        is_zero = [](Vecf4 const& c) {
            return c.x == 0.f && c.y == 0.f && c.z == 0.f && c.w == 0.f;
        };
        /* All-zero bounds = unset constant; from_blam substituted identity
         * white, keep it. */
        if(is_zero(s0.color0_lower) && is_zero(s0.color0_upper))
            return;
        f32 f;
        if(static_cast<u32>(s0.flags) & 0x4u)
            /* a_out_controls_color0_anim: driven by the object's exported
             * function (e.g. generator power), which we don't simulate.
             * Stand-in: steady low value — the generator shield reads as
             * violet there (blue constant blended slightly toward its red
             * counterpart); higher values shift it pink. */
            f = 0.15f;
        else
            f = transparent_color0_factor(s0, t);
        Vecf4 c = glm::mix(s0.color0_lower, s0.color0_upper, f);
        /* tag colors are ARGB → RGBA; RGB is global (stage 0), alpha is each
         * stage's own animated weight. */
        Vecf3 global_rgb(c.y, c.z, c.w);
        if(static_cast<u32>(s0.flags) & 0x4u)
            /* Part of the function stand-in: soften the constant toward
             * white so fully-constant-lit areas (shield spots where both
             * mask and scratch alpha drop out) read pale instead of
             * fully saturated. */
            global_rgb = glm::mix(global_rgb, Vecf3(1.f), 0.35f);
        /* mat lives in the mapped GPU buffer and may not be populated yet
         * on the first frame an instance appears (update runs before
         * populate) — clamp instead of trusting num_stages. */
        u32 n = std::min(mat.num_stages, 7u);
        for(u32 i = 0; i < n && i < stages.size(); i++)
        {
            Vecf4 own =
                glm::mix(stages[i].color0_lower, stages[i].color0_upper, f);
            mat.stages[i].color0 = Vecf4(global_rgb, own.x);
        }
    }

    // color_animation::anim has no source field in tag data; function is at
    // byte 0 (stored as 'source'), and 'scale' is actually phase (amplitude is
    // always 1)
    f32 color_anim_factor(
        blam::shader::simple_tex_property_anim const& anim, f32 const& time)
    {
        using namespace blam::shader;
        auto fn =
            static_cast<animation_function>(static_cast<u16>(anim.source));
        f32 phase = anim.scale;
        switch(fn)
        {
        case animation_function::one:
            return 1.f;
        case animation_function::zero:
            return 0.f;
        case animation_function::jitter:
            return random.frand();
        default:
            break;
        }
        if(anim.period == 0.f)
            return 0.f;
        switch(fn)
        {
        case animation_function::slide:
        case animation_function::slide_variable:
            return 1.f -
                   glm::abs(
                       2.f * glm::fract((time + phase) / anim.period) - 1.f);
        case animation_function::cosine:
        case animation_function::cosine_variable:
            return glm::cos(
                       glm::fract((time + phase) / anim.period) *
                       glm::two_pi<f32>()) *
                       0.5f +
                   0.5f;
        default:
            return 0.f;
        }
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
        case animation_function::one:
            return 1.f;
        case animation_function::zero:
            return 0.f;
        case animation_function::jitter:
            return random.frand() * anim.scale;
        default:
            break;
        }
        if(anim.period == 0.f)
            return 0.f;
        switch(anim.function)
        {
        case animation_function::slide:
        case animation_function::slide_variable:
            return glm::fract(time / anim.period) * anim.scale;
        case animation_function::cosine:
        case animation_function::cosine_variable: {
            f32 phase = glm::fract(time / anim.period) * glm::two_pi<f32>();
            if constexpr(requires { anim.phase; })
                return glm::cos(phase + anim.phase) * anim.scale;
            else
                return glm::cos(phase) * anim.scale;
        }
        case animation_function::jitter:
            return random.frand() * anim.scale;
        default:
            return 0.f;
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

        // Wrap to [0, 3600) before converting to f32 to avoid precision loss
        // at Unix epoch scale (~1.7e9s, where f32 ULP ≈ 256s)
        double t64 = std::chrono::duration_cast<stl_types::Chrono::seconds_f64>(
                         time.time_since_epoch())
                         .count();
        f32 t = static_cast<f32>(std::fmod(t64, 3600.0));

        switch(shader.tag_class)
        {
        case blam::tag_class_t::scex: {
            shader_chicago_extended<blam::pc_version_t> const* info =
                shader.header->as<blam::shader::shader_chicago_extended<
                    blam::pc_version_t>>();
            auto maps = info->maps_4stage.data(magic).value();
            populate_chicago_uv_anims(mat, maps, t);
            break;
        }
        case blam::tag_class_t::sotr: {
            /* Per-map UV scroll for the combiner: packed map0/1 into
             * inputs[0], map2/3 into inputs[1] (input2/input3 in GLSL). */
            shader_transparent const* info =
                shader.header->as<shader_transparent>();
            auto maps_ = info->maps.data(magic);
            if(maps_.has_error())
                break;
            auto  maps = maps_.value();
            Vecf4 uv01(0), uv23(0);
            for(u32 i = 0; i < maps.size() && i < 4u; i++)
            {
                Vecf2  uv  = uv_animation(maps[i].animation, t);
                Vecf4& dst = i < 2 ? uv01 : uv23;
                if(i % 2 == 0)
                {
                    dst.x = uv.x;
                    dst.y = uv.y;
                } else
                {
                    dst.z = uv.x;
                    dst.w = uv.y;
                }
            }
            mat.material.inputs[0] = uv01;
            mat.material.inputs[1] = uv23;
            break;
        }
        case blam::tag_class_t::schi: {
            shader_chicago<blam::pc_version_t> const* info =
                shader.header->as<shader_chicago<blam::pc_version_t>>();
            auto maps = info->maps.data(magic).value();
            populate_chicago_uv_anims(mat, maps, t);
            break;
        }
        case blam::tag_class_t::senv: {
            shader_env const* info = shader.header->as<shader_env>();
            using simple_uv        = blam::shader::simple_texture_uv_animation;
            auto uv =
                uv_animation(static_cast<simple_uv const&>(info->scrolling), t);
            auto& inp2 = mat.material.inputs[2];
            inp2       = Vecf4(uv.x, uv.y, inp2.z, inp2.w);

            if(shader.senv.self_illum.valid())
            {
                auto illum_color =
                    [&](blam::shader::color_animation const& ch) {
                        f32 factor =
                            glm::clamp(color_anim_factor(ch.anim, t), 0.f, 1.f);
                        return glm::mix(ch.color_off, ch.color_on, factor);
                    };
                auto const& si          = info->self_illum;
                f32         plasma_anim = color_anim_factor(si.plasma.anim, t);
                mat.material.inputs[3]  = Vecf4(illum_color(si.primary), 1.f);
                mat.material.inputs[4]  = Vecf4(illum_color(si.secondary), 1.f);
                // .rgb = plasma on_color, .a = animated value for A-channel
                // proximity
                mat.material.inputs[5] = Vecf4(si.plasma.color_on, plasma_anim);
            }
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
