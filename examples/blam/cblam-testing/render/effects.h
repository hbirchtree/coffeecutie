#pragma once

#include <blam/volta/blam_particles.h>
#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <array>
#include <optional>
#include <vector>

/* What effects look like this frame, rebuilt every frame by EffectSystem and
 * drawn by EffectRenderer. Colors are rgba, positions world space. */
struct EffectScene : compo::SubsystemBase
{
    using type = EffectScene;

    using Vecf3 = typing::vector_types::Vecf3;
    using Vecf4 = typing::vector_types::Vecf4;
    using Vec2    = typing::vector_types::Vecf2;
    using f32   = libc_types::f32;
    using i16   = libc_types::i16;
    using blend_t = blam::scn::framebuffer_blending;
    using u32     = libc_types::u32;

    /* The local seats that see it, as for the model it comes off: a
     * first-person model's only its own, a held weapon's all but its holder's */
    struct audience_t
    {
        std::optional<u32> only_seat;
        std::optional<u32> skip_seat;

        bool sees(u32 seat) const
        {
            return only_seat ? *only_seat == seat : skip_seat != seat;
        }

        bool operator==(audience_t const&) const = default;
    };

    struct sprite_t
    {
        Vecf3 position{};
        f32   radius{0.f};
        Vecf3 axis{};    /* parallel_to_direction: stretched along this */
        bool  flat{false}; /* perpendicular_to_direction: lies across axis */
        f32   rotation{0.f};
        Vecf4 color{1.f};
        blam::tagref_t const* bitmap{nullptr}; /* none: a soft round glow */
        i16                   sequence{0};
        i16                   frame{0};        /* wraps by the sequence */
        blend_t               blend{blend_t::add};
        audience_t            audience{};
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
        audience_t                  audience{};
    };

    struct light_t
    {
        Vecf3                 position{};
        f32                   radius{0.f};
        Vecf3                 color{1.f};
        blam::tagref_t const* lens_flare{nullptr};
        Vecf3                 direction{};       /* spotlights shine this way */
        f32                   cos_falloff{-1.f}; /* full brightness inside */
        f32                   cos_cutoff{-1.f};  /* dark outside, -1 = omni */
    };

    /* A mark on the structure: the world triangles it covers */
    struct decal_t
    {
        blam::tagref_t const* bitmap{nullptr};
        i16                   frame{0};
        blend_t               blend{blend_t::multiply};
        Vecf4                 color{1.f};
        std::vector<Vecf3>    positions; /* triangles */
        std::vector<Vec2>     uv;        /* 0..1 across the decal */
    };

    /* A lens flare and how much of it each local seat sees */
    struct flare_t
    {
        blam::scn::lens_flare const* tag{nullptr};
        Vecf3                        position{}; /* a direction when infinite */
        bool                         infinite{false};
        Vecf3                        color{1.f};
        std::array<f32, 4>           visibility{};
    };

    std::vector<sprite_t> sprites;
    std::vector<ribbon_t> ribbons;
    std::vector<light_t>  lights;
    std::vector<flare_t>  flares;
    std::vector<decal_t const*>
        decals; /* near the eye, owned by EffectSystem */

    /* The map load it was built for; its tag pointers are from that map */
    libc_types::u32 generation{0};

    void clear()
    {
        sprites.clear();
        ribbons.clear();
        lights.clear();
        flares.clear();
        decals.clear();
    }
};

/* Plays effe tags and object attachments: sounds, lights, particles,
 * particle systems, contrails and light volumes */
void alloc_effects(compo::EntityContainer& e);
