#pragma once

#include "bitmap_cache.h"
#include "effects.h"
#include "graphics_api.h"
#include "shader_compiler.h"

#include <algorithm>
#include <map>
#include <tuple>
#include <vector>

/* Turns the EffectScene into camera-facing quads, sampled from the bitmap
 * atlas, and draws them grouped by blend mode */
template<typename V>
struct EffectDrawer
{
    using Vecf2 = typing::vector_types::Vecf2;
    using Vecf3 = typing::vector_types::Vecf3;
    using Vecf4 = typing::vector_types::Vecf4;
    using Matf3 = typing::vector_types::Matf3;
    using Matf4 = typing::vector_types::Matf4;
    using f32   = libc_types::f32;
    using i16   = libc_types::i16;
    using u32   = libc_types::u32;
    using blend_t = EffectScene::blend_t;

    struct vertex_t
    {
        Vecf3 position;
        Vecf3 axis;
        Vecf4 corner; /* x, y in [-1, 1], z radius, w rotation */
        Vecf2 tex;
        Vecf4 color;
        Vecf4 atlas;  /* scale xy, offset zw */
        Vecf4 source; /* format, layer, kind */
    };

    struct batch_t
    {
        u32 offset{0};
        u32 count{0};
        f32 mode{0.f}; /* what the shader does with the color */
        gfx::blend_state blend{};
    };

    struct image_t
    {
        Vecf4 rect{0.f, 0.f, 1.f, 1.f}; /* left, top, right, bottom */
        Vecf4 atlas{1.f, 1.f, 0.f, 0.f};
        f32   format{0.f};
        f32   layer{0.f};
        bool  ok{true};
    };

    std::shared_ptr<gfx::program_t>      program;
    std::shared_ptr<gfx::buffer_t>       buffer;
    std::shared_ptr<gfx::vertex_array_t> array;
    bool                                 failed{false};

    std::vector<vertex_t> vertices;
    std::vector<batch_t>  batches;

    std::map<std::tuple<blam::tagref_t const*, i16, i16>, image_t> images;
    u32 image_generation{~0u};

    bool load(gfx::api& api)
    {
        using namespace std::string_view_literals;
        if(program)
            return true;
        if(failed || std::get<0>(api.api_version()) < 3)
            return false;
        create_shaders(
            api,
            std::array<shader_pair_t, 1>{{
                {
                    .vertex_file   = "effects"sv,
                    .fragment_file = "effects"sv,
                    .shader        = program,
                },
            }});
        if(!program)
        {
            failed = true;
            return false;
        }
        buffer = api.alloc_buffer(
            gfx::buffers::vertex,
            semantic::RSCA::WriteOnly | semantic::RSCA::Streaming);
        buffer->alloc();
        array = api.alloc_vertex_array();
        array->alloc();
        array->add(gfx::vertex_attribute::from_member(&vertex_t::position));
        array->add(gfx::vertex_attribute::from_member(&vertex_t::axis).at(1));
        array->add(gfx::vertex_attribute::from_member(&vertex_t::corner).at(2));
        array->add(gfx::vertex_attribute::from_member(&vertex_t::tex).at(3));
        array->add(gfx::vertex_attribute::from_member(&vertex_t::color).at(4));
        array->add(gfx::vertex_attribute::from_member(&vertex_t::atlas).at(5));
        array->add(gfx::vertex_attribute::from_member(&vertex_t::source).at(6));
        array->set_buffer(gfx::buffers::vertex, buffer, 0);
        array->set_attribute_names({
            {"position", 0},
            {"axis", 1},
            {"corner", 2},
            {"tex", 3},
            {"color", 4},
            {"atlas", 5},
            {"source", 6},
        });
        return true;
    }

    /* A sequence's frame: a sprite on a sheet, or a whole image */
    image_t const& image(
        BitmapCache<V>& cache, blam::tagref_t const* bitmap, i16 sequence, i16 frame)
    {
        auto key = std::tuple{bitmap, sequence, frame};
        if(auto it = images.find(key); it != images.end())
            return it->second;
        image_t& out = images[key];
        if(!bitmap || !bitmap->valid())
            return out;
        out.ok     = false;
        auto first = cache.resolve(*bitmap, 0);
        if(!first.valid())
            return out;
        auto const* header = cache.find(first)->second.header;
        i16         index  = 0;
        if(auto sequences = header->sequences.data(cache.magic);
           sequences.has_value() && sequence >= 0 &&
           static_cast<size_t>(sequence) < sequences.value().size())
        {
            auto const& seq     = sequences.value()[static_cast<size_t>(sequence)];
            auto        sprites = seq.sprites.data(cache.magic);
            if(sprites.has_value() && !sprites.value().empty())
            {
                auto const& sprite =
                    sprites.value()[static_cast<size_t>(frame) % sprites.value().size()];
                index    = sprite.bitmap_index;
                out.rect = Vecf4(sprite.left, sprite.top, sprite.right, sprite.bottom);
            } else if(seq.bitmap_count > 0)
                index = static_cast<i16>(
                    seq.first_bitmap + static_cast<u32>(frame) % seq.bitmap_count);
        }
        struct
        {
            Vecf2 atlas_offset;
            Vecf2 atlas_scale;
            u32   layer;
        } atlas{};
        if(!cache.assign_atlas_data(atlas, cache.resolve(*bitmap, index)))
            return out;
        out.atlas  = Vecf4(atlas.atlas_scale, atlas.atlas_offset);
        out.format = static_cast<f32>(atlas.layer >> 24);
        out.layer  = static_cast<f32>(atlas.layer & 0xFFFFFF);
        out.ok     = true;
        return out;
    }

