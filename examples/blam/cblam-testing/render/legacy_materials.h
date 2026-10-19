#pragma once

#include "bitmap_cache.h"
#include "caching.h"
#include "components.h"
#include "graphics_api.h"
#include "materials.h"
#include "shader_compiler.h"

#include <coffee/core/CProfiling>
#include <fmt/format.h>

#include <array>
#include <map>
#include <string>
#include <unordered_map>

/* Shaders for the enhanced legacy renderer. A material's maps, formats and
 * switches become constants of material_legacy.{vert,frag}, and each distinct
 * set compiles once. Material parameters come from the same
 * ShaderCache::populate_material the uber shaders read. */
template<typename V>
struct LegacyMaterials
{
    using Vecf2 = typing::vector_types::Vecf2;
    using Vecf3 = typing::vector_types::Vecf3;
    using Vecf4 = typing::vector_types::Vecf4;
    using Matf4 = typing::vector_types::Matf4;
    using f32   = libc_types::f32;
    using u32   = libc_types::u32;
    using u64   = libc_types::u64;
    using i16   = libc_types::i16;
    using i32   = libc_types::i32;

    using constants_t = gfx::shader_t::constants_t;

    enum class geometry_t : u32
    {
        bsp     = 0,
        batched = 1,
        skinned = 2,
    };

    static constexpr u32 sky_lights       = 4;
    static constexpr u32 instance_vectors = 5;
    /* camera, eye, map_uv, key_light, ambient, sky_lights */
    static constexpr u32 reserved_vectors = 4 + 1 + 5 + 1 + 1 + sky_lights * 2;
    static constexpr u32 transparent_stages = 7;
    static constexpr f32 resample_distance  = .25f; /* wu */

    /* One map: the texture of its layer and the bucket sampler it filters
     * like */
    struct map_t
    {
        std::shared_ptr<gfx::texture_t> texture;
        std::shared_ptr<gfx::sampler_t> filtering;
        u32                             format{0};
        Vecf4                           fade{1.f}; /* average rgb, texels */

        explicit operator bool() const
        {
            return texture && format != 0;
        }
    };

    struct material_t
    {
        materials::shader_data      data{};
        materials::transparent_data transparent{};
        std::array<map_t, 5>        maps;
        map_t                       cube;
        std::array<Vecf4, 5>        map_uv{};
        std::array<Vecf4, 5>        fade{};
        constants_t                 constants;
        bool                        ok{false};
        bool                        lit{false}; /* reads the lightmap */
        generation_idx_t            shader{};
    };

    /* The lightmap under an object, looked up again once it moves */
    struct probe_t
    {
        Vecf3            at{};
        i16              section{-1};
        u32              generation{0};
        u64              frame{0};
        generation_idx_t bitm{};
        Vecf2            uv{};
        Vecf3            direction{}; /* length = how directional */
    };

    struct frame_t
    {
        gfx::api*                         api;
        BlamResources const*              resources;
        BitmapCache<V>*                   bitm;
        ShaderCache<V>*                   shaders;
        Matf4                             camera;
        Vecf3                             eye;
        time_point                        time;
        Vecf3                             key_light{0, 0, 1};
        Vecf3                             ambient{};
        std::array<Vecf4, sky_lights * 2> lights{};
    };

    u32  model_batch{0};
    u32  max_bones{0};
    f32  la_textures{0.f};
    bool loaded{false};

    std::map<std::string, std::shared_ptr<gfx::program_t>> m_programs;
    std::array<std::shared_ptr<gfx::sampler_t>, 8>         m_units;
    std::unordered_map<u64, probe_t>                       m_probes;
    std::unordered_map<u64, Vecf4>                         m_fades;
    std::map<std::tuple<u64, f32, f32>, material_t>        m_frame_materials;
    /* By shader, geometry and lightmap format */
    std::map<std::pair<u64, u32>, std::shared_ptr<gfx::program_t>> m_variants;
    u32 m_variant_generation{0};
    u32 m_fade_generation{0};
    u64 m_frame{0};

