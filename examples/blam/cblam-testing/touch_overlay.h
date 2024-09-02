#pragma once

#include <coffee/components/restricted_subsystem.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/image/ktx_load.h>

#include "rendering.h"

using TouchOverlayManifest = compo::SubsystemManifest<
    compo::empty_list_t,
    compo::type_list_t<gfx::system, ScreenClear, BlamCamera>,
    compo::type_list_t<
        comp_app::DisplayInfo,
        comp_app::GraphicsFramebuffer,
        comp_app::ControllerConfig>>;

struct TouchOverlay
    : compo::RestrictedSubsystem<TouchOverlay, TouchOverlayManifest>
{
    using type  = TouchOverlay;
    using Proxy = compo::proxy_of<TouchOverlayManifest>;

    struct stick_definition_t
    {
        glm::mat2 box;
        glm::vec2 size;
        glm::vec2 origin;
        glm::vec2 offset;

        glm::vec2 control;
        glm::vec2 instant_control;

        bool visible{false};
    };

    void start_restricted(Proxy& proxy, time_point const&);
    void end_restricted(Proxy& proxy, time_point const&time);

    void draw_stick(Proxy& proxy, stick_definition_t const& stick);

    f32 controller_size(Proxy& proxy) const;

    void operator()(CIEvent& ev, CIMouseMoveEvent* event);
    void operator()(CIEvent& ev, CITouchMotionEvent* event);
    void operator()(CIEvent& ev, CITouchPinchEvent* event);

    std::shared_ptr<gfx::texture_2d_t> controller;
    std::shared_ptr<gfx::sampler_t>    controller_sampler;

    stick_definition_t movement;
    stick_definition_t look;
    Vecf2 control_scale{1.f, 1.f};

    // glm::mat2 move_transform;
    // glm::vec2 move_displacement;
    // glm::vec2 move_ui_origin;
    // glm::vec2 move_ui_offset;
    // glm::mat2 look_transform;
    // glm::vec2 look_displacement;
    // glm::vec2 look_immediate_displacement;
    // glm::vec2 look_ui_origin;
    // glm::vec2 look_ui_offset;

    time_point previous_time{};
    f32 dpi{1.f};

    ktx::texture_t atlas_storage;
    bool           loaded{false};
};

void create_touch_overlay(compo::EntityContainer& container);
