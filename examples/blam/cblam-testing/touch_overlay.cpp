#include "touch_overlay.h"

#include <glm/gtx/matrix_transform_2d.hpp>

#include <coffee/core/CDebug>

using namespace Coffee::resource_literals;

namespace {

bool point_in(glm::vec2 const& pos, glm::mat2 const& box)
{
    return pos.x > box[0].x && pos.y > box[0].y && pos.x < box[1].x &&
           pos.y < box[1].y;
}

constexpr f32 look_sensitivity = 2.f;

} // namespace

void TouchOverlay::start_restricted(Proxy& proxy, const time_point&)
{
    if(!controller)
    {
        gfx::system* gfx;
        proxy.subsystem(gfx);

        if(!gfx->feature_info().texture.tex.gl.etc2)
            return;

        auto tex =
            ktx::load_from("textures/controller_atlas.0.etc2"_rsc.data());

        if(tex.has_error())
        {
            cFatal("Failed to load texture: {0}", tex.error());
            Throw(std::runtime_error("touch overlay failed to load"));
        }

        controller = gfx->alloc_texture(
            gfx::textures::d2,
            typing::pixels::CompFmt(
                comp_app::pix_fmt::ETC2, typing::pixels::pix_flags::RGBA),
            1);

        atlas_storage = std::move(tex.value());

        controller->alloc(
            size_3d<u32>{
                atlas_storage.mips.at(0).size.x,
                atlas_storage.mips.at(0).size.y,
                1});
        controller->upload(
            atlas_storage.mips.at(0).data,
            Veci2{},
            Veci2(atlas_storage.mips.at(0).size));

        controller_sampler = controller->sampler();
        controller_sampler->alloc();
        controller_sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear);
    }

    comp_app::interfaces::GraphicsFramebuffer* framebuffer =
        proxy.service<comp_app::GraphicsFramebuffer>();
    f32 half_screen   = controller_size(proxy);
    f32 screen_height = framebuffer->size().h;
    f32 look_stick_x  = framebuffer->size().w - half_screen;

    control_scale = {1.f / half_screen, -1.f / half_screen};

    movement.box[0] = glm::vec2{
        0,
        screen_height - controller_size(proxy),
    };
    movement.box[1] = glm::vec2{controller_size(proxy), screen_height};
    movement.size   = {half_screen / 3.f, half_screen / 3.f};
    look.box[0]     = glm::vec2{
        framebuffer->size().w / 2.f,
        0,
    };
    look.box[1] = glm::vec2{framebuffer->size().w, screen_height};

    dpi = proxy.service<comp_app::DisplayInfo>()->dpi();

    f32 stick_range = half_screen / 2.f;
    // Movement stick
    if(movement.visible)
        draw_stick(proxy, movement);

    // Look stick
    // draw_stick(proxy, Vecf2{look_stick_x, 0}, look_ui_offset);
}

void TouchOverlay::end_restricted(Proxy& proxy, const time_point& time)
{
    using namespace std::chrono_literals;

    auto delta    = time - previous_time;
    previous_time = time;

    if(delta > 1s)
        delta = 10ms;
    auto delta_s = stl_types::Chrono::to_f32(delta);

    /* Find seat_idx==0 PlayerCamera */
    for(auto entity : proxy.select<PlayerCamera>())
    {
        auto* info = proxy.get<PlayerInfo>(entity.id());
        auto* cam  = proxy.get<PlayerCamera>(entity.id());
        if(!info || !cam || info->seat_idx != 0)
            continue;
        cam->camera_->move(
            movement.control.y * delta_s, movement.control.x * delta_s, 0);
        cam->camera_->rotate(
            -look.control.x * delta_s, look.control.y * delta_s);
        cam->camera_->rotate(-look.instant_control.x, look.instant_control.y);
        break;
    }
    look.instant_control = {};
}