    bool load(gfx::api& api)
    {
        if(loaded)
            return model_batch > 0;
        loaded            = true;
        u32 const vectors = api.limits().shaders.vertex_uniform_vectors;
        u32 const free =
            vectors > reserved_vectors ? vectors - reserved_vectors : 0;
        model_batch = std::min<u32>(free / instance_vectors, 32);
        /* Halo caps skeletons at 64 */
        max_bones = std::min<u32>(free > 2 ? (free - 2) / 3 : 0, 64);
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        la_textures = 0.f;
#else
        la_textures = api.feature_info().texture.tex.ext.rg8 ? 0.f : 1.f;
#endif
        if(model_batch == 0)
            Coffee::cWarning(
                "Enhanced materials need more than {} vertex uniform vectors",
                vectors);
        return model_batch > 0;
    }

    std::shared_ptr<gfx::program_t> program(
        gfx::api& api, constants_t const& constants)
    {
        using namespace Coffee::resource_literals;
        std::string key;
        for(auto const& [name, value] : constants)
            key += name + "=" + value + ";";
        if(auto it = m_programs.find(key); it != m_programs.end())
            return it->second;
        Coffee::ProfContext _("legacy_render: Compile material");
        auto                program = api.alloc_program();
        program->add(
            gfx::program_t::stage_t::Vertex,
            api.alloc_shader(
                "shaders/material_legacy.vert"_rsc.data(), constants));
        program->add(
            gfx::program_t::stage_t::Fragment,
            api.alloc_shader(
                "shaders/material_legacy.frag"_rsc.data(), constants));
        if(auto res = program->compile(); res.has_error())
        {
            Coffee::cWarning(
                "Failed to compile material {}: {}",
                key,
                std::get<0>(res.error()));
            program.reset();
        }
        m_programs[key] = program;
        return program;
    }

    map_t map_of(BitmapCache<V>& cache, generation_idx_t const& id)
    {
        if(!id.valid())
            return {};
        auto it = cache.find(id);
        if(it == cache.end() || !it->second.valid())
            return {};
        BitmapItem const& bitm = it->second;
        auto& bucket = cache.template get_bucket<gfx::compat::texture_2da_t>(
            bitm.image.fmt);
        auto sub =
            bucket.template texture_as<gfx::compat::texture_2da_t>().subtexture(
                bitm.image.layer);
        return {
            .texture   = sub,
            .filtering = bucket.sampler,
            .format    = cache.type_mask(bitm) >> 24,
            .fade      = fade_of(cache, id, bitm),
        };
    }

    /* What a map averages to, and its size, for fading out detail */
    Vecf4 fade_of(
        BitmapCache<V>&         cache,
        generation_idx_t const& id,
        BitmapItem const&       bitm)
    {
        if(m_fade_generation != cache.generation)
            m_fades.clear(), m_fade_generation = cache.generation;
        auto [it, fresh] = m_fades.try_emplace(id.i);
        if(fresh)
        {
            Vecf2 const texels = bitm.texel_count();
            it->second         = Vecf4(
                Vecf3(cache.average_color(bitm)), std::max(texels.x, texels.y));
        }
        return it->second;
    }

    map_t cube_of(BitmapCache<V>& cache, generation_idx_t const& id)
    {
        auto cube = cache.cube_texture(id);
        if(!cube)
            return {};
        return {
            .texture   = cube,
            .filtering = cache.cube_sampler(),
            .format    = 1,
        };
    }

    /* Each unit gets its own sampler: on ES2 a sampler is the texture it
     * binds, so two maps sharing a bucket sampler would sample one image */
    gfx::sampler_definition_t bind(
        u32 unit, std::string_view name, map_t const& map)
    {
        auto& sampler = m_units.at(unit);
        if(!sampler)
        {
            sampler = std::make_shared<gfx::sampler_t>(map.texture);
            sampler->alloc();
        }
        sampler->rebind(map.texture);
        if(map.filtering)
            sampler->set_filtering(
                map.filtering->m_mag,
                map.filtering->m_min,
                map.filtering->m_mip);
        return gleam::sampler_definition_t{
            typing::graphics::ShaderStage::Fragment,
            {name, static_cast<i32>(unit)},
            sampler};
    }

