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

namespace blam_render {

using namespace libc_types;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using typing::vector_types::Veci4;

/*!
 * Cascaded shadow maps for one directional light. The view is split into
 * cascades by distance, each fitted with a texel-snapped orthographic box
 * around a bounding sphere (so shadows hold still while the camera turns),
 * and rendered into its own tile of one depth atlas. Nothing here knows the
 * game: callers draw their casters once per cascade with `light_matrix()`
 * and `viewport()`, and bind `sampling` and `sampler()` for the receivers
 * (fragments/cascaded_shadows.glsl).
 */
struct CascadedShadowMap
{
    static constexpr u32 max_cascades = 4;

    struct config_t
    {
        u32 cascades{3};
        u32 resolution{1536};   /*!< Per cascade tile */
        f32 max_distance{90.f}; /*!< Shadows fade out by here */
        f32 split_lambda{.75f}; /*!< 0 = even splits, 1 = logarithmic */
        f32 near_distance{.2f};
        f32 caster_reach{250.f}; /*!< How far towards the light casters count */
    };

    /*! A view frustum as an eye and the directions through its corners */
    struct frustum_t
    {
        Vecf3                eye;
        Vecf3                forward;
        std::array<Vecf3, 4> corners; /*!< Unit rays through the screen corners */
    };

    /*! What receivers sample with, laid out as the GLSL uniforms */
    struct sampling_t
    {
        std::array<Matf4, max_cascades> matrices{}; /*!< World to atlas uv + depth */
        Vecf4 texel{0.f};  /*!< World size of a texel, per cascade */
        Vecf4 params{0.f}; /*!< cascades, atlas texel width, strength, distance */
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
        m_config          = config;
        m_config.cascades = std::clamp<u32>(config.cascades, 1, max_cascades);
        auto const format = api.feature_info().rendertarget.high_precision_depth_format;
        m_depth = api.alloc_texture(
            gfx::textures::d2, typing::pixels::PixDesc(format), 1);
        m_depth->alloc(size_3d<u32>{m_config.resolution * m_config.cascades,
                                    m_config.resolution,
                                    1});
        m_target = api.alloc_rendertarget();
        m_target->alloc();
        m_target->attach(gfx::render_targets::attachment::depth, *m_depth, 0);
        m_target->resize(
            {0,
             0,
             static_cast<i32>(m_config.resolution * m_config.cascades),
             static_cast<i32>(m_config.resolution)});
        m_sampler = m_depth->sampler();
        m_sampler->alloc();
        m_sampler->set_filtering(typing::Filtering::Nearest, typing::Filtering::Nearest);
    }

    bool valid() const
    {
        return m_target != nullptr;
    }

    /*! Fits the cascades to `view`, the light shining along -`to_light` */
    void update(frustum_t const& view, Vecf3 const& to_light, f32 strength = .6f)
    {
        Vecf3 const light = glm::normalize(to_light);
        Vecf3 const up    = std::abs(light.z) > .99f ? Vecf3{1.f, 0.f, 0.f}
                                                     : Vecf3{0.f, 0.f, 1.f};
        u32 const   count = m_config.cascades;
        f32 const   near  = m_config.near_distance;
        f32 const   far   = m_config.max_distance;
        f32 const   atlas = 1.f / static_cast<f32>(count);
        Matf4 const light_basis = glm::lookAt(Vecf3(0.f), -light, up);
        for(u32 i = 0; i < count; i++)
        {
            /* Practical split: blend of logarithmic and even */
            f32 const t      = static_cast<f32>(i + 1) / static_cast<f32>(count);
            f32 const log_d  = near * std::pow(far / near, t);
            f32 const even_d = near + (far - near) * t;
            f32 const split  = glm::mix(even_d, log_d, m_config.split_lambda);
            f32 const from   = i == 0 ? near : m_splits[i - 1];
            m_splits[i]      = split;

            /* The slice's corners, sphere-bounded so the box keeps its size */
            Vecf3 center{0.f};
            std::array<Vecf3, 8> corners;
            for(u32 c = 0; c < 4; c++)
            {
                f32 const along = glm::dot(view.corners[c], view.forward);
                corners[c]      = view.eye + view.corners[c] * (from / along);
                corners[c + 4]  = view.eye + view.corners[c] * (split / along);
            }
            for(auto const& c : corners)
                center += c / 8.f;
            f32 radius = 0.f;
            for(auto const& c : corners)
                radius = std::max(radius, glm::distance(center, c));
            radius = std::ceil(radius * 16.f) / 16.f;

            /* Snap the centre to whole texels of a fixed light basis */
            f32 const texel = 2.f * radius / static_cast<f32>(m_config.resolution);
            sampling.texel[i] = texel;
            Vecf3 c = Vecf3(light_basis * Vecf4(center, 1.f));
            c.x     = std::floor(c.x / texel) * texel;
            c.y     = std::floor(c.y / texel) * texel;
            m_light[i] = glm::ortho(
                             c.x - radius,
                             c.x + radius,
                             c.y - radius,
                             c.y + radius,
                             -(c.z + m_config.caster_reach),
                             radius - c.z) *
                         light_basis;

            /* Receivers land in this cascade's tile of the atlas */
            Matf4 const to_tile =
                glm::translate(Matf4(1.f), Vecf3(atlas * i, 0.f, 0.f)) *
                glm::scale(Matf4(1.f), Vecf3(atlas, 1.f, 1.f)) *
                glm::translate(Matf4(1.f), Vecf3(.5f)) *
                glm::scale(Matf4(1.f), Vecf3(.5f));
            sampling.matrices[i] = to_tile * m_light[i];
        }
        sampling.params = Vecf4(
            static_cast<f32>(count),
            atlas / static_cast<f32>(m_config.resolution),
            strength,
            far);
    }

    u32 cascades() const
    {
        return m_config.cascades;
    }

    /*! World to clip for drawing casters into cascade `i` */
    Matf4 const& light_matrix(u32 i) const
    {
        return m_light[i];
    }

    /*! Cascade `i`'s tile of the atlas */
    Veci4 viewport(u32 i) const
    {
        auto const r = static_cast<i32>(m_config.resolution);
        return Veci4{r * static_cast<i32>(i), 0, r, r};
    }

    std::shared_ptr<gfx::rendertarget_t> const& target() const
    {
        return m_target;
    }

    std::shared_ptr<gfx::sampler_t> const& sampler() const
    {
        return m_sampler;
    }

    /*! Clears every cascade before casters are drawn */
    void clear()
    {
        m_target->clear(1.0);
    }

    sampling_t sampling;

  private:
    config_t                             m_config;
    std::shared_ptr<gfx::texture_2d_t>   m_depth;
    std::shared_ptr<gfx::rendertarget_t> m_target;
    std::shared_ptr<gfx::sampler_t>      m_sampler;
    std::array<Matf4, max_cascades>      m_light{};
    std::array<f32, max_cascades>        m_splits{};
};

} // namespace blam_render
