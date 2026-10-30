#pragma once

#include <blam/volta/blam_tag_ref.h>
#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <optional>
#include <string>
#include <vector>

/* The HUD of every local seat this frame, rebuilt by HudSystem and drawn by
 * UIRenderer. Each view is 480 units tall and as wide as its viewport's
 * aspect, so corner anchors reach the viewport's edges. */
struct HudScene : compo::SubsystemBase
{
    using type = HudScene;

    using Vecf2 = typing::vector_types::Vecf2;
    using Vecf4 = typing::vector_types::Vecf4;
    using f32   = libc_types::f32;
    using u16   = libc_types::u16;
    using u32   = libc_types::u32;

    static constexpr f32 height = 480.f;

    /* One image, or a sprite on it */
    struct sprite_t
    {
        Vecf2                 min{};
        Vecf2                 max{};
        blam::tagref_t const* bitmap{nullptr};
        u16                   image{0};
        Vecf4                 uv{0.f, 0.f, 1.f, 1.f}; /* left, top, right, bottom */
        Vecf4                 color{1.f};
        /* Meters: texels with alpha up to `fill` take `color`, the rest
         * `empty`; negative for plain sprites */
        f32   fill{-1.f};
        Vecf4 empty{0.f};
    };

    /* Messages and help text: runs of glyphs with icons between them,
     * laid out by the renderer in the HUD font */
    struct text_t
    {
        struct run_t
        {
            std::u16string          text; /*!< '\n' starts a line */
            std::optional<sprite_t> icon; /*!< after the text, from the line's top */
            f32                     advance{0.f};
        };
        std::vector<run_t> runs;
        Vecf2              at{}; /*!< top-left of the first line */
        Vecf4              color{1.f};
        bool               below{false}; /*!< under the text before it instead */
    };

    struct view_t
    {
        u32                   seat{0};
        f32                   width{640.f};
        std::vector<sprite_t> sprites;
        std::vector<text_t>   texts;
        /* Over the whole view, under its HUD; premultiplied */
        Vecf4 tint{0.f};
    };

    std::vector<view_t> views;

    /* Over every view: a cutscene's fade, and its letterbox bars as a
     * fraction of the height each */
    Vecf4 fade{0.f};
    f32   letterbox{0.f};

    /* The map load it was built for; its tag pointers are from that map */
    u32 generation{0};
};

/* Unit, weapon and grenade HUD interfaces for each local seat */
void alloc_hud(compo::EntityContainer& e);