    std::vector<gfx::sampler_definition_t> samplers(
        material_t const& mat, map_t const& light)
    {
        /* light is only passed to materials that read it */
        static constexpr std::array<std::string_view, 5> names = {
            {"map0", "map1", "map2", "map3", "map4"}};
        std::vector<gfx::sampler_definition_t> out;
        for(u32 i = 0; i < mat.maps.size(); i++)
            if(mat.maps[i])
                out.push_back(bind(i, names[i], mat.maps[i]));
        if(light)
            out.push_back(bind(5, "lightmap", light));
        if(mat.cube)
            out.push_back(bind(6, "cube", mat.cube));
        return out;
    }

    static std::string vec4_literal(Vecf4 const& v)
    {
        return fmt::format(
            "vec4({:.6f}, {:.6f}, {:.6f}, {:.6f})", v.x, v.y, v.z, v.w);
    }

    /* Everything a shader resolves from its tag: maps, switches and the
     * per-frame animation of scrolling, self-illumination and combiners */
    material_t material(
        frame_t const&                                           frame,
        generation_idx_t const&                                  shader,
        Vecf2 const&                                             base_scale,
        std::optional<typename ShaderCache<V>::material_context> context = {})
    {
        using materials::id;

        material_t out;
        out.shader = shader;
        auto it    = frame.shaders->find(shader);
        if(it == frame.shaders->end() || !it->second.valid())
            return out;
        ShaderItem const& item  = it->second;
        BitmapCache<V>&   cache = *frame.bitm;

        auto functions = frame.shaders->resolved_functions(context);
        frame.shaders->populate_material(out.data, shader, base_scale, context);
        frame.shaders->update_uv_animations(
            out.data, shader, frame.time, functions);

        auto const& data  = out.data;
        u32 const   flags = data.material.flags;
        auto&       c     = out.constants;
        c["MATERIAL"] =
            std::to_string(static_cast<u32>(data.material.material));
        c["LA_TEXTURES"] = la_textures > .5f ? "1" : "0";
        if constexpr(std::is_same_v<V, blam::xbox_version_t>)
            c["VERTEX_XBOX"] = "1";

        auto map_from =
            [&](u32 i, generation_idx_t const& bitm, Vecf2 offset = {}) {
                out.maps[i]   = map_of(cache, bitm);
                out.map_uv[i] = Vecf4(data.maps[i].uv_scale, offset);
                out.fade[i]   = out.maps[i].fade;
            };

        switch(item.tag_class)
        {
        case blam::tag_class_t::senv: {
            auto const& senv = item.senv;
            map_from(0, senv.base_bitm, Vecf2(data.material.inputs[2]));
            map_from(
                1, senv.self_illum.valid() ? senv.self_illum : senv.micro_bitm);
            map_from(2, senv.primary_bitm);
            map_from(3, senv.secondary_bitm);
            map_from(4, senv.bump);
            out.cube          = cube_of(cache, senv.reflection_bitm);
            bool const illum  = data.lightmap.meta1 != 0;
            c["SENV_TYPE"]    = std::to_string((flags >> 4) & 0x3);
            c["ALPHA_TESTED"] = std::to_string(flags & 0x1);
            c["DETAIL_FUNC"]  = std::to_string((flags >> 20) & 0x3);
            c["MICRO_FUNC"]   = std::to_string((flags >> 22) & 0x3);
            c["SELF_ILLUM"]   = illum && out.maps[1] ? "1" : "0";
            c["MICRO"]        = !illum && out.maps[1] ? "1" : "0";
            c["REFLECTION"]   = (flags >> 6) & 0x1 && out.cube ? "1" : "0";
            break;
        }
        case blam::tag_class_t::schi:
        case blam::tag_class_t::scex: {
            auto const& maps = item.tag_class == blam::tag_class_t::schi
                                   ? item.schi.maps
                                   : item.scex.maps;
            auto const& in   = data.material.inputs;
            std::array<Vecf2, 4> offsets = {{
                data.material.inputs1,
                Vecf2(in[0].x, in[0].y),
                Vecf2(in[0].z, in[0].w),
                Vecf2(in[1].x, in[1].y),
            }};
            for(u32 i = 0; i < 4; i++)
                map_from(i, maps[i], offsets[i]);
            /* A map's functions fold it into the next one */
            for(u32 i = 1; i < 4; i++)
            {
                u32 const stage = (data.lightmap.meta1 >> ((i - 1) * 8)) & 0xFF;
                c[fmt::format("C{}", i)] = std::to_string(stage & 0xF);
                c[fmt::format("A{}", i)] = std::to_string(stage >> 4);
            }
            c["BLEND"] = std::to_string(data.lightmap.meta2);
            break;
        }
        case blam::tag_class_t::sotr: {
            auto const& in = data.material.inputs;
            for(u32 i = 0; i < 4; i++)
                map_from(
                    i,
                    item.sotr.maps[i],
                    i % 2 == 0 ? Vecf2(in[i / 2].x, in[i / 2].y)
                               : Vecf2(in[i / 2].z, in[i / 2].w));
            frame.shaders->populate_transparent_material(
                out.transparent, shader);
            frame.shaders->update_transparent_animations(
                out.transparent, shader, frame.time, functions);
            transparent_constants(out.transparent, c);
            break;
        }
        case blam::tag_class_t::soso: {
            auto const& soso = item.soso;
            map_from(0, soso.base_bitm);
            map_from(1, soso.multi_bitm);
            map_from(2, soso.detail_bitm);
            out.cube = cube_of(cache, soso.reflection_bitm);
            u32 mask = 0;
            for(u32 bit = 0; bit < 8; bit++)
                if(flags & (0x8u << bit))
                    mask = bit + 1;
            c["DETAIL_FUNC"] = flags & 0x2 ? "1" : flags & 0x4 ? "2" : "0";
            c["DETAIL_MASK"] = std::to_string(mask);
            c["REFLECTION"]  = out.cube ? "1" : "0";
            break;
        }
        default:
            return out;
        }

        for(u32 i = 0; i < out.maps.size(); i++)
            c[fmt::format("FORMAT{}", i)] =
                std::to_string(out.maps[i] ? out.maps[i].format : 0);
        out.ok  = out.maps[0] || item.tag_class == blam::tag_class_t::sotr;
        out.lit = item.tag_class == blam::tag_class_t::senv ||
                  item.tag_class == blam::tag_class_t::soso;
        return out;
    }

