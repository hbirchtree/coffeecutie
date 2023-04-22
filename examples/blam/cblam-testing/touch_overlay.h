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

    void start_restricted(Proxy& proxy, time_point const&);
    void end_restricted(Proxy& proxy, time_point const&);

    void draw_stick(
        Proxy& proxy, Vecf2 const& origin, Vecf2 const& stick_offset);

    f32 controller_size(Proxy& proxy) const;

    std::optional<glm::vec2> point_in(
        glm::vec2 const& pos, glm::mat2 const& box);

    void operator()(CIEvent& ev, CIMouseMoveEvent* event);
    void operator()(CIEvent& ev, CITouchMotionEvent* event);

    std::shared_ptr<gfx::texture_2d_t> controller;
    std::shared_ptr<gfx::sampler_t>    controller_sampler;

    glm::mat2 move_transform;
    glm::vec2 move_displacement;
    glm::mat2 look_transform;
    glm::vec2 look_displacement;

    ktx::texture_t atlas_storage;
    bool           loaded{false};
};

void create_touch_overlay(compo::EntityContainer& container);
