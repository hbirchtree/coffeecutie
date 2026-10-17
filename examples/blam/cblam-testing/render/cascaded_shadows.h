#pragma once

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_rendertarget.h>
#include <glm/gtc/matrix_transform.hpp>
#include <peripherals/libc/types.h>
#include <peripherals/typing/enum/pixels/format.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

namespace blam_render {

using namespace libc_types;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using typing::vector_types::Veci4;

/*!
 * Cascaded shadow maps for one directional light and one or more views
 * (splitscreen). Each view has near cascades split by distance, and every
 * view shares the far ones, fitted once to the world's bounds. Each cascade
 * is a texel-snapped orthographic box around a bounding sphere (so shadows
 * hold still while the camera turns) and a tile of one depth atlas.
 *
 * Nothing here knows the game: callers draw their casters per tile with
 * `light_matrix()` and `viewport()`, skipping what `covers()` rejects, and
 * bind `sampling(view)` and `sampler()` for the receivers
 * (fragments/cascaded_shadows.glsl).
 */
struct CascadedShadowMap
{
    static constexpr u32 max_cascades = 4; /*!< A view samples at most these */
    static constexpr u32 max_views    = 4;

    struct config_t
    {
        u32 views{1};
        u32 view_cascades{2};   /*!< Fitted to each view */
        u32 shared_cascades{1}; /*!< Fitted to the world, 0 or 1 */
        u32 resolution{1536};   /*!< Per tile */
        f32 max_distance{60.f}; /*!< What the view cascades cover */
        f32 split_lambda{.75f}; /*!< 0 = even splits, 1 = logarithmic */
        f32 near_distance{.2f};
        f32 caster_reach{250.f}; /*!< How far towards the light casters count */
        f32 strength{.6f};
    };

    /*! A view frustum as an eye and the directions through its corners */
    struct frustum_t
    {
        Vecf3                eye;
        Vecf3                forward;
        std::array<Vecf3, 4> corners; /*!< Unit rays through the screen corners */
    };

    /*! What a view's receivers sample with, laid out as the GLSL uniforms */
    struct sampling_t
    {
        std::array<Matf4, max_cascades> matrices{}; /*!< World to atlas uv + depth */
        Vecf4 texel{0.f};  /*!< World size of a texel, per cascade */
        Vecf4 params{0.f}; /*!< cascades, strength, fade distance */
        std::array<Vecf4, max_cascades> tiles{}; /*!< Atlas rect, inset for PCF */
    };

    /*! The frustum of a camera from its inverse view-projection */
    static frustum_t frustum_from(Matf4 const& inverse_view_projection, Vecf3 const& eye)
    {
        auto through = [&](f32 x, f32 y) {
            Vecf4 const p = inverse_view_projection * Vecf4(x, y, .5f, 1.f);
            return glm::normalize(Vecf3(p) / p.w - eye);
        };
        return frustum_t{
            .eye     = eye,
            .forward = through(0.f, 0.f),
            .corners = {through(-1, -1), through(1, -1), through(1, 1), through(-1, 1)},
        };
    }

