#include "touch_overlay.h"

#include <coffee/core/CDebug>

using namespace Coffee::resource_literals;
using namespace Coffee::Input;

namespace {

using button_t = TouchOverlay::button_t;
using kind_t   = button_t::kind_t;

constexpr f32 atlas_size  = 1024.f;
Vecf4 const   ring_sprite = {20, 20, 512, 512};
Vecf4 const   knob_sprite = {567.6f, 35.6f, 224.8f, 224.8f};
/* How long a tap holds its button, long enough for a tick to see it */
constexpr auto tap_pulse = std::chrono::milliseconds(100);

std::vector<button_t> default_buttons()
{
    auto button = [](CIControllerButtonMapping mapping, Vecf4 atlas, bool overlaps = false) {
        return button_t{
            .kind = kind_t::button, .mapping = mapping, .overlaps_look = overlaps, .atlas = atlas};
    };
    auto trigger = [](kind_t kind, Vecf4 atlas) {
        return button_t{.kind = kind, .mapping = CK_BUTTON_BASE, .overlaps_look = true, .atlas = atlas};
    };
    return {
        button(CK_BUTTON_A, {0, 560, 192, 192}, true),
        button(CK_BUTTON_B, {200, 560, 192, 192}, true),
        button(CK_BUTTON_X, {400, 560, 192, 192}, true),
        button(CK_BUTTON_Y, {600, 560, 192, 192}, true),
        trigger(kind_t::trigger_left, {560, 300, 192, 240}),
        trigger(kind_t::trigger_right, {760, 300, 192, 240}),
        button(CK_BUTTON_START, {800, 560, 192, 192}),
        button(CK_BUTTON_LB, {0, 760, 192, 160}),
        button(CK_BUTTON_RB, {200, 760, 192, 160}),
        button(CK_BUTTON_POV_LEFT, {400, 760, 128, 128}),
        button(CK_BUTTON_POV_RIGHT, {533.3f, 760, 128, 128}),
        button(CK_BUTTON_POV_UP, {666.7f, 760, 128, 128}),
        button(CK_BUTTON_POV_DOWN, {803.2f, 760, 128, 128}),
        button(CK_BUTTON_LS, {400, 890, 128, 128}),
        button(CK_BUTTON_RS, {534, 890, 128, 128}),
    };
}

bool hit(button_t const& button, Vecf2 const& pos)
{
    Vecf2 const d = (pos - button.center) / (button.size * .5f);
    return glm::dot(d, d) <= 1.f;
}

u16 button_bit(button_t const& button)
{
    return static_cast<u16>(1u << (button.mapping - CK_BUTTON_BASE));
}

i16 to_axis(f32 v)
{
    return static_cast<i16>(std::clamp(v, -1.f, 1.f) * 32767.f);
}

Vecf2 stick_value(TouchOverlay::finger_t const& finger, f32 radius)
{
    Vecf2 d = (finger.current - finger.origin) / radius;
    if(f32 len = glm::length(d); len > 1.f)
        d /= len;
    return d;
}

/* Larger deflection wins, so either device can steer */
i16 merge_axis(i16 a, i16 b)
{
    return std::abs(b) > std::abs(a) ? b : a;
}

} // namespace

TouchOverlay::TouchOverlay()
    : buttons(default_buttons())
{
}

void TouchOverlay::start_restricted(Proxy& proxy, const time_point&)
{
    active = false;
    for(auto player : proxy.select<PlayerInfo, PlayerInput>())
    {
        auto [info, input] = player.components();
        if(info.seat_idx == 0)
            active = input.input_mode == PlayerInput::input_mode_t::game;
    }
    if(!active)
    {
        fingers.clear();
        for(auto& button : buttons)
            button.held = 0;
    }

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

    auto const size = proxy.service<comp_app::GraphicsFramebuffer>()->size();
    layout(Vecf2(size.w, size.h));

    if(active)
    {
        ScreenClear* screen;
        proxy.subsystem(screen);
        draw(*screen, screen_size.y);
    }
}

