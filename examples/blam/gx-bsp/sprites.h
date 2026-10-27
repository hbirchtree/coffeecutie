#pragma once

/* Camera-facing sprites: particles, the glow of plasma bolts. Gathered over a
 * frame, then drawn in one batch per texture and blend, their vertices sent
 * straight to the GP (they change every frame, so no display lists). */

#include <coffee/gexxo/gexxo_api.h>
#include <coffee/graphics/apis/gexxo/rhi.h>
#include <coffee/graphics/apis/gexxo/rhi_draw.h>

#include <glm/glm.hpp>
#include <malloc.h>
#include <ogc/cache.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

namespace gx_sprites {

using libc_types::f32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

/* One image of a sprite sheet: u0, v0, u1, v1 */
struct frame_t
{
    std::shared_ptr<gexxo::texture_t> texture;
    glm::vec4                         uv{0.f, 0.f, 1.f, 1.f};
};

enum class blend_t : u8
{
    add,
    alpha,
    multiply,
};

class batcher_t
{
  public:
    void begin(gexxo::api& gfx)
    {
        using namespace gexxo;
        using stage_t = program_t::stage_t;

        m_gfx = &gfx;
        m_vbo = gfx.alloc_buffer(buffers::vertex, semantic::RSCA::ReadWrite);
        m_vbo->commit(max_sprites * 6 * sizeof(vertex));
        m_vao = gfx.alloc_vertex_array();
        m_vao->add({
            .index = 0,
            .role  = vertex_attribute::position,
            .value = {.offset = 0, .stride = sizeof(vertex), .count = 3},
        });
        m_vao->add({
            .index = 1,
            .role  = vertex_attribute::color0,
            .value =
                {.offset = offsetof(vertex, r),
                 .stride = sizeof(vertex),
                 .count  = 4,
                 .type   = semantic::type_t::u8},
        });
        m_vao->add({
            .index = 2,
            .role  = vertex_attribute::texcoord0,
            .value =
                {.offset = offsetof(vertex, u),
                 .stride = sizeof(vertex),
                 .count  = 2},
        });
        m_vao->set_buffer(buffers::vertex, m_vbo, 0);

        m_program           = gfx.alloc_program();
        m_program->channels = {
            {.diffuse_src = program_t::channel_t::src_vertex}};
        m_program->stages = {stage_t{
            .stage    = stage_t::stage0,
            .op       = stage_t::modulate,
            .texcoord = stage_t::texcoord0,
            .texmap   = stage_t::texmap0}};

        /* Sheets whose alpha the GX formats dropped: their backgrounds are
         * black, so how bright a texel is stands in for its coverage */
        GX_SetTevSwapModeTable(
            GX_TEV_SWAP3, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_GREEN);
        m_coverage           = gfx.alloc_program();
        m_coverage->channels = m_program->channels;
        m_coverage->stages   = {stage_t{
              .stage          = stage_t::stage0,
              .op             = stage_t::custom,
              .texcoord       = stage_t::texcoord0,
              .texmap         = stage_t::texmap0,
              .color_combiner = {GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO},
              .alpha_combiner = {GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO},
              .tex_swap       = GX_TEV_SWAP3}};
    }

    /* What a sprite without a bitmap shows: a soft round glow */
    std::shared_ptr<gexxo::texture_t> glow()
    {
        if(m_glow)
            return m_glow;
        constexpr u32 size = 32;
        // IA8 in 4x4 tiles: intensity, then alpha
        auto* texels = static_cast<u8*>(memalign(32, size * size * 2));
        if(!texels)
            return nullptr;
        u32 i = 0;
        for(u32 ty = 0; ty < size; ty += 4)
            for(u32 tx = 0; tx < size; tx += 4)
                for(u32 y = ty; y < ty + 4; y++)
                    for(u32 x = tx; x < tx + 4; x++)
                    {
                        f32 const r = glm::length(
                            glm::vec2{x + .5f, y + .5f} / f32(size) * 2.f -
                            1.f);
                        f32 const a = std::clamp(1.f - r, 0.f, 1.f);
                        texels[i++] = 0xFF;
                        texels[i++] = static_cast<u8>(a * a * 255.f);
                    }
        DCFlushRange(texels, size * size * 2);
        m_glow = std::make_shared<gexxo::texture_t>(
            gexxo::textures::type::d2, typing::pixels::PixDesc{}, 1);
        m_glow->init_raw(texels, size, size, GX_TF_IA8, GX_CLAMP, GX_CLAMP);
        return m_glow;
    }

    /* A square of `radius` facing the camera, turned by `rotation` */
    void add(
        frame_t const&   frame,
        blend_t          blend,
        glm::vec3 const& center,
        f32              radius,
        glm::vec4 const& color,
        f32              rotation = 0.f)
    {
        if(!frame.texture || m_sprites.size() >= max_sprites || radius <= 0.f)
            return;
        m_sprites.push_back(
            {frame.texture.get(),
             frame.uv,
             center,
             radius,
             rotation,
             color,
             blend});
    }

