#pragma once

#include <coffee/components/restricted_subsystem.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/image/ktx_load.h>

#include "render/rendering.h"

using TouchOverlayManifest = compo::SubsystemManifest<
    compo::type_list_t<const PlayerInfo, const PlayerInput>,
    compo::type_list_t<gfx::system, ScreenClear>,
    compo::type_list_t<
        comp_app::GraphicsFramebuffer,
        comp_app::ControllerInput>>;

/* On-screen sticks and buttons, presented as pad 0 of comp_app::ControllerInput
 * merged with whatever pad the platform has there */
struct TouchOverlay
    : compo::RestrictedSubsystem<TouchOverlay, TouchOverlayManifest>
    , comp_app::interfaces::ControllerInput
{
    using type        = TouchOverlay;
    using Proxy       = compo::proxy_of<TouchOverlayManifest>;
    using pulse_clock = std::chrono::steady_clock;

    enum class role_t
    {
        none,
        move,
        look,
        button,
    };

    struct button_t
    {
        enum class kind_t
        {
            button,
            trigger_left,
            trigger_right,
        };

        kind_t                                   kind;
        Coffee::Input::CIControllerButtonMapping mapping;
        /* Shares the look stick's area: ABXY press on a tap, triggers hold
         * while a drag started on them looks around */
        bool  overlaps_look;
        Vecf4 atlas; /* x, y, w, h in atlas pixels */

        Vecf2                   center{}; /* Screen pixels, y down */
        Vecf2                   size{};
        u32                     held{0};
        pulse_clock::time_point pulse_until{};
    };

    struct finger_t
    {
        role_t                role{role_t::none};
        Vecf2                 origin{};
        Vecf2                 current{};
        std::optional<size_t> button;
    };

    TouchOverlay();

    void start_restricted(Proxy& proxy, time_point const&);

    void layout(Vecf2 const& screen);
    void draw(ScreenClear& screen, f32 height) const;

    void operator()(CIEvent& ev, CITouchMotionEvent* event);
    void operator()(CIEvent& ev, CITouchTapEvent* event);

    libc_types::u32       count() const override;
    controller_map        state(libc_types::u32 idx) const override;
    comp_app::text_type_t name(libc_types::u32 idx) const override;

    controller_map touch_state() const;

    comp_app::interfaces::ControllerInput* platform{nullptr};

    std::shared_ptr<gfx::texture_2d_t> controller;
    std::shared_ptr<gfx::sampler_t>    controller_sampler;
    ktx::texture_t                     atlas_storage;

    std::vector<button_t>   buttons;
    std::map<u16, finger_t> fingers;
    Vecf2                   screen_size{};
    f32                     stick_radius{1.f};
    bool                    active{false};
};

void create_touch_overlay(compo::EntityContainer& container);