    /* Materials without per-object state, resolved once a frame */
    material_t& cached(
        frame_t const&          frame,
        generation_idx_t const& shader,
        Vecf2 const&            base_scale)
    {
        auto key = std::make_tuple(shader.i, base_scale.x, base_scale.y);
        if(auto it = m_frame_materials.find(key); it != m_frame_materials.end())
            return it->second;
        return m_frame_materials
            .emplace(key, material(frame, shader, base_scale))
            .first->second;
    }

    /* The variant of a material for one kind of geometry, null if it failed
     * to compile */
    std::shared_ptr<gfx::program_t> const& program_for(
        frame_t const&    frame,
        material_t const& mat,
        geometry_t        geometry,
        map_t const&      lightmap)
    {
        if(m_variant_generation != frame.bitm->generation)
            m_variants.clear(), m_variant_generation = frame.bitm->generation;
        u32 const variant =
            static_cast<u32>(geometry) << 8 | (lightmap ? lightmap.format : 0);
        auto [it, fresh] = m_variants.try_emplace({mat.shader.i, variant});
        if(!fresh)
            return it->second;
        constants_t constants    = mat.constants;
        constants["GEOMETRY"]    = std::to_string(static_cast<u32>(geometry));
        constants["MODEL_BATCH"] = std::to_string(model_batch);
        constants["MAX_BONES"]   = std::to_string(max_bones);
        constants["SKY_LIGHTS"]  = std::to_string(sky_lights);
        if(lightmap)
        {
            constants["LIGHTMAP"]        = "1";
            constants["LIGHTMAP_FORMAT"] = std::to_string(lightmap.format);
        }
        it->second = program(*frame.api, constants);
        return it->second;
    }