    void alloc(gfx::api& api, config_t const& config)
    {
        m_config                 = config;
        m_config.views           = std::clamp<u32>(config.views, 1, max_views);
        m_config.shared_cascades = std::min<u32>(config.shared_cascades, 1);
        m_config.view_cascades   = std::clamp<u32>(
            config.view_cascades, 1, max_cascades - m_config.shared_cascades);
        m_tiles = m_config.shared_cascades + m_config.views * m_config.view_cascades;
        m_columns = static_cast<u32>(std::ceil(std::sqrt(static_cast<f32>(m_tiles))));
        m_rows    = (m_tiles + m_columns - 1) / m_columns;
        m_light.assign(m_tiles, Matf4(1.f));
        m_texel.assign(m_tiles, 0.f);
        m_sampling.assign(m_config.views, {});

        auto const format = api.feature_info().rendertarget.high_precision_depth_format;
        auto const width  = static_cast<i32>(m_config.resolution * m_columns);
        auto const height = static_cast<i32>(m_config.resolution * m_rows);
        m_depth = api.alloc_texture(gfx::textures::d2, typing::pixels::PixDesc(format), 1);
        m_depth->alloc(size_3d<u32>{
            static_cast<u32>(width), static_cast<u32>(height), 1});
        m_target = api.alloc_rendertarget();
        m_target->alloc();
        m_target->attach(gfx::render_targets::attachment::depth, *m_depth, 0);
        m_target->resize({0, 0, width, height});
        m_sampler = m_depth->sampler();
        m_sampler->alloc();
        m_sampler->set_filtering(typing::Filtering::Nearest, typing::Filtering::Nearest);
    }

    bool valid() const
    {
        return m_target != nullptr;
    }

    config_t const& config() const
    {
        return m_config;
    }

    u32 tiles() const
    {
        return m_tiles;
    }

    /*! Tile of a view's cascade `i`; the shared ones come first */
    u32 view_tile(u32 view, u32 i) const
    {
        return m_config.shared_cascades + view * m_config.view_cascades + i;
    }

    /*! Fits the shared cascade around the world, the light shining along
     * -`to_light` */
    void update_shared(Vecf3 const& center, f32 radius, Vecf3 const& to_light)
    {
        if(m_config.shared_cascades)
            fit(0, center, radius, basis(to_light));
    }

    /*! Fits a view's cascades and what its receivers sample */
    void update_view(u32 view, frustum_t const& frustum, Vecf3 const& to_light)
    {
        if(view >= m_config.views)
            return;
        Matf4 const light = basis(to_light);
        u32 const   count = m_config.view_cascades;
        f32 const   near  = m_config.near_distance;
        f32 const   far   = m_config.max_distance;
        f32         from  = near;
        auto&       out   = m_sampling[view];
        for(u32 i = 0; i < count; i++)
        {
            /* Practical split: blend of logarithmic and even */
            f32 const t     = static_cast<f32>(i + 1) / static_cast<f32>(count);
            f32 const split = glm::mix(
                near + (far - near) * t, near * std::pow(far / near, t), m_config.split_lambda);

            /* The slice's corners, sphere-bounded so the box keeps its size */
            std::array<Vecf3, 8> corners;
            for(u32 c = 0; c < 4; c++)
            {
                f32 const along = glm::dot(frustum.corners[c], frustum.forward);
                corners[c]      = frustum.eye + frustum.corners[c] * (from / along);
                corners[c + 4]  = frustum.eye + frustum.corners[c] * (split / along);
            }
            Vecf3 center{0.f};
            for(auto const& c : corners)
                center += c / 8.f;
            f32 radius = 0.f;
            for(auto const& c : corners)
                radius = std::max(radius, glm::distance(center, c));
            fit(view_tile(view, i), center, radius, light);
            from = split;
        }

        /* Nearest first: a receiver takes the first cascade it lands in */
        u32 n = 0;
        for(u32 i = 0; i < count; i++)
            sample(out, n++, view_tile(view, i));
        for(u32 i = 0; i < m_config.shared_cascades; i++)
            sample(out, n++, i);
        out.params = Vecf4(
            static_cast<f32>(n),
            m_config.strength,
            m_config.shared_cascades ? 1e9f : far,
            0.f);
    }

    /*! A view's receivers; nothing to sample before the first update */
    sampling_t const& sampling(u32 view) const
    {
        static sampling_t const none{};
        return view < m_sampling.size() ? m_sampling[view] : none;
    }

    /*! World to clip for drawing casters into tile `i` */
    Matf4 const& light_matrix(u32 i) const
    {
        return m_light[i];
    }

