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

    /* Help text a script shows on every local HUD: a message of the
     * scenario's hud messages */
    struct help_t
    {
        std::string message;
        bool        shown{false};
        bool        flash{false};
        f32         elapsed{0.f}; /* since it was set */
    } help;

    /* hud_set_objective_text: a message of the scenario's hud messages,
     * shown in the pause menu */
    std::string objective;

    /* hud_show_*: parts of the unit HUD scripts hide */
    struct hud_parts_t
    {
        bool health{true};
        bool shield{true};
        bool motion_sensor{true};
        bool crosshair{true};
    } hud_parts;
    /*! hud_blink_*: panels flash in their backgrounds' flashing colours */
    struct hud_blink_t
    {
        bool health{false};
        bool shield{false};
        bool motion_sensor{false};
    } hud_blink;

    /* The HUD countdown: hud_set_timer_time and friends */
    struct timer_t
    {
        libc_types::i64 ticks{0};   /*!< left */
        libc_types::i64 warning{0}; /*!< ticks left below which it warns */
        libc_types::u16 corner{0};  /*!< hud_corner: top_left .. bottom_right */
        libc_types::i16 x{0}, y{0};
        bool            shown{false};
        bool            paused{false};
    } timer;

    bool letterbox{false};
    /*! Between cinematic_start and cinematic_stop: split screen gives way to
     *  one view of the whole window */
    bool playing{false};
    bool show_hud{true};
    bool input{true};
    bool camera_control{true}; /*!< player_camera_control: players may look around */
    bool invert_pitch{false};  /*!< player0_look_invert_pitch: player 0 looks up by pushing down */
    libc_types::u32 unzooms{0}; /*!< players_unzoom_all calls: each takes every zoom back out */
};
