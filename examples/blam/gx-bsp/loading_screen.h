#pragma once

/* Loading screen: the BlamGraphics loading still with a lit band that follows
 * progress, zoom-blurred by drawing a scaled copy of the previous frame (a
 * framebuffer copy into a texture) under the new one. Drawn from the load path
 * whenever it reports progress, so there's no thread and nothing runs between
 * steps. */

#include <coffee/gexxo/gexxo_api.h>
#include <coffee/graphics/apis/gexxo/rhi.h>
#include <coffee/graphics/apis/gexxo/rhi_draw.h>

#include <ogc/lwp_watchdog.h>

#include <algorithm>
#include <array>
#include <memory>

namespace gx_loading {

using libc_types::f32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;
using typing::vector_types::Vecf4;

class screen_t
{
  public:
    /* tpl: the loading image as a GX texture file */
    bool begin(gexxo::api& gfx, gsl::span<u8 const> tpl)
    {
        using namespace gexxo;
        using stage_t = program_t::stage_t;

        m_gfx   = &gfx;
        m_image = std::make_shared<texture_t>(
            textures::type::d2, typing::pixels::PixDesc{}, 1);
        if(!m_image->upload(std::move(tpl)))
        {
            m_image.reset();
            return false;
        }
        auto const fb = framebufferSize();
        m_w           = static_cast<u32>(fb.w);
        m_h           = static_cast<u32>(fb.h);

        m_feedback = std::make_shared<texture_t>(
            textures::type::d2,
            typing::pixels::PixDesc{typing::pixels::pix_fmt::RGB565},
            1);
        m_feedback->alloc({m_w / 2, m_h / 2, 1});
        m_feedback_rt = gfx.alloc_rendertarget();
        m_feedback_rt->attach(
            render_targets::attachment::color, *m_feedback, 0);

        /* One quad each: the still, last frame, the band */
        m_vbo = gfx.alloc_buffer(buffers::vertex, semantic::RSCA::ReadWrite);
        m_ebo = gfx.alloc_buffer(buffers::element, semantic::RSCA::ReadOnly);
        m_vbo->commit(quads * 4 * sizeof(vertex));
        std::array<u16, quads * 6> indices;
        for(u16 q = 0; q < quads; q++)
            for(u16 i = 0; i < 6; i++)
                indices[q * 6 + i] =
                    q * 4 + std::array<u16, 6>{0, 1, 2, 0, 2, 3}[i];
        m_ebo->commit(gsl::span<u16 const>(indices.data(), indices.size()));
        m_vao = gfx.alloc_vertex_array();
        m_vao->add({
            .index = 0,
            .role  = vertex_attribute::position,
            .value = {.offset = 0, .stride = sizeof(vertex), .count = 2},
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
        m_vao->set_buffer(buffers::element, m_ebo);

        program_t::channel_t const vertex_color{
            .diffuse_src = program_t::channel_t::src_vertex};
        stage_t const textured{
            .stage    = stage_t::stage0,
            .op       = stage_t::modulate,
            .texcoord = stage_t::texcoord0,
            .texmap   = stage_t::texmap0};
        m_textured           = gfx.alloc_program();
        m_textured->channels = {vertex_color};
        m_textured->stages   = {textured};

        /* Overexposed so the band stands out from the dark still */
        stage_t lit        = textured;
        lit.op             = stage_t::custom;
        lit.color_combiner = {GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO};
        lit.color_combiner.scale = GX_CS_SCALE_4;
        lit.alpha_combiner = {GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO};
        m_lit              = gfx.alloc_program();
        m_lit->channels    = {vertex_color};
        m_lit->stages      = {lit};

        m_last = 0;
        draw();
        return true;
    }

    /* Progress from here on maps onto [from, to] of the whole load */
    void phase(f32 from, f32 to)
    {
        m_from = from;
        m_to   = to;
        step(0, 1);
    }

    void step(u32 done, u32 total)
    {
        if(!m_image || !total)
            return;
        f32 const p =
            m_from + (m_to - m_from) * std::min(1.f, f32(done) / total);
        m_progress = std::max(m_progress, p); // never backwards
        if(ticks_to_millisecs(gettime() - m_last) >= frame_ms)
            draw();
    }

    /* The quads stay: draws are cached by their vertex array */
    void finish()
    {
        if(!m_image)
            return;
        m_progress = 1.f;
        draw();
        m_feedback_rt.reset();
        m_feedback.reset();
        m_image.reset();
    }

  private:
    static constexpr u32 frame_ms = 100;
    static constexpr f32 band     = 0.18f; // width, as a fraction of the screen
    static constexpr f32 zoom     = 1.05f;
    static constexpr u8  dim      = 0x40; // the still behind the band
    static constexpr u16 quads    = 3;

    struct vertex
    {
        f32 x, y;
        u8  r, g, b, a;
        f32 u, v;
    };

    /* x0..x1, y0..y1 in pixels; alpha at the left and right edges */
    void quad(
        u16 q,
        f32 x0,
        f32 y0,
        f32 x1,
        f32 y1,
        u8  shade,
        u8  a0,
        u8  a1,
        f32 u0 = 0.f,
        f32 u1 = 1.f)
    {
        auto v = m_vbo->map<vertex>(q * 4 * sizeof(vertex), 4 * sizeof(vertex));
        auto const nx = [this](f32 x) { return x / m_w * 2.f - 1.f; };
        auto const ny = [this](f32 y) { return 1.f - y / m_h * 2.f; };
        v[0]          = {nx(x0), ny(y0), shade, shade, shade, a0, u0, 0};
        v[1]          = {nx(x1), ny(y0), shade, shade, shade, a1, u1, 0};
        v[2]          = {nx(x1), ny(y1), shade, shade, shade, a1, u1, 1};
        v[3]          = {nx(x0), ny(y1), shade, shade, shade, a0, u0, 1};
    }

    void submit(
        u16                               q,
        std::shared_ptr<gexxo::program_t> program,
        std::shared_ptr<gexxo::texture_t> texture,
        gexxo::blend_state const&         blend)
    {
        gexxo::draw_command cmd{
            .call     = {.indexed = true, .instanced = true},
            .data     = {.elements = {.count = 6, .offset = q * 6u}},
            .program  = std::move(program),
            .vertices = m_vao,
            .textures = {{.texture = std::move(texture)}},
        };
        cmd.data.instances.count = 1;
        m_gfx->submit(
            cmd, blend, gexxo::depth_state{.test = false, .write = false});
    }

    void draw()
    {
        f32 const w = static_cast<f32>(m_w), h = static_cast<f32>(m_h);

        // The still is 16:9, letterboxed
        f32 const ih = w * 9.f / 16.f;
        f32 const y0 = (h - ih) * 0.5f, y1 = y0 + ih;
        quad(0, 0, y0, w, y1, dim, 0xFF, 0xFF);

        // Last frame, slightly enlarged: streaks grow outwards and fade
        f32 const dx = w * (zoom - 1.f) * 0.5f, dy = h * (zoom - 1.f) * 0.5f;
        quad(1, -dx, -dy, w + dx, h + dy, 0xFF, 0x90, 0x90);

        // The lit band trails the progress point, brightest at its front
        f32 const x1 = m_progress * w;
        f32 const x0 = std::max(0.f, x1 - band * w);
        quad(2, x0, y0, x1, y1, 0xFF, 0x00, 0xFF, x0 / w, x1 / w);
        m_vbo->unmap();

        auto rt = m_gfx->default_rendertarget();
        rt->clear(Vecf4{0, 0, 0, 1}, 1.0, 0);
        submit(0, m_textured, m_image, {.opaque = true});
        if(m_have_feedback)
            submit(1, m_textured, m_feedback, {.additive = true});
        submit(2, m_lit, m_image, {.additive = true});

        m_feedback_rt->copy(
            *rt,
            {0, 0, m_w, m_h},
            {0, 0},
            gexxo::render_targets::attachment::color,
            gexxo::render_targets::attachment::color);
        m_have_feedback = true;
        gexxo::swapBuffers();
        m_last = gettime();
    }

    gexxo::api*                            m_gfx{nullptr};
    std::shared_ptr<gexxo::texture_t>      m_image, m_feedback;
    std::shared_ptr<gexxo::rendertarget_t> m_feedback_rt;
    std::shared_ptr<gexxo::buffer_t>       m_vbo, m_ebo;
    std::shared_ptr<gexxo::vertex_array_t> m_vao;
    std::shared_ptr<gexxo::program_t>      m_textured, m_lit;
    bool                                   m_have_feedback{false};
    u32                                    m_w{640}, m_h{480};
    f32                                    m_from{0}, m_to{1}, m_progress{0};
    u64                                    m_last{0};
};

} // namespace gx_loading