    /* Each combiner stage's routing as one argument list, S0..S6 */
    static void transparent_constants(
        materials::transparent_data const& tr, constants_t& c)
    {
        u32 const stages = std::min(tr.num_stages, transparent_stages);
        c["STAGES"]      = std::to_string(stages);
        c["BLEND"]       = std::to_string(tr.blend_mode);
        for(u32 i = 0; i < stages; i++)
        {
            auto const& s   = tr.stages[i];
            auto        in4 = [](u32 v) {
                return fmt::format(
                    "{}, {}, {}, {}, {}, {}, {}, {}",
                    v & 0x1F,
                    (v >> 5) & 0x7,
                    (v >> 8) & 0x1F,
                    (v >> 13) & 0x7,
                    (v >> 16) & 0x1F,
                    (v >> 21) & 0x7,
                    (v >> 24) & 0x1F,
                    (v >> 29) & 0x7);
            };
            u32 const o              = s.outputs;
            c[fmt::format("S{}", i)] = fmt::format(
                "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}",
                in4(s.color_in),
                in4(s.alpha_in),
                o & 0xF,
                (o >> 4) & 0x1,
                (o >> 5) & 0xF,
                (o >> 9) & 0x1,
                (o >> 10) & 0xF,
                (o >> 14) & 0x7,
                (o >> 17) & 0xF,
                (o >> 21) & 0xF,
                (o >> 25) & 0xF,
                (o >> 29) & 0x7,
                s.flags & 0x7);
            c[fmt::format("S{}_C0_UP", i)] = vec4_literal(s.color0_up);
            c[fmt::format("S{}_C1", i)]    = vec4_literal(s.color1);
        }
    }

    /* The sun for bump shading, the ambient and sky lights for objects with
     * no lightmap below them */
    static void world_lighting(
        frame_t& frame, materials::world_data const& world)
    {
        frame.key_light = Vecf3(world.lighting[0].light_direction);
        bool key_found  = false;
        u32  count      = 0;
        for(auto const& light : world.lighting)
        {
            Vecf3 const dir = Vecf3(light.light_direction);
            if(glm::dot(dir, dir) < 1e-6f)
                continue;
            /* .w: 1 = exteriors, 2 = interiors */
            if((static_cast<u32>(light.light_direction.w) & 0x1) == 0)
                continue;
            if(!key_found)
                frame.key_light = dir, key_found = true;
            if(count < sky_lights)
            {
                frame.lights[count * 2] = Vecf4(glm::normalize(dir), 0.f);
                frame.lights[count * 2 + 1] =
                    Vecf4(Vecf3(light.light_color) * light.light_color.w, 0.f);
                count++;
            }
        }
        if(glm::dot(frame.key_light, frame.key_light) > 1e-6f)
            frame.key_light = glm::normalize(frame.key_light);
        frame.ambient =
            Vecf3(world.fog.outdoor_ambient) * world.fog.outdoor_ambient.w;
    }

    void begin_frame()
    {
        m_frame++;
        m_frame_materials.clear();
        std::erase_if(m_probes, [this](auto const& entry) {
            return entry.second.frame + 1 < m_frame;
        });
    }

    probe_t const& probe(
        BSPCache<V> const& bsp_cache,
        BitmapCache<V>&    cache,
        u64                entity,
        Model const&       model)
    {
        Vecf3 at = model.position;
        if(model.origin_object)
            if(auto obj = model.origin_object->template data<blam::scn::object>(
                   cache.magic);
               obj.has_value())
                at += model.rotation * obj.value()[0].bound_offset;

        u32 const generation = cache.generation ^ (bsp_cache.generation << 16);
        auto&     light      = m_probes[entity];
        light.frame          = m_frame;
        if(light.section == bsp_cache.active_section &&
           light.generation == generation &&
           glm::distance(light.at, at) < resample_distance)
            return light;
        light = {
            .at         = at,
            .section    = bsp_cache.active_section,
            .generation = generation,
            .frame      = m_frame,
        };
        BSPItem const* bsp = nullptr;
        for(auto const& [id, item] : bsp_cache.m_cache)
            if(item.valid() && item.section_idx == bsp_cache.active_section)
                bsp = &item;
        if(auto sample = bsp ? bsp->light_below(at) : std::nullopt)
        {
            light.bitm      = sample->light_bitm;
            light.uv        = glm::clamp(sample->uv, 0.f, 1.f);
            light.direction = sample->direction;
        }
        return light;
    }

    static Vecf4 probe_uv(probe_t const& p)
    {
        return Vecf4(p.uv, 0.f, 0.f);
    }

    static Vecf4 probe_direction(probe_t const& p)
    {
        return Vecf4(p.direction, 0.f);
    }
};
