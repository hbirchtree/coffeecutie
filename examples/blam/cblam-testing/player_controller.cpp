#include "player_controller.h"

#include <coffee/comp_app/app_events.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/Scene>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <coffee/core/types/input/keymap_latin1.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <map>
#include <utility>

using namespace Coffee;
using namespace std::chrono_literals;

struct PlayerController
    : compo::RestrictedSubsystem<PlayerController, PlayerControllerManifest>
{
    using type  = PlayerController;
    using Proxy = compo::proxy_of<PlayerControllerManifest>;

    /* wu/s on the ground; about a Halo biped's run */
    static constexpr f32 run_speed = 2.25f;
    /* As high as 4 wu/s jumped under 9.81, in Halo's gravity */
    static constexpr f32 jump_speed = 2.3f;
    /* A press this long before landing still jumps on landing */
    static constexpr duration jump_buffer = 150ms;
    /* A press this long after stepping off an edge still jumps */
    static constexpr duration coyote_time = 100ms;
    /* Re-arms a jump the ground probe never saw leave the ground */
    static constexpr duration jump_cooldown   = 300ms;
    static constexpr i16      trigger_pressed = 8192;

    /* What a seat's movement goes to */
    enum class seat_t
    {
        freecam,
        body,
        vehicle,
    };

    struct jump_t
    {
        time_point buffered_until{};
        time_point last_grounded{};
        time_point jumped_at{};
        bool       pending{false}; /*!< intent.jump seen, deadline set */
        bool airborne_seen{true};  /*!< Off the ground since the last jump */
    };

    struct menu_pad_t
    {
        debounced_button_t accept, back, option, option_2;
        debounced_button_t up, down, left, right;
    };

    PlayerController()
    {
        /* After event pumps, before UI and physics */
        this->priority = 800;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        duration const dt          = frame_delta(t);
        auto*          controllers = p.service<comp_app::ControllerInput>();
        auto&          ui          = p.subsystem<UIEventBus>();
        auto&          physics     = p.subsystem<PhysicsBus>();
        auto&          render      = p.subsystem<RenderingParameters>();

        for(auto player : p.select<
                          PlayerCamera,
                          PlayerInput,
                          PlayerInfo,
                          NetworkInfo,
                          Model>())
        {
            auto [cam, input, info, net, model] = player.components();
            if(info.permissions.camera)
            {
                sample(input, cam, controllers, dt);
                route(
                    p,
                    player.id(),
                    input,
                    cam,
                    info,
                    net,
                    ui,
                    physics,
                    render,
                    t);
                input.frame_end();
            }
            update_camera(cam);
            if(info.riding.vehicle == 0)
                place_biped(cam, info, model);
        }
        menu_pads(p, controllers, ui, render);
    }

    duration frame_delta(time_point const& t)
    {
        auto dt = std::chrono::duration_cast<duration>(
            t - std::exchange(m_last_time, t));
        /* The first frame, and a stall: one frame's worth, not the gap */
        if(dt <= duration::zero() || dt > 1s)
            dt = 16ms;
        return dt;
    }

    /* Local devices into the seat's input. Buttons are read in every mode
     * so a held one doesn't re-press after a mode change */
    static void sample(
        PlayerInput&                           input,
        PlayerCamera const&                    cam,
        comp_app::interfaces::ControllerInput* controllers,
        duration const&                        dt)
    {
        using namespace Coffee::Input;
        bool const pad      = controllers && cam.controller.index.has_value();
        bool const keyboard = cam.keyboard.enabled;
        bool const in_menu =
            input.input_mode == PlayerInput::input_mode_t::menu;
        CIControllerState const state =
            pad ? controllers->state(*cam.controller.index)
                : CIControllerState{};
        auto const& b   = state.buttons.e;
        auto        key = [&input](u16 k) {
            return StandardCamera::has_key(input.keys, k);
        };
        auto axis = [&cam](i16 raw) {
            return std::abs(raw) > cam.controller.opts.deadzone
                       ? convert_i16_f(raw)
                       : 0.f;
        };

        if(pad)
            controller_sample_input(
                input.look_delta,
                input.movement,
                input.accel,
                cam.controller.opts,
                state,
                dt);
        if(keyboard)
            StandardCamera::sample_keys(
                input.keys, cam.camera_opts, input.movement, input.accel, dt);

        if(pad)
        {
            input.start |= b.start;
            input.accept |= b.a;
            input.option |= b.y;
            input.option_2 |= b.x;
            input.up |= b.p_up;
            input.down |= b.p_down;
            input.left |= b.p_left;
            input.right |= b.p_right;
            input.jump |= b.a;
            /* B backs out of menus only; back also toggles freecam */
            input.back |= b.back || (in_menu && b.b);
        }
        if(keyboard)
        {
            input.start |= key(CK_F2);
            input.accept |= key(CK_EnterCR);
            input.up |= key(CK_Up);
            input.down |= key(CK_Down);
            input.left |= key(CK_Left);
            input.right |= key(CK_Right);
            input.jump |= key(CK_Space);
            input.back |= in_menu && key(CK_BackSpace);
        }

        auto& intent = input.intent;
        if(input.jump)
            intent.jump = true;
        if(in_menu)
        {
            input.movement  = {};
            intent.move     = {};
            intent.throttle = intent.steer = 0.f;
            intent.use = intent.grab = intent.jump = false;
            return;
        }

        /* Forward/right from left stick and WASD */
        Vecf2 move{};
        if(pad)
            move += Vecf2{-axis(state.axes.e.l_y), axis(state.axes.e.l_x)};
        if(keyboard)
            move += Vecf2{
                (key(CK_w) ? 1.f : 0.f) - (key(CK_s) ? 1.f : 0.f),
                (key(CK_d) ? 1.f : 0.f) - (key(CK_a) ? 1.f : 0.f)};
        intent.throttle = std::clamp(move.x, -1.f, 1.f);
        intent.steer    = std::clamp(move.y, -1.f, 1.f);
        /* Clamp, not normalize, so partial stick stays analog */
        if(f32 len2 = glm::dot(move, move); len2 > 1.f)
            move /= std::sqrt(len2);
        intent.move = move;
        intent.use  = (pad && b.x) || (keyboard && key(CK_f));
        intent.grab = (pad && state.axes.e.t_r > trigger_pressed) ||
                      (keyboard &&
                       (input.mouse_buttons & CIMouseButtonEvent::RightButton));
    }

    static seat_t seat_of(PlayerInfo const& info)
    {
        if(info.riding.vehicle != 0)
            return seat_t::vehicle;
        return info.mode.physics ? seat_t::body : seat_t::freecam;
    }

    void route(
        Proxy&               p,
        u64                  id,
        PlayerInput&         input,
        PlayerCamera&        cam,
        PlayerInfo&          info,
        NetworkInfo&         net,
        UIEventBus&          ui,
        PhysicsBus&          physics,
        RenderingParameters& render,
        time_point const&    t)
    {
        if(input.rotation)
        {
            cam.camera.rotation  = *std::exchange(input.rotation, std::nullopt);
            net.changes.viewport = true;
        }
        if(input.position)
        {
            cam.camera.position = *std::exchange(input.position, std::nullopt);
            net.changes.transform = net.changes.viewport = true;
        }
        if(input.look_delta != Vecf2{})
        {
            cam.camera_.rotate(
                cam.camera, input.look_delta.x, input.look_delta.y);
            input.look_delta     = {};
            net.changes.viewport = true;
        }
        cam.camera_.refresh_basis(cam.camera, cam.camera_opts);

        /* Remote seats are moved by what the network says of them */
        if(info.is_remote())
            return;

        bool const in_menu =
            input.input_mode == PlayerInput::input_mode_t::menu;
        if(in_menu)
            route_menu(input, info, ui);
        /* Route even with the menu open (intents cleared) so things stop */
        switch(seat_of(info))
        {
        case seat_t::vehicle:
            route_vehicle(input, cam, info, physics);
            break;
        case seat_t::body:
            route_body(p, id, input, cam, info, net, physics, !in_menu, t);
            break;
        case seat_t::freecam:
            route_freecam(input, cam, info, net, !in_menu);
            break;
        }

        /* Start opens and closes the seat's menu from any mode */
        if(input.start)
        {
            bool const to_menu =
                input.input_mode == PlayerInput::input_mode_t::game;
            input.input_mode = to_menu ? PlayerInput::input_mode_t::menu
                                       : PlayerInput::input_mode_t::game;
            render.render_ui = to_menu;
            navigate(
                ui,
                info.seat_idx,
                to_menu ? UINavigation::open : UINavigation::close);
        }

        /* Resampled every frame */
        input.movement = {};
        input.accel    = 1.f;
    }

    static void navigate(
        UIEventBus& ui, u32 seat_idx, UINavigation::action_t action)
    {
        UIEvent      ev{.type = UIEvent::navigation};
        UINavigation nav{.action = action, .seat_idx = seat_idx};
        ui.inject(ev, &nav);
    }

    /* This seat's menu gets its button presses */
    static void route_menu(
        PlayerInput const& input, PlayerInfo const& info, UIEventBus& ui)
    {
        std::pair<debounced_button_t const*, UINavigation::action_t> const
            actions[] = {
                {&input.accept, UINavigation::accept},
                {&input.option, UINavigation::option},
                {&input.option_2, UINavigation::option_2},
                {&input.back, UINavigation::back},
                {&input.left, UINavigation::left},
                {&input.right, UINavigation::right},
                {&input.up, UINavigation::up},
                {&input.down, UINavigation::down},
            };
        for(auto const& [button, action] : actions)
            if(*button)
                navigate(ui, info.seat_idx, action);
    }

    /* Flying the camera itself */
    static void route_freecam(
        PlayerInput const& input,
        PlayerCamera&      cam,
        PlayerInfo&        info,
        NetworkInfo&       net,
        bool               playing)
    {
        if(input.movement != Vecf3{})
        {
            cam.camera_.move(
                cam.camera,
                input.movement.x,
                input.movement.y,
                input.movement.z,
                input.accel);
            net.changes.transform = net.changes.viewport = true;
        }
        if(playing && input.back)
            info.mode.physics = true;
    }

    /* Walking the biped; physics moves the camera after its body */
    void route_body(
        Proxy&              p,
        u64                 id,
        PlayerInput&        input,
        PlayerCamera const& cam,
        PlayerInfo&         info,
        NetworkInfo&        net,
        PhysicsBus&         physics,
        bool                playing,
        time_point const&   t)
    {
        // TODO: Check for changes in position on physics movement
        net.changes.transform = net.changes.viewport = true;
        if(playing && input.back)
            info.mode.physics = false;

        /* Flatten so looking down doesn't push into the floor */
        auto planar = [](Vecf3 v) {
            v.z      = 0.f;
            f32 len2 = glm::dot(v, v);
            return len2 > 1e-8f ? v / std::sqrt(len2) : Vecf3{};
        };
        auto const& basis = cam.camera_.cached;
        Vecf3       dir   = planar(basis.forward) * input.intent.move.x +
                    planar(basis.right) * input.intent.move.y;
        if(f32 len2 = glm::dot(dir, dir); len2 > 1.f)
            dir /= std::sqrt(len2);

        auto const* data     = p.get<PhysicsData>(id);
        bool const  grounded = data && data->enabled && data->grounded;
        f32 const   jump     = take_jump(m_jumps[id], input.intent, grounded, t)
                                   ? jump_speed
                                   : 0.f;

        Physics::Event    ev{Physics::Event::Velocity};
        Physics::Velocity velocity{
            .entity_id  = id,
            .velocity   = dir * run_speed * input.accel,
            .preserve_z = true,
            .jump       = jump,
        };
        physics.process(ev, &velocity);
    }

    /* Jump with input buffer and coyote time; one jump per press */
    static bool take_jump(
        jump_t&                jump,
        PlayerInput::intent_t& intent,
        bool                   grounded,
        time_point const&      t)
    {
        if(grounded)
            jump.last_grounded = t;
        else
            jump.airborne_seen = true;
        if(!jump.airborne_seen && t - jump.jumped_at > jump_cooldown)
            jump.airborne_seen = true;

        if(!intent.jump)
        {
            jump.pending = false;
            return false;
        }
        if(!jump.pending)
        {
            jump.pending        = true;
            jump.buffered_until = t + jump_buffer;
        }
        if(t > jump.buffered_until)
        {
            intent.jump  = false;
            jump.pending = false;
            return false;
        }
        bool const on_ground =
            grounded || t - jump.last_grounded <= coyote_time;
        if(!jump.airborne_seen || !on_ground)
            return false;
        intent.jump        = false;
        jump.pending       = false;
        jump.airborne_seen = false;
        jump.jumped_at     = t;
        return true;
    }

    /* A driver steers where they look; the chase camera is Gameplay's */
    static void route_vehicle(
        PlayerInput const&  input,
        PlayerCamera const& cam,
        PlayerInfo const&   info,
        PhysicsBus&         physics)
    {
        if(!info.riding.driver || info.riding.exiting)
            return;
        Physics::Event ev{Physics::Event::Drive};
        Physics::Drive drive{
            .vehicle  = info.riding.vehicle,
            .throttle = input.intent.throttle,
            .aim      = cam.camera_.cached.forward,
        };
        physics.process(ev, &drive);
    }

    static void update_camera(PlayerCamera& cam)
    {
        using namespace typing::vectors::scene;

        cam.camera.zVals = {100.f, 0.001f};

        /* Fold the vertex→BSP permutation into the rotation so that
         * cam.camera.position can be stored in plain vertex space.
         * Equivalent to: R * T(-bsp_pos) * bsp_basis. */
        static const Matf4 bsp_basis{
            {0, 0, 1, 0},
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 1},
        };

        Matf4 const view_matrix = glm::translate(
            glm::mat4_cast(cam.camera.rotation) * bsp_basis,
            -cam.camera.position);

        cam.matrix       = GenPerspective(cam.camera);
        cam.matrix[2][2] = 0.f;
        cam.matrix       = cam.matrix * view_matrix;
        cam.rotation     = glm::mat4_cast(cam.camera.rotation) * bsp_basis;
    }

    /* Biped under its camera, yaw only; riders are placed by Gameplay */
    static void place_biped(
        PlayerCamera const& cam, PlayerInfo const& info, Model& model)
    {
        Vecf3 const forward =
            glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
        model.position =
            cam.camera.position - Vecf3{0, 0, info.biped.eye_height};
        model.rotation  = Quatf(Vecf3(0, 0, std::atan2(forward.y, forward.x)));
        model.transform = glm::translate(Matf4(1), model.position) *
                          glm::mat4_cast(model.rotation);
    }

    /* Unowned controllers drive menus by index, for split-screen joins */
    void menu_pads(
        Proxy&                                 p,
        comp_app::interfaces::ControllerInput* controllers,
        UIEventBus&                            ui,
        RenderingParameters const&             render)
    {
        if(!controllers || !render.render_ui)
            return;

        std::array<bool, 4> owned{};
        for(auto player : p.select<PlayerCamera>())
            if(auto const& index = player.get<PlayerCamera>().controller.index;
               index && *index >= 0 &&
               static_cast<size_t>(*index) < owned.size())
                owned[*index] = true;

        u32 const count = std::min<u32>(controllers->count(), m_pads.size());
        for(u32 idx = 0; idx < count; ++idx)
        {
            auto& pad = m_pads[idx];
            if(owned[idx])
            {
                pad = {};
                continue;
            }
            auto const buttons = controllers->state(idx).buttons.e;
            pad.accept |= buttons.a;
            pad.back |= buttons.b || buttons.back;
            pad.option |= buttons.y;
            pad.option_2 |= buttons.x;
            pad.up |= buttons.p_up;
            pad.down |= buttons.p_down;
            pad.left |= buttons.p_left;
            pad.right |= buttons.p_right;

            std::pair<debounced_button_t*, UINavigation::action_t> const
                actions[] = {
                    {&pad.accept, UINavigation::accept},
                    {&pad.back, UINavigation::back},
                    {&pad.option, UINavigation::option},
                    {&pad.option_2, UINavigation::option_2},
                    {&pad.up, UINavigation::up},
                    {&pad.down, UINavigation::down},
                    {&pad.left, UINavigation::left},
                    {&pad.right, UINavigation::right},
                };
            for(auto const& [button, action] : actions)
            {
                if(*button)
                    navigate(ui, idx, action);
                button->frame_end();
            }
        }
    }

    time_point                m_last_time{};
    std::map<u64, jump_t>     m_jumps;
    std::array<menu_pad_t, 4> m_pads{};
};