void TouchOverlay::draw_stick(Proxy& proxy, stick_definition_t const& stick)
{
    ScreenClear* screen;
    proxy.subsystem(screen);

    auto screen_size_ = proxy.service<comp_app::GraphicsFramebuffer>()->size();
    f32  half_screen  = controller_size(proxy);
    f32  stick_size   = half_screen / 3.f;

    const auto flip_y = [height = screen_size_.h](const auto& vec) {
        return Vecf2{vec.x, height - vec.y};
    };

    screen->extra_quads.push_back({
        .position     = flip_y(stick.origin),
        .size         = Vecf2{half_screen, half_screen},
        .atlas_offset = Vecf2{0.02f, 0.02f},
        .atlas_scale  = Vecf2{0.5f, 0.5f},
        .sampler      = controller_sampler,
    });
    screen->extra_quads.push_back({
        .position     = flip_y(stick.offset),
        .size         = Vecf2{stick_size, stick_size},
        .atlas_offset = Vecf2{0.55f, 0.03f},
        .atlas_scale  = Vecf2{0.23f, 0.23f},
        .sampler      = controller_sampler,
    });
}

f32 TouchOverlay::controller_size(Proxy& proxy) const
{
    comp_app::interfaces::GraphicsFramebuffer* framebuffer =
        proxy.service<comp_app::GraphicsFramebuffer>();
    return glm::min(framebuffer->size().h / 2.f, framebuffer->size().w / 2.f);
}

void TouchOverlay::operator()(CIEvent& ev, CIMouseMoveEvent* event)
{
    // if(event->btn == 0)
    // {
    //     move_displacement = {};
    //     look_displacement = {};
    //     return;
    // }

    // glm::vec2 pos = event->origin + event->delta;
    // if(auto xf = point_in(pos, move_transform); xf.has_value())
    // {
    //     ev.type        = CIEvent::NoneType;
    //     move_ui_offset = move_displacement = xf.value() * 2.f - 1.f;
    //     return;
    // }
    // if(auto xf = point_in(pos, look_transform); xf.has_value())
    // {
    //     ev.type        = CIEvent::NoneType;
    //     look_ui_offset = look_displacement = xf.value() * 2.f - 1.f;
    // }
}

void TouchOverlay::operator()(CIEvent& ev, CITouchMotionEvent* event)
{
    if(event->hover)
        return;

    if(point_in(event->origin, movement.box))
    {
        ev.type = CIEvent::NoneType;
        movement.origin =
            event->origin - Vecf2{movement.size.x * 1.5f} * Vecf2{1, -1};
        movement.offset = event->origin + event->delta() -
                          Vecf2{movement.size.x / 2.f} * Vecf2{1, -1};
        movement.control =
            event->end ? Vecf2{} : event->delta() * control_scale * dpi * 3.f;
        movement.visible = !event->end;
    }
    if(!event->end && point_in(event->origin, look.box))
        look.instant_control +=
            event->frame_delta() * control_scale * look_sensitivity * dpi;
}

void TouchOverlay::operator()(CIEvent& ev, CITouchPinchEvent* event)
{
    movement.control.y = (event->factor - 1.f) * dpi;
}

void create_touch_overlay(compo::EntityContainer& container)
{
    using namespace Coffee::Input;

    container.register_subsystem_inplace<TouchOverlay>();

    auto eventhandler = container.service<comp_app::BasicEventBus<CIEvent>>();

    auto& overlay = container.subsystem_cast<TouchOverlay>();
    eventhandler->addEventFunction<CIMouseMoveEvent>(
        768, [&overlay](CIEvent& ev, CIMouseMoveEvent* event) {
            overlay(ev, event);
        });
    eventhandler->addEventFunction<CITouchMotionEvent>(
        768, [&overlay](CIEvent& ev, CITouchMotionEvent* event) {
            overlay(ev, event);
        });
    eventhandler->addEventFunction<CITouchPinchEvent>(
        768, [&overlay](CIEvent& ev, CITouchPinchEvent* event) {
            overlay(ev, event);
        });
}
