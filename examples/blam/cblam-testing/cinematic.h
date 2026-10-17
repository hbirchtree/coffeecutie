#pragma once

#include <coffee/components/subsystem.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <optional>
#include <string>

/* What scripts ask of the screen during cutscenes: a fade, letterbox bars,
 * the HUD and the players' control. Drawn by the HUD, read by input. */
struct CinematicState : compo::SubsystemBase
{
    using type = CinematicState;

    using Vecf3 = typing::vector_types::Vecf3;
    using Vecf4 = typing::vector_types::Vecf4;
    using f32   = libc_types::f32;

    /* fade_out goes to the color, fade_in from it */
    struct fade_t
    {
        Vecf3 color{0.f};
        f32   from{0.f}; /* opacity */
        f32   to{0.f};
        f32   seconds{0.f};
        f32   elapsed{0.f};

        f32 opacity() const
        {
            f32 const s = seconds > 0.f ? std::min(elapsed / seconds, 1.f) : 1.f;
            return from + (to - from) * s;
        }
    } fade;

    /* cinematic_set_title: a line of text fading in, held, fading out */
    struct title_t
    {
        std::u16string text;
        Vecf4          bounds{0.f}; /* 640x480: top, left, bottom, right */
        f32            align{0.f};  /* 0 left, .5 centred, 1 right */
        Vecf4          color{1.f};
        f32            fade_in{0.f}, hold{0.f}, fade_out{0.f}; /* seconds */
        f32            elapsed{0.f};

        f32 opacity() const
        {
            if(elapsed < fade_in)
                return fade_in > 0.f ? elapsed / fade_in : 1.f;
            f32 const out = elapsed - fade_in - hold;
            if(out <= 0.f)
                return 1.f;
            return fade_out > 0.f ? std::max(1.f - out / fade_out, 0.f) : 0.f;
        }
    };
    std::optional<title_t> title;

    bool letterbox{false};
    bool show_hud{true};
    bool input{true};
};