void TouchOverlay::layout(Vecf2 const& screen)
{
    screen_size  = screen;
    f32 const u  = glm::min(screen.x, screen.y) / 2.f;
    f32 const s  = u * .3f; /* Face button diameter */
    stick_radius = u * .4f;

    Vecf2 const face = {screen.x - 1.9f * s, screen.y - 1.9f * s};
    Vecf2 const dpad = {1.6f * s, 1.6f * s};

    auto place = [this, s](CIControllerButtonMapping mapping, Vecf2 center, f32 scale) {
        for(auto& button : buttons)
            if(button.kind == kind_t::button && button.mapping == mapping)
            {
                button.center = center;
                button.size   = Vecf2(button.atlas.z, button.atlas.w) / 192.f * s * scale;
            }
    };
    place(CK_BUTTON_A, face + Vecf2{0, 1.15f * s}, 1.f);
    place(CK_BUTTON_B, face + Vecf2{1.15f * s, 0}, 1.f);
    place(CK_BUTTON_X, face + Vecf2{-1.15f * s, 0}, 1.f);
    place(CK_BUTTON_Y, face + Vecf2{0, -1.15f * s}, 1.f);
    place(CK_BUTTON_RB, {screen.x - .7f * s, .55f * s}, 1.f);
    place(CK_BUTTON_LB, {screen.x - 1.8f * s, .55f * s}, 1.f);
    place(CK_BUTTON_START, {screen.x * .5f, .6f * s}, .8f);
    place(CK_BUTTON_POV_UP, dpad + Vecf2{0, -.75f * s}, 1.f);
    place(CK_BUTTON_POV_DOWN, dpad + Vecf2{0, .75f * s}, 1.f);
    place(CK_BUTTON_POV_LEFT, dpad + Vecf2{-.75f * s, 0}, 1.f);
    place(CK_BUTTON_POV_RIGHT, dpad + Vecf2{.75f * s, 0}, 1.f);
    place(CK_BUTTON_LS, {screen.x * .5f - .7f * s, screen.y - .7f * s}, 1.f);
    place(CK_BUTTON_RS, {screen.x * .5f + .7f * s, screen.y - .7f * s}, 1.f);

    for(auto& button : buttons)
    {
        if(button.kind == kind_t::button)
            continue;
        bool const right = button.kind == kind_t::trigger_right;
        button.size      = Vecf2(button.atlas.z, button.atlas.w) / 192.f * s;
        button.center    = {
            screen.x - (right ? .7f : 1.9f) * s,
            face.y - (right ? 2.f : 2.2f) * s - button.size.y * .5f};
    }
}

void TouchOverlay::draw(ScreenClear& screen, f32 height) const
{
    auto quad = [&](Vecf2 center, Vecf2 size, Vecf4 sprite, f32 alpha) {
        screen.extra_quads.push_back({
            .position     = Vecf2{center.x - size.x * .5f, height - center.y - size.y * .5f},
            .size         = size,
            /* Quads sample bottom-up, the atlas is laid out top-down */
            .atlas_offset = Vecf2(sprite.x, sprite.y + sprite.w) / atlas_size,
            .atlas_scale  = Vecf2(sprite.z, -sprite.w) / atlas_size,
            .tint         = Vecf4{1, 1, 1, alpha},
            .sampler      = controller_sampler,
        });
    };

    auto const now = pulse_clock::now();
    for(auto const& button : buttons)
        quad(
            button.center,
            button.size,
            button.atlas,
            button.held || now < button.pulse_until ? 1.f : .5f);

    for(auto const& [id, finger] : fingers)
    {
        if(finger.role != role_t::move && finger.role != role_t::look)
            continue;
        Vecf2 const knob = finger.origin + stick_value(finger, stick_radius) * stick_radius;
        quad(finger.origin, Vecf2(stick_radius * 2.5f), ring_sprite, .6f);
        quad(knob, Vecf2(stick_radius * .8f), knob_sprite, .8f);
    }
}