    /* view: world to eye */
    void flush(glm::mat4 const& view)
    {
        if(m_sprites.empty() || !m_gfx)
            return;
        std::stable_sort(
            m_sprites.begin(),
            m_sprites.end(),
            [](sprite_t const& a, sprite_t const& b) {
                return std::tie(a.blend, a.texture) <
                       std::tie(b.blend, b.texture);
            });

        glm::vec3 const right{view[0][0], view[1][0], view[2][0]};
        glm::vec3 const up{view[0][1], view[1][1], view[2][1]};
        auto out = m_vbo->map<vertex>(0, m_sprites.size() * 6 * sizeof(vertex));
        for(size_t i = 0; i < m_sprites.size(); i++)
        {
            auto const&     s  = m_sprites[i];
            f32 const       c  = std::cos(s.rotation) * s.radius;
            f32 const       n  = std::sin(s.rotation) * s.radius;
            glm::vec3 const x  = right * c + up * n;
            glm::vec3 const y  = up * c - right * n;
            auto const      ch = [](f32 v) {
                return static_cast<u8>(std::clamp(v, 0.f, 1.f) * 255.f);
            };
            u8 const rgba[4] = {
                ch(s.color.r), ch(s.color.g), ch(s.color.b), ch(s.color.a)};
            auto const v = [&](glm::vec3 const& p, f32 u, f32 t) {
                return vertex{
                    p.x, p.y, p.z, rgba[0], rgba[1], rgba[2], rgba[3], u, t};
            };
            // Clockwise on screen, which GX takes as the front
            vertex const tl = v(s.center - x + y, s.uv.x, s.uv.y);
            vertex const tr = v(s.center + x + y, s.uv.z, s.uv.y);
            vertex const br = v(s.center + x - y, s.uv.z, s.uv.w);
            vertex const bl = v(s.center - x - y, s.uv.x, s.uv.w);
            vertex*      q  = &out[i * 6];
            q[0]            = tl;
            q[1]            = tr;
            q[2]            = br;
            q[3]            = tl;
            q[4]            = br;
            q[5]            = bl;
        }
        m_vbo->unmap();

        glm::mat4 const views[1] = {view};
        size_t          first    = 0;
        while(first < m_sprites.size())
        {
            size_t last = first + 1;
            while(last < m_sprites.size() &&
                  m_sprites[last].blend == m_sprites[first].blend &&
                  m_sprites[last].texture == m_sprites[first].texture)
                last++;
            draw(
                m_sprites[first],
                static_cast<u32>(first * 6),
                static_cast<u32>((last - first) * 6),
                views);
            first = last;
        }
        m_sprites.clear();
    }

  private:
    static constexpr size_t max_sprites = 512;

    struct vertex
    {
        f32 x, y, z;
        u8  r, g, b, a;
        f32 u, v;
    };

    struct sprite_t
    {
        gexxo::texture_t* texture;
        glm::vec4         uv;
        glm::vec3         center;
        f32               radius;
        f32               rotation;
        glm::vec4         color;
        blend_t           blend;
    };

    void draw(sprite_t const& s, u32 offset, u32 count, glm::mat4 const* views)
    {
        gexxo::draw_command cmd{
            .call =
                {.indexed   = false,
                 .instanced = true,
                 .mode      = gexxo::drawing::primitive::triangle,
                 .streamed  = true},
            .data =
                {.arrays    = {.count = count, .offset = offset},
                 .instances = {.count = 1}},
            .program             = s.texture->m_gxfmt == GX_TF_CMPR ||
                               s.texture->m_gxfmt == GX_TF_RGB565
                                       ? m_coverage
                                       : m_program,
            .vertices            = m_vao,
            .instance_transforms = gsl::span<glm::mat4 const>(views, 1),
        };
        // Shared, not owned: the sprite only borrows its frame's texture
        cmd.textures = {
            {.texture = std::shared_ptr<gexxo::texture_t>(
                 std::shared_ptr<void>{}, s.texture)}};
        gexxo::depth_state const depth{.test = true, .write = false};
        switch(s.blend)
        {
        case blend_t::add:
            m_gfx->submit(cmd, gexxo::blend_state{.additive = true}, depth);
            break;
        case blend_t::multiply:
            m_gfx->submit(cmd, gexxo::blend_state{.multiply = true}, depth);
            break;
        case blend_t::alpha:
            m_gfx->submit(cmd, gexxo::blend_state{}, depth);
            break;
        }
    }

    gexxo::api*                            m_gfx{nullptr};
    std::shared_ptr<gexxo::buffer_t>       m_vbo;
    std::shared_ptr<gexxo::vertex_array_t> m_vao;
    std::shared_ptr<gexxo::program_t>      m_program, m_coverage;
    std::shared_ptr<gexxo::texture_t>      m_glow;

    std::vector<sprite_t> m_sprites;
};

} // namespace gx_sprites
