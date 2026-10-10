#pragma once

#include <blam/volta/blam_particles.h>
#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <vector>

/* What effects look like this frame, rebuilt every frame by EffectSystem and
 * drawn by EffectRenderer. Colors are rgba, positions world space. */
struct EffectScene : compo::SubsystemBase
{
    using type = EffectScene;

    using Vecf3 = typing::vector_types::Vecf3;
    using Vecf4 = typing::vector_types::Vecf4;
    using f32   = libc_types::f32;
    using i16   = libc_types::i16;
    using blend_t = blam::scn::framebuffer_blending;

    struct sprite_t
    {
        Vecf3 position{};
        f32   radius{0.f};
        Vecf3 axis{};    /* parallel_to_direction: stretched along this */
        f32   rotation{0.f};
        Vecf4 color{1.f};
        blam::tagref_t const* bitmap{nullptr}; /* none: a soft round glow */
        i16                   sequence{0};
        i16                   frame{0};        /* wraps by the sequence */
        blend_t               blend{blend_t::add};
    };

    struct ribbon_point_t
    {
        Vecf3 position{};
        f32   width{0.f};
        Vecf4 color{1.f};
        f32   v{0.f}; /* texture coordinate along the ribbon */
    };

    struct ribbon_t
    {
        blam::tagref_t const*       bitmap{nullptr};
        i16                         sequence{0};
        blend_t                     blend{blend_t::add};
        std::vector<ribbon_point_t> points; /* head first */
    };

    struct light_t
    {
        Vecf3                 position{};
        f32                   radius{0.f};
        Vecf3                 color{1.f};
        blam::tagref_t const* lens_flare{nullptr};
    };

    std::vector<sprite_t> sprites;
    std::vector<ribbon_t> ribbons;
    std::vector<light_t>  lights;

    void clear()
    {
        sprites.clear();
        ribbons.clear();
        lights.clear();
    }
};

/* Plays effe tags and object attachments: sounds, lights, particles,
 * particle systems, contrails and light volumes */
void alloc_effects(compo::EntityContainer& e);
