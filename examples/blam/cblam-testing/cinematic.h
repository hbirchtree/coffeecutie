#pragma once

#include <coffee/components/subsystem.h>
#include <peripherals/typing/vectors/vector_types.h>

/* What scripts ask of the screen during cutscenes: a fade, letterbox bars,
 * the HUD and the players' control. Drawn by the HUD, read by input. */
struct CinematicState : compo::SubsystemBase
{
    using type = CinematicState;

    using Vecf3 = typing::vector_types::Vecf3;
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

    bool letterbox{false};
    bool show_hud{true};
    bool input{true};
};
