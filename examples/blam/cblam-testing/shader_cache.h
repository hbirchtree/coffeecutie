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
    : DataCache<ShaderItem, blam::tag_index<>, blam::tagref_t const&>
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

    /* What a draw knows about the object it belongs to. `unit` is the tag
     * side (change colours); the rest is per-object state. */
    struct material_context
    {
        blam::scn::unit const*         unit{nullptr};
        std::array<libc_types::f32, 4> object_function{
            {-1.f, -1.f, -1.f, -1.f}};
        libc_types::f32 meter_value{-1.f};
        libc_types::u8  numeric_place{0}; /*!< Digit of a numeric counter, 0 = ones */
    };

    /* A chicago shader that shows a digit of its first function */
    bool numeric(generation_idx_t const& shader_id)
    {
        auto it = find(shader_id);
        if(it == end() || it->second.tag_class != blam::tag_class_t::schi)
            return false;
        auto const* info = it->second.header->template as<
            blam::shader::shader_chicago<blam::pc_version_t>>();
        using flags_t = blam::shader::chicago::flags_t;
        return (static_cast<u8>(info->transparent.flags) &
                static_cast<u8>(flags_t::numeric)) != 0;
    }

    void populate_material(
        materials::shader_data&         mat,
        generation_idx_t const&         shader_id,
        Vecf2 const&                    base_map_scale,
        std::optional<material_context> context = {});

    /* Chicago maps: a cube first map is looked up by direction */
    void first_map_cube(
        materials::shader_data&       mat,
        blam::shader::chicago::map_type type,
        generation_idx_t const&       map);

    f32 meter_value_of(
        blam::shader::animation_src            src,
        std::optional<material_context> const& context) const;

    std::array<f32, 4> resolved_functions(
        std::optional<material_context> const& context) const;

    generation_idx_t reflection_bitmap(generation_idx_t const& shader_id)
    {
        auto it = find(shader_id);
        if(it == m_cache.end())
            return generation_idx_t();
        ShaderItem const& shader = it->second;
        if(!shader.valid())
            return generation_idx_t();
        switch(shader.tag_class)
        {
        case blam::tag_class_t::senv:
            return shader.senv.reflection_bitm;
        case blam::tag_class_t::soso:
            return shader.soso.reflection_bitm;
        case blam::tag_class_t::swat:
            return shader.swat.reflection;
        case blam::tag_class_t::sgla:
            return shader.sgla.reflection_cube;
        default:
            return generation_idx_t();
        }
    }

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

    /* What an object's change colours A..D are now: a permutation's colour
     * where it has one, else its bounds scaled by the function named. None
     * where the object has no such colour */
    std::array<std::optional<Vecf3>, 4> change_colors(
        blam::scn::unit const* unit, std::array<f32, 4> const& functions) const
    {
        using blam::shader::param_src;
        std::array<std::optional<Vecf3>, 4> out{};
        auto colors = unit ? unit->change_colors.data(magic)
                           : decltype(unit->change_colors.data(magic)){};
        if(!colors.has_value())
            return out;
        auto function_of = [&functions](param_src src) -> std::optional<f32> {
            auto const i = static_cast<u32>(src);
            if(i < static_cast<u32>(param_src::A_out) || i > static_cast<u32>(param_src::D_out))
                return std::nullopt;
            f32 const v = functions[i - static_cast<u32>(param_src::A_out)];
            return v >= 0.f ? std::optional(glm::clamp(v, 0.f, 1.f)) : std::nullopt;
        };
        for(size_t i = 0; i < out.size() && i < colors.value().size(); i++)
        {
            auto const& color = colors.value()[i];
            auto        perms = color.permutations.data(magic);
            if(perms.has_value() && !perms.value().empty())
            {
                out[i] = perms.value()[0].upper_bound;
                continue;
            }
            Vecf3 c = color.upper_bound;
            if(auto scale = function_of(color.scale_by))
                c = glm::mix(color.lower_bound, color.upper_bound, *scale);
            if(auto darken = function_of(color.darken_by))
                c *= 1.f - *darken;
            out[i] = c;
        }
        return out;
    }

    /* Per-frame: animate each stage's constant_color0 between its
     * lower/upper bounds (the generator shield's shimmer lives here —
     * a static midpoint saturates its alpha chain to a white blob). A
     * source names the object's change colour that tints the constant;
     * an object without it falls back to the function as the ramp */
    void update_transparent_animations(
        materials::transparent_data&               mat,
        generation_idx_t const&                    shader_id,
        time_point const&                          time,
        std::array<f32, 4> const&                  functions = {{-1.f, -1.f, -1.f, -1.f}},
        std::array<std::optional<Vecf3>, 4> const& colors    = {})
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

        double t64 = std::chrono::duration_cast<stl_types::chrono::seconds_f64>(
                         time.time_since_epoch())
                         .count();
        f32 t = static_cast<f32>(std::fmod(t64, 3600.0));

        if(mat.num_stages == 0 || stages.empty())
            return;
        auto is_zero = [](Vecf4 const& c) {
            return c.x == 0.f && c.y == 0.f && c.z == 0.f && c.w == 0.f;
        };
        /* The ramp's factor, and whether an object function it names was
         * there to give it: a_out_controls_color0_anim rides A_out; a source
         * rides time while it tints, and the function where it cannot */
        auto factor_of = [&](shader_transparent::stage_t const& s) {
            auto const src = s.color0_source;
            bool const a_out = static_cast<u32>(s.flags) & 0x4u;
            auto const i     = static_cast<size_t>(src) - 1u;
            bool const tinted = src != animation_src::none && i < colors.size() && colors[i];
            if(!a_out && (src == animation_src::none || tinted))
                return std::pair{transparent_color0_factor(s, t), false};
            auto const fi    = a_out ? 0u : i;
            bool const given = fi < functions.size() && functions[fi] >= 0.f;
            return std::pair{given ? glm::clamp(functions[fi], 0.f, 1.f) : 0.f, given};
        };
        auto tint_of = [&](shader_transparent::stage_t const& s) {
            auto const i = static_cast<size_t>(s.color0_source) - 1u;
            return s.color0_source != animation_src::none && i < colors.size() && colors[i]
                       ? *colors[i]
                       : Vecf3(1.f);
        };

        /* Each combiner stage has its own constant_color0 (tag colors are
         * ARGB). mat lives in the mapped GPU buffer and may not be populated
         * yet on the first frame an instance appears (update runs before
         * populate) — clamp instead of trusting num_stages. */
        u32 n = std::min(mat.num_stages, 7u);
        for(u32 i = 0; i < n && i < stages.size(); i++)
        {
            auto const& s = stages[i];
            /* All-zero bounds = unset; from_blam substituted identity white */
            if(is_zero(s.color0_lower) && is_zero(s.color0_upper))
                continue;
            auto const [factor, given] = factor_of(s);
            Vecf4 const c   = glm::mix(s.color0_lower, s.color0_upper, factor);
            Vecf3       rgb = Vecf3(c.y, c.z, c.w) * tint_of(s);
            u32         flags = static_cast<u32>(s.flags);
            if(flags & 0x4u)
            {
                /* Given its function, the ramp is settled here; without one
                 * the shader rides its own alpha instead, softened toward
                 * white so areas lit purely by it read pale */
                if(given)
                    flags &= ~0x4u;
                else
                    rgb = glm::mix(rgb, Vecf3(1.f), 0.35f);
            }
            mat.stages[i].flags  = flags;
            mat.stages[i].color0 = Vecf4(rgb, c.x);
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
    f32 tex_animation(
        PropertyAnim const&       anim,
        f32 const&                time,
        std::array<f32, 4> const& functions = {{0.f, 0.f, 0.f, 0.f}})
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

        f32 phase01;
        if(anim.source != animation_src::none)
        {
            auto const i = static_cast<size_t>(anim.source) - 1u;
            phase01      = i < functions.size() ? functions[i] : 0.f;
        } else
        {
            if(anim.period == 0.f)
                return 0.f;
            phase01 = glm::fract(time / anim.period);
        }

        switch(anim.function)
        {
        case animation_function::slide:
        case animation_function::slide_variable:
            return phase01 * anim.scale;
        case animation_function::cosine:
        case animation_function::cosine_variable: {
            f32 phase = phase01 * glm::two_pi<f32>();
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
    Vecf2 uv_animation(
        Anim const&               anim,
        f32 const&                time,
        std::array<f32, 4> const& functions = {{0.f, 0.f, 0.f, 0.f}})
    {
        return Vecf2{
            tex_animation(anim.u, time, functions),
            tex_animation(anim.v, time, functions)};
    }

    void populate_chicago_uv_anims(
        materials::shader_data&                         mat,
        Span<blam::shader::chicago::map_t const> const& maps,
        f32                                             t,
        std::array<f32, 4> const&                       functions)
    {
        using namespace blam::shader;

        u32 i = 0;
        for(chicago::map_t const& map : maps)
        {
            auto        uv = uv_animation(map.anim_2d, t, functions);
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
        materials::shader_data&   mat,
        generation_idx_t const&   shader_id,
        time_point const&         time,
        std::array<f32, 4> const& functions = {{0.f, 0.f, 0.f, 0.f}})
    {
        using namespace blam::shader;

        ShaderItem const& shader = find(shader_id)->second;

        // Wrap to [0, 3600) before converting to f32 to avoid precision loss
        // at Unix epoch scale (~1.7e9s, where f32 ULP ≈ 256s)
        double t64 = std::chrono::duration_cast<stl_types::chrono::seconds_f64>(
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
            populate_chicago_uv_anims(mat, maps, t, functions);
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
                Vecf2  uv  = uv_animation(maps[i].animation, t, functions);
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
            populate_chicago_uv_anims(mat, maps, t, functions);
            break;
        }
        case blam::tag_class_t::senv: {
            shader_env const* info = shader.header->as<shader_env>();
            using simple_uv        = blam::shader::simple_texture_uv_animation;
            auto uv                = uv_animation(
                static_cast<simple_uv const&>(info->scrolling), t, functions);
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

    blam::tag_index<> get_id(blam::tagref_t const& args)
    {
        return args.tag_id;
    }
};