void alloc_player_controller(compo::EntityContainer& e)
{
    using namespace Coffee::Input;

    e.register_subsystem_inplace<PlayerController>();

    /* The keyboard seat: whichever camera takes the keyboard */
    auto keyboard_input = [&e]() -> PlayerInput* {
        for(auto entity :
            e.select<PlayerCamera, PlayerInput, PlayerInfo, NetworkInfo>())
        {
            auto [cam, input, info, net] = entity.components();
            if(cam.keyboard.enabled && info.permissions.camera)
            {
                net.changes.viewport = net.changes.transform = true;
                return &input;
            }
        }
        cWarning("No camera selected");
        return nullptr;
    };

    auto* input_bus = e.service<comp_app::BasicEventBus<CIEvent>>();
    input_bus->addEventHandler(
        1024, StandardCamera::KeyboardInput([keyboard_input] {
            auto* input = keyboard_input();
            return input ? &input->keys : nullptr;
        }));
    input_bus->addEventHandler(
        1024, StandardCamera::MouseInput([keyboard_input] {
            auto* input = keyboard_input();
            return input ? &input->look_delta : nullptr;
        }));
    /* Latched from the event, so a tap shorter than a frame still jumps */
    input_bus->addEventFunction<CIKeyEvent>(
        1024, [keyboard_input](CIEvent&, CIKeyEvent* key) {
            if(key->key != CK_Space ||
               !(key->mod & CIKeyEvent::PressedModifier) ||
               (key->mod & CIKeyEvent::RepeatedModifier))
                return;
            if(auto* input = keyboard_input())
                input->jump |= true;
        });
    input_bus->addEventFunction<CIMouseButtonEvent>(
        1024, [&e](CIEvent&, CIMouseButtonEvent* button) {
            for(auto entity : e.select<PlayerCamera, PlayerInput>())
            {
                auto [cam, input] = entity.components();
                if(!cam.keyboard.enabled)
                    continue;
                if(button->mod == CIMouseButtonEvent::Pressed)
                    input.mouse_buttons |= button->btn;
                else
                    input.mouse_buttons &= ~u32(button->btn);
            }
        });

    if constexpr(compile_info::platform::is_android)
    {
        /* Android's back action opens and closes the menu like start */
        auto& app_bus =
            e.subsystem_cast<comp_app::BasicEventBus<comp_app::AppEvent>>();
        app_bus.addEventFunction<comp_app::NavigationEvent>(
            0, [&e](comp_app::AppEvent&, comp_app::NavigationEvent* nav) {
                if(nav->navigation_type != comp_app::NavigationEvent::Back)
                    return;
                for(auto player : e.select<PlayerCamera, PlayerInput>())
                {
                    auto [cam, input] = player.components();
                    if(cam.keyboard.enabled)
                        input.start = true;
                }
            });
    }

    /* B or a resume button closed the menu without start */
    e.subsystem_cast<UIEventBus>().addEventFunction<UIMenuLeave>(
        0, [&e](UIEvent&, UIMenuLeave* leave) {
            for(auto player : e.select<PlayerInfo, PlayerInput>())
            {
                auto [info, input] = player.components();
                if(info.seat_idx == leave->seat_idx)
                    input.input_mode = PlayerInput::input_mode_t::game;
            }
            e.subsystem_cast<RenderingParameters>().render_ui = false;
        });
}
