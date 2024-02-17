#include "touch_overlay.h"

#include <glm/gtx/matrix_transform_2d.hpp>

#include <coffee/core/CDebug>

using namespace Coffee::resource_literals;

void TouchOverlay::start_restricted(Proxy& proxy, const time_point&)
{
    if(!controller)
    {
        gfx::system* gfx;
        proxy.subsystem(gfx);

        auto tex = ktx::load_from("controller_atlas.0.etc2"_rsc.data());

        if(tex.has_error())
        {
            cFatal("Failed to load texture: {0}", tex.error());
            Throw(std::runtime_error("touch overlay failed to load"));
        }

        controller = gfx->alloc_texture(
            gfx::textures::d2,
            typing::pixels::CompFmt(
                comp_app::PixFmt::ETC2, typing::pixels::PixFlg::RGBA),
            1);

        atlas_storage = std::move(tex.value());

        controller->alloc(size_3d<u32>{
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

    move_transform[0] = glm::vec2{0, -screen_height};
    move_transform[1] = glm::vec2{1.f / half_screen, -1.f / half_screen};

    look_transform[0] = glm::vec2{-look_stick_x, -screen_height};
    look_transform[1] = glm::vec2{1.f / half_screen, -1.f / half_screen};

    f32 stick_range = half_screen / 2.f;
    // Movement stick
    draw_stick(proxy, Vecf2{0, 0}, move_displacement * stick_range);

    // Look stick
    draw_stick(proxy, Vecf2{look_stick_x, 0}, look_displacement * stick_range);
}

void TouchOverlay::end_restricted(Proxy& proxy, const time_point&)
{
    BlamCamera* camera;
    proxy.subsystem(camera);

    camera->player(0).camera_->move(
        move_displacement.y, -move_displacement.x, 0);
    //    camera->std_camera->rotate(look_displacement.y, look_displacement.x);
}

void TouchOverlay::draw_stick(
    Proxy& proxy, Vecf2 const& origin, Vecf2 const& stick_offset)
{
    ScreenClear* screen;
    proxy.subsystem(screen);

    f32 half_screen = controller_size(proxy);
    f32 stick_size  = half_screen / 3.f;

    screen->extra_quads.push_back({
        .position     = origin,
        .size         = Vecf2{half_screen, half_screen},
        .atlas_offset = Vecf2{0.02f, 0.02f},
        .atlas_scale  = Vecf2{0.5f, 0.5f},
        .sampler      = controller_sampler,
    });
    screen->extra_quads.push_back({
        .position =
            Vecf2{
                half_screen / 2.f - stick_size / 2.f,
                half_screen / 2.f - stick_size / 2.f,
            } +
            origin + stick_offset,
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

std::optional<glm::vec2> TouchOverlay::point_in(
    glm::vec2 const& pos, glm::mat2 const& box)
{
    glm::vec2 xf = (pos + box[0]) * box[1];
    return (xf.x > 0 && xf.x < 1 && xf.y > 0 && xf.y < 1)
               ? std::make_optional(xf)
               : std::nullopt;
}

void TouchOverlay::operator()(CIEvent& ev, CIMouseMoveEvent* event)
{
    if(event->btn == 0)
    {
        move_displacement = {};
        look_displacement = {};
        return;
    }

    glm::vec2 pos = event->origin + event->delta;
    if(auto xf = point_in(pos, move_transform); xf.has_value())
    {
        ev.type           = CIEvent::NoneType;
        move_displacement = xf.value() * 2.f - 1.f;
        cDebug("Move displacement: {0}", move_displacement);
        return;
    }
    if(auto xf = point_in(pos, look_transform); xf.has_value())
    {
        //        ev.type           = CIEvent::NoneType;
        look_displacement = xf.value() * 2.f - 1.f;
        //        cDebug("Look displacement: {0}", look_displacement);
        //        return;
    }
}

void TouchOverlay::operator()(CIEvent& /*ev*/, CITouchMotionEvent* /*event*/)
{
    //    comp_app::interfaces::GraphicsFramebuffer* framebuffer
    //        = proxy.service<comp_app::GraphicsFramebuffer>();
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
}