    /*! Whether a caster's bounding sphere reaches into tile `i` */
    bool covers(u32 i, Vecf3 const& center, f32 radius) const
    {
        Vecf4 const p = m_light[i] * Vecf4(center, 1.f);
        f32 const   r = radius / std::max(m_texel[i] * m_config.resolution * .5f, 1e-6f);
        return std::abs(p.x) <= 1.f + r && std::abs(p.y) <= 1.f + r &&
               std::abs(p.z) <= 1.f + r;
    }

    Veci4 viewport(u32 i) const
    {
        auto const r = static_cast<i32>(m_config.resolution);
        return Veci4{r * static_cast<i32>(i % m_columns), r * static_cast<i32>(i / m_columns), r, r};
    }

    std::shared_ptr<gfx::rendertarget_t> const& target() const
    {
        return m_target;
    }

    std::shared_ptr<gfx::sampler_t> const& sampler() const
    {
        return m_sampler;
    }

    /*! Clears every tile before casters are drawn */
    void clear()
    {
        m_target->clear(1.0);
    }

  private:
    /* A fixed light basis, so texel snapping holds as the boxes move */
    static Matf4 basis(Vecf3 const& to_light)
    {
        Vecf3 const light = glm::normalize(to_light);
        Vecf3 const up    = std::abs(light.z) > .99f ? Vecf3{1.f, 0.f, 0.f}
                                                     : Vecf3{0.f, 0.f, 1.f};
        return glm::lookAt(Vecf3(0.f), -light, up);
    }

    void fit(u32 tile, Vecf3 const& center, f32 radius, Matf4 const& light)
    {
        radius            = std::ceil(radius * 16.f) / 16.f;
        f32 const texel   = 2.f * radius / static_cast<f32>(m_config.resolution);
        Vecf3     c       = Vecf3(light * Vecf4(center, 1.f));
        c.x               = std::floor(c.x / texel) * texel;
        c.y               = std::floor(c.y / texel) * texel;
        m_texel[tile]     = texel;
        m_light[tile]     = glm::ortho(
                            c.x - radius,
                            c.x + radius,
                            c.y - radius,
                            c.y + radius,
                            -(c.z + m_config.caster_reach),
                            radius - c.z) *
                        light;
    }

    /* Receivers land in a tile of the atlas, kept two texels in for PCF */
    void sample(sampling_t& out, u32 slot, u32 tile) const
    {
        f32 const w  = 1.f / static_cast<f32>(m_columns);
        f32 const h  = 1.f / static_cast<f32>(m_rows);
        f32 const x0 = w * static_cast<f32>(tile % m_columns);
        f32 const y0 = h * static_cast<f32>(tile / m_columns);
        Matf4 const to_tile =
            glm::translate(Matf4(1.f), Vecf3(x0, y0, 0.f)) *
            glm::scale(Matf4(1.f), Vecf3(w, h, 1.f)) *
            glm::translate(Matf4(1.f), Vecf3(.5f)) * glm::scale(Matf4(1.f), Vecf3(.5f));
        f32 const inset_x = 2.f * w / static_cast<f32>(m_config.resolution);
        f32 const inset_y = 2.f * h / static_cast<f32>(m_config.resolution);
        out.matrices[slot] = to_tile * m_light[tile];
        out.texel[slot]    = m_texel[tile];
        out.tiles[slot] = Vecf4(x0 + inset_x, y0 + inset_y, x0 + w - inset_x, y0 + h - inset_y);
    }

    config_t                             m_config;
    u32                                  m_tiles{0};
    u32                                  m_columns{1};
    u32                                  m_rows{1};
    std::shared_ptr<gfx::texture_2d_t>   m_depth;
    std::shared_ptr<gfx::rendertarget_t> m_target;
    std::shared_ptr<gfx::sampler_t>      m_sampler;
    std::vector<Matf4>                   m_light;
    std::vector<f32>                     m_texel;
    std::vector<sampling_t>              m_sampling;
};

} // namespace blam_render