    static i32 group_of(blend_t blend)
    {
        switch(blend)
        {
        case blend_t::multiply:
        case blend_t::double_multiply:
        case blend_t::component_min:
            return 1;
        case blend_t::add:
            return 2;
        case blend_t::component_max:
            return 3;
        case blend_t::subtract:
            return -1;
        default:
            return 0;
        }
    }

    void build(
        EffectScene const& scene, BitmapCache<V>& cache, Vecf3 const& eye, u32 generation)
    {
        if(generation != image_generation)
        {
            images.clear();
            image_generation = generation;
        }
        vertices.clear();
        batches.clear();

        /* Back to front, so alpha blending layers right */
        std::vector<EffectScene::sprite_t const*> sprites;
        sprites.reserve(scene.sprites.size());
        for(auto const& s : scene.sprites)
            sprites.push_back(&s);
        std::sort(sprites.begin(), sprites.end(), [&eye](auto a, auto b) {
            return glm::distance(a->position, eye) > glm::distance(b->position, eye);
        });

        for(i32 group = 0; group < 4; group++)
        {
            batch_t batch{.offset = static_cast<u32>(vertices.size())};
            switch(group)
            {
            case 1:
                batch.mode  = 2.f;
                batch.blend = {.multiply = true};
                break;
            case 2:
                batch.mode  = 1.f;
                batch.blend = {.additive = true};
                break;
            case 3:
                batch.mode  = 1.f;
                batch.blend = {.maximum = true};
                break;
            default:
                break;
            }
            for(auto const* s : sprites)
                if(group_of(s->blend) == group)
                    add_sprite(cache, *s);
            for(auto const& r : scene.ribbons)
                if(group_of(r.blend) == group)
                    add_ribbon(cache, r);
            batch.count = static_cast<u32>(vertices.size()) - batch.offset;
            if(batch.count > 0)
                batches.push_back(batch);
        }
    }

    void add_sprite(BitmapCache<V>& cache, EffectScene::sprite_t const& s)
    {
        image_t const& img = image(cache, s.bitmap, s.sequence, s.frame);
        if(!img.ok || s.radius <= 0.f || s.color.a <= 0.f)
            return;
        constexpr std::array<Vecf2, 6> corners = {{
            {-1.f, -1.f}, {1.f, -1.f}, {1.f, 1.f},
            {-1.f, -1.f}, {1.f, 1.f}, {-1.f, 1.f},
        }};
        for(auto const& c : corners)
        {
            Vecf2 const t = (c + 1.f) * .5f;
            vertices.push_back({
                .position = s.position,
                .axis     = s.axis,
                .corner   = Vecf4(c, s.radius, s.rotation),
                .tex      = Vecf2(
                    glm::mix(img.rect.x, img.rect.z, t.x),
                    glm::mix(img.rect.w, img.rect.y, t.y)),
                .color    = s.color,
                .atlas    = img.atlas,
                .source   = Vecf4(img.format, img.layer, 0.f, 0.f),
            });
        }
    }

    void add_ribbon(BitmapCache<V>& cache, EffectScene::ribbon_t const& r)
    {
        image_t const& img = image(cache, r.bitmap, r.sequence, 0);
        if(!img.ok)
            return;
        auto const& pts = r.points;
        auto tangent = [&pts](size_t i) {
            size_t const a = i > 0 ? i - 1 : i;
            size_t const b = i + 1 < pts.size() ? i + 1 : i;
            return pts[b].position - pts[a].position;
        };
        auto vertex = [&](size_t i, f32 side) {
            return vertex_t{
                .position = pts[i].position,
                .axis     = tangent(i),
                .corner   = Vecf4(side, 0.f, pts[i].width * .5f, 0.f),
                .tex      = Vecf2(pts[i].v, side > 0.f ? 1.f : 0.f),
                .color    = pts[i].color,
                .atlas    = img.atlas,
                .source   = Vecf4(img.format, img.layer, 1.f, 0.f),
            };
        };
        for(size_t i = 0; i + 1 < pts.size(); i++)
        {
            vertex_t const a0 = vertex(i, -1.f), a1 = vertex(i, 1.f);
            vertex_t const b0 = vertex(i + 1, -1.f), b1 = vertex(i + 1, 1.f);
            vertices.insert(vertices.end(), {a0, b0, b1, a0, b1, a1});
        }
    }

    void upload()
    {
        if(!vertices.empty())
            buffer->commit(semantic::Bytes::ofContainer(vertices).view);
    }
};