void TouchOverlay::operator()(CIEvent& ev, CITouchMotionEvent* event)
{
    if(!active || event->hover)
        return;

    auto [it, down] = fingers.try_emplace(event->finger);
    auto& finger    = it->second;
    finger.current  = event->current;

    if(down)
    {
        finger.origin = event->origin;
        auto taken    = [this](role_t role) {
            return std::any_of(fingers.begin(), fingers.end(), [role](auto const& f) {
                return f.second.role == role;
            });
        };
        auto const button = std::find_if(buttons.begin(), buttons.end(), [&](button_t const& b) {
            return hit(b, event->origin);
        });
        bool const look_side = event->origin.x >= screen_size.x * .5f;
        if(button != buttons.end())
        {
            finger.button = static_cast<size_t>(button - buttons.begin());
            /* Face buttons press on their tap, so only triggers hold here */
            if(button->kind != kind_t::button || !button->overlaps_look)
                ++button->held;
            finger.role = button->overlaps_look && !taken(role_t::look) ? role_t::look : role_t::button;
        } else if(look_side && !taken(role_t::look))
            finger.role = role_t::look;
        else if(!look_side && !taken(role_t::move))
            finger.role = role_t::move;
    }
    if(finger.role != role_t::none)
        ev.type = CIEvent::NoneType;

    if(event->end)
    {
        if(finger.button)
        {
            auto& button = buttons[*finger.button];
            if(button.held)
                --button.held;
        }
        fingers.erase(it);
    }
}

void TouchOverlay::operator()(CIEvent& ev, CITouchTapEvent* event)
{
    if(!active || !event->pressed)
        return;
    for(auto& button : buttons)
        if(button.kind == kind_t::button && button.overlaps_look && hit(button, event->pos))
        {
            button.pulse_until = pulse_clock::now() + tap_pulse;
            ev.type            = CIEvent::NoneType;
            return;
        }
}

TouchOverlay::controller_map TouchOverlay::touch_state() const
{
    controller_map out{};
    if(!active)
        return out;

    auto const now = pulse_clock::now();
    for(auto const& button : buttons)
    {
        bool const pressed = button.held || now < button.pulse_until;
        if(!pressed)
            continue;
        switch(button.kind)
        {
        case kind_t::button:
            out.buttons.d |= button_bit(button);
            break;
        case kind_t::trigger_left:
            out.axes.e.t_l = 32767;
            break;
        case kind_t::trigger_right:
            out.axes.e.t_r = 32767;
            break;
        }
    }
    for(auto const& [id, finger] : fingers)
    {
        Vecf2 const v = stick_value(finger, stick_radius);
        if(finger.role == role_t::move)
        {
            out.axes.e.l_x = to_axis(v.x);
            out.axes.e.l_y = to_axis(v.y);
        } else if(finger.role == role_t::look)
        {
            out.axes.e.r_x = to_axis(v.x);
            out.axes.e.r_y = to_axis(v.y);
        }
    }
    return out;
}

libc_types::u32 TouchOverlay::count() const
{
    return std::max(platform ? platform->count() : 0u, 1u);
}

TouchOverlay::controller_map TouchOverlay::state(libc_types::u32 idx) const
{
    controller_map out = platform ? platform->state(idx) : controller_map{};
    if(idx != 0)
        return out;
    auto const touch = touch_state();
    out.buttons.d |= touch.buttons.d;
    for(size_t i = 0; i < std::size(out.axes.d); ++i)
        out.axes.d[i] = merge_axis(out.axes.d[i], touch.axes.d[i]);
    return out;
}

comp_app::text_type_t TouchOverlay::name(libc_types::u32 idx) const
{
    if(platform && idx < platform->count())
        return platform->name(idx);
    return "Touchscreen";
}

void create_touch_overlay(compo::EntityContainer& container)
{
    auto& overlay = container.register_subsystem_inplace<TouchOverlay>();
    overlay.platform =
        container.replace_service<comp_app::ControllerInput>(&overlay);

    auto eventhandler = container.service<comp_app::BasicEventBus<CIEvent>>();
    eventhandler->addEventFunction<CITouchMotionEvent>(
        768, [&overlay](CIEvent& ev, CITouchMotionEvent* event) {
            overlay(ev, event);
        });
    eventhandler->addEventFunction<CITouchTapEvent>(
        768, [&overlay](CIEvent& ev, CITouchTapEvent* event) {
            overlay(ev, event);
        });
}
