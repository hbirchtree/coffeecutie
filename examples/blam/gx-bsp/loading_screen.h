#pragma once

/* Loading screen: the BlamGraphics loading still with a lit band that follows
 * progress, zoom-blurred by drawing a scaled copy of the previous frame (an
 * EFB-to-texture copy) under the new one. Drawn from the load path whenever
 * it reports progress, so there's no thread and nothing runs between steps. */

#include <coffee/gexxo/gexxo_api.h>
#include <coffee/graphics/apis/gexxo/rhi_resources.h>

#include <malloc.h>
#include <ogc/gu.h>
#include <ogc/gx.h>
#include <ogc/lwp_watchdog.h>

#include <algorithm>
#include <memory>

namespace gx_loading {

using libc_types::f32;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;

class screen_t
{
  public:
    /* tpl: the loading image as a GX texture file */
    bool begin(gsl::span<u8 const> tpl)
    {
        m_image = std::make_shared<gexxo::texture_t>(
            gexxo::textures::type::d2, typing::pixels::PixDesc{}, 1);
        if(!m_image->upload(std::move(tpl)))
        {
            m_image.reset();
            return false;
        }
        auto const fb = gexxo::framebufferSize();
        m_w           = static_cast<u32>(fb.w);
        m_h           = static_cast<u32>(fb.h);
        u32 const size =
            GX_GetTexBufferSize(m_w / 2, m_h / 2, GX_TF_RGB565, GX_FALSE, 0);
        m_feedback = memalign(32, size);
        if(m_feedback)
            GX_InitTexObj(
                &m_feedback_obj,
                m_feedback,
                m_w / 2,
                m_h / 2,
                GX_TF_RGB565,
                GX_CLAMP,
                GX_CLAMP,
                GX_FALSE);
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

    void finish()
    {
        if(!m_image)
            return;
        m_progress = 1.f;
        draw();
        free(m_feedback);
        m_feedback = nullptr;
        m_image.reset();
    }

  private:
    static constexpr u32 frame_ms = 100;
    static constexpr f32 band     = 0.18f; // width, as a fraction of the screen
    static constexpr f32 zoom     = 1.05f;
    static constexpr u8  dim      = 0x40; // the still behind the band

    struct vertex
    {
        f32 x, y;
        u8  r, g, b, a;
        f32 u, v;
    };

    static void quad(vertex const (&v)[4])
    {
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        for(auto const& p : v)
        {
            GX_Position2f32(p.x, p.y);
            GX_Color4u8(p.r, p.g, p.b, p.a);
            GX_TexCoord2f32(p.u, p.v);
        }
        GX_End();
    }

    void draw()
    {
        f32 const w = static_cast<f32>(m_w), h = static_cast<f32>(m_h);
        Mtx44     proj;
        guOrtho(proj, 0, h, 0, w, 0, 1);
        GX_LoadProjectionMtx(proj, GX_ORTHOGRAPHIC);
        Mtx id;
        guMtxIdentity(id);
        GX_LoadPosMtxImm(id, GX_PNMTX0);
        GX_SetCurrentMtx(GX_PNMTX0);

        GX_SetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
        GX_SetCullMode(GX_CULL_NONE);
        GX_ClearVtxDesc();
        GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
        GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GX_SetNumChans(1);
        GX_SetChanCtrl(
            GX_COLOR0A0,
            GX_DISABLE,
            GX_SRC_VTX,
            GX_SRC_VTX,
            0,
            GX_DF_NONE,
            GX_AF_NONE);
        GX_SetNumTexGens(1);
        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
        GX_SetNumTevStages(1);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
        GX_SetCopyClear(GXColor{0, 0, 0, 0xFF}, GX_MAX_Z24);
        GX_InvalidateTexAll(); // the feedback texture was just written by GX

        // The still is 16:9, letterboxed
        f32 const ih = w * 9.f / 16.f;
        f32 const y0 = (h - ih) * 0.5f, y1 = y0 + ih;

        GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
        GX_LoadTexObj(&m_image->m_obj, GX_TEXMAP0);
        quad(
            {{0, y0, dim, dim, dim, 0xFF, 0, 0},
             {w, y0, dim, dim, dim, 0xFF, 1, 0},
             {w, y1, dim, dim, dim, 0xFF, 1, 1},
             {0, y1, dim, dim, dim, 0xFF, 0, 1}});

        GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
        if(m_feedback && m_have_feedback)
        {
            // Last frame, slightly enlarged: streaks grow outwards and fade
            f32 const dx = w * (zoom - 1.f) * 0.5f,
                      dy = h * (zoom - 1.f) * 0.5f;
            GX_LoadTexObj(&m_feedback_obj, GX_TEXMAP0);
            quad(
                {{-dx, -dy, 0xFF, 0xFF, 0xFF, 0x90, 0, 0},
                 {w + dx, -dy, 0xFF, 0xFF, 0xFF, 0x90, 1, 0},
                 {w + dx, h + dy, 0xFF, 0xFF, 0xFF, 0x90, 1, 1},
                 {-dx, h + dy, 0xFF, 0xFF, 0xFF, 0x90, 0, 1}});
        }

        // The lit band trails the progress point, brightest at its front,
        // overexposed so it stands out from the dark still
        GX_SetTevColorOp(
            GX_TEVSTAGE0,
            GX_TEV_ADD,
            GX_TB_ZERO,
            GX_CS_SCALE_4,
            GX_TRUE,
            GX_TEVPREV);
        f32 const x1 = m_progress * w;
        f32 const x0 = std::max(0.f, x1 - band * w);
        GX_LoadTexObj(&m_image->m_obj, GX_TEXMAP0);
        quad(
            {{x0, y0, 0xFF, 0xFF, 0xFF, 0x00, x0 / w, 0},
             {x1, y0, 0xFF, 0xFF, 0xFF, 0xFF, x1 / w, 0},
             {x1, y1, 0xFF, 0xFF, 0xFF, 0xFF, x1 / w, 1},
             {x0, y1, 0xFF, 0xFF, 0xFF, 0x00, x0 / w, 1}});

        if(m_feedback)
        {
            GX_SetTexCopySrc(0, 0, m_w, m_h);
            GX_SetTexCopyDst(m_w / 2, m_h / 2, GX_TF_RGB565, GX_TRUE);
            GX_CopyTex(m_feedback, GX_FALSE);
            GX_PixModeSync();
            m_have_feedback = true;
        }
        gexxo::swapBuffers();
        m_last = gettime();
    }

    std::shared_ptr<gexxo::texture_t> m_image;
    void*                             m_feedback{nullptr};
    GXTexObj                          m_feedback_obj{};
    bool                              m_have_feedback{false};
    u32                               m_w{640}, m_h{480};
    f32                               m_from{0}, m_to{1}, m_progress{0};
    u64                               m_last{0};
};

} // namespace gx_loading
