#include "player_controller.h"
#include "ticks.h"

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

    /* A press this long before landing still jumps on landing */
    static constexpr duration jump_buffer = 150ms;
    /* A press this long after stepping off an edge still jumps */
    static constexpr duration coyote_time = 100ms;
    /* The dead's camera backs this far off over this long, rising a little */
    static constexpr f32 death_distance  = 2.5f; /* wu */
    static constexpr f32 death_pull_time = 1.5f; /* s */
    static constexpr f32 death_rise      = .5f;
    static constexpr f32 death_wall_gap  = .2f;  /* wu */
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
        auto*          mouse       = p.service<comp_app::MouseInput>();
        auto*          controllers = p.service<comp_app::ControllerInput>();
        auto&          ui          = p.subsystem<UIEventBus>();
        auto&          physics     = p.subsystem<PhysicsBus>();
        auto&          render      = p.subsystem<RenderingParameters>();
        m_remote_queue->poll();

        /* A cutscene's single view covers the whole window */
        auto const cutscene = cutscene_seat(p, p.template subsystem<CinematicState>().playing);
        for(auto player : p.select<PlayerCamera, PlayerInfo>())
        {
            auto [cam, info] = player.components();
            if(f32 const aspect = cutscene ? cam.screen_aspect : cam.split_aspect; aspect > 0.f)
                cam.camera.aspect = aspect;
        }
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
                sample(input, cam, controllers, dt, info.mode.physics);
                if(info.is_remote() && info.mode.simulated)
                    take_remote_input(player.id(), input, cam);
                /* Scripts take the controls away; looking around stays
                 * unless a cutscene has the camera */
                if(!p.template subsystem<CinematicState>().input && !info.is_remote() &&
                   input.input_mode == PlayerInput::input_mode_t::game)
                {
                    input.movement = {};
                    input.intent   = {};
                    if(cam.cinematic)
                        input.look_delta = {};
                }
                /* The dead wait to come back, and nobody plays on once the
                 * game is over */
                bool const over =
                    p.template subsystem<GameState>().phase == GameState::phase_t::over;
                if(auto const* status = p.template get<BipedStatus>(player.id());
                   ((status && status->dead) || over) && info.mode.physics)
                {
                    input.movement = {};
                    input.intent   = {};
                }
                clamp_pitch(input, cam, info);
                if(!info.is_remote())
                    m_look[player.id()] += input.look_delta;
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
            /* The dead lie where they fell, watched from further and further
             * back as the camera pulls away from them */
            auto const* status = p.template get<BipedStatus>(player.id());
            bool const  dead   = status && status->dead && info.mode.physics && !info.is_remote();
            cam.view_offset    = dead ? death_view(p, player.id(), cam, dt) : Vecf3{};
            if(!dead)
                m_dead_for.erase(player.id());
            ease_crouch(cam, info, info.crouched && seat_of(info) == seat_t::body, dt);
            update_camera(cam);
            if(info.riding.vehicle == 0 && !dead)
                place_biped(cam, info, model);
        }
        for(auto ent : p.select<Attachment, Model>())
        {
            auto [attachment, model] = ent.components();
            if(auto const* cam = p.get<PlayerCamera>(attachment.parent))
                place_first_person(*cam, cam->camera.position, attachment, model);
        }
        menu_pads(p, controllers, ui, render);
        tick_inputs(p);
    }

    /* Each tick's input from what the frame sampled: held state repeats,
     * look is shared out over the ticks it spans */
    void tick_inputs(Proxy& p)
    {
        auto const ticks = m_ticks->take();
        if(ticks.empty())
            return;
        auto& game = p.subsystem<GameEventBus>();
        for(auto player : p.select<PlayerCamera, PlayerInput, PlayerInfo>())
        {
            auto [cam, input, info] = player.components();
            if(info.is_remote() || !cam.is_active())
                continue;
            auto const& intent  = input.intent;
            InputIntent intents = InputIntent::none;
            for(auto [held, what] : {std::pair{intent.fire, InputIntent::fire},
                                     std::pair{intent.grab, InputIntent::grab},
                                     std::pair{intent.use, InputIntent::use},
                                     std::pair{intent.jump, InputIntent::jump},
                                     std::pair{intent.crouch, InputIntent::crouch}})
                if(held)
                    intents |= what;
            Vecf2 const look = std::exchange(m_look[player.id()], Vecf2{}) /
                               static_cast<f32>(ticks.size());
            for(auto const& tick : ticks)
            {
                GameEvent            ev{.type = GameEvent::PlayerTickInput};
                PlayerTickInputEvent data{
                    .player     = player.id(),
                    .player_idx = info.player_idx,
                    .tick       = tick.tick,
                    .move       = intent.move,
                    .look       = look,
                    .view       = cam.camera.rotation,
                    .intents    = intents,
                    .resync     = tick.resync,
                };
                game.inject(ev, &data);
            }
        }
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
        duration const&                        dt,
        bool                                   physics)
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
            input.swap |= b.y;
            input.reload |= b.x;
            input.grenade |= state.axes.e.t_l > trigger_pressed;
            input.grenade_switch |= b.b_l;
            /* B strikes in play and backs out of menus; back also toggles
             * freecam */
            input.back |= b.back || (in_menu && b.b);
            input.melee |= !in_menu && b.b;
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
            input.swap |= key(CK_HTab);
            input.reload |= key(CK_r);
            input.grenade_switch |= key(CK_g);
            input.melee |= key(CK_q);
            /* Grabs in forge, throws in physics mode */
            input.grenade |= (input.mouse_buttons & CIMouseButtonEvent::RightButton) != 0;
            input.back |= (!in_menu && key(CK_v)) || (in_menu && key(CK_BackSpace));
        }

        input.scores = (pad && b.p_down) || (keyboard && key(CK_F1));
        input.crouch = (pad && b.s_l) || (keyboard && key(CK_LCtrl));

        auto& intent = input.intent;
        if(input.jump)
            intent.jump = true;
        if(input.swap)
            intent.swap = true;
        if(input.reload)
            intent.reload = true;
        /* Grenades are for bodies; forge keeps these buttons */
        if(input.grenade && physics)
            intent.grenade = true;
        if(input.grenade_switch && physics)
            intent.grenade_switch = true;
        if(input.melee && physics)
            intent.melee = true;
        if(in_menu)
        {
            input.movement  = {};
            intent.move     = {};
            intent.throttle = intent.steer = 0.f;
            intent.use = intent.grab = intent.jump = intent.fire = false;
            intent.swap = intent.reload = false;
            intent.grenade = intent.grenade_switch = intent.melee = false;
            intent.crouch = false;
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
        intent.crouch = input.crouch && physics;
        intent.grab = (pad && state.axes.e.t_r > trigger_pressed) ||
                      (keyboard &&
                       (input.mouse_buttons & CIMouseButtonEvent::RightButton));
        intent.fire = (pad && state.axes.e.t_r > trigger_pressed) ||
                      (keyboard &&
                       (input.mouse_buttons & CIMouseButtonEvent::LeftButton));
        intent.fire2 = (pad && state.axes.e.t_l > trigger_pressed) ||
                       (keyboard &&
                        (input.mouse_buttons & CIMouseButtonEvent::RightButton));
    }

    /* Stops short of straight up or down, where turning would spin the
     * view around; a view already past it may only come back */
    static void clamp_pitch(
        PlayerInput& input, PlayerCamera& cam, PlayerInfo const& info)
    {
        if(input.look_delta.y == 0.f)
            return;
        cam.camera_.refresh_basis(cam.camera, cam.camera_opts);
        f32 const pitch =
            std::asin(std::clamp(cam.camera_.cached.forward.z, -1.f, 1.f));
        auto const& range =
            info.riding.vehicle ? info.riding.pitch_range : info.biped.pitch_range;
        input.look_delta.y = std::clamp(
                                 pitch + input.look_delta.y,
                                 std::min(range.x, pitch),
                                 std::max(range.y, pitch)) -
                             pitch;
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

        /* Remote seats are moved by what the network says of them, or
         * walked here when they have no physics */
        if(info.is_remote())
        {
            if(info.mode.simulated && seat_of(info) == seat_t::body)
                route_body(p, id, input, cam, info, net, physics, false, t);
            return;
        }

        bool const in_menu =
            input.input_mode == PlayerInput::input_mode_t::menu;
        bool const was_in_menu = in_menu;
        bool const was_physics = info.mode.physics;
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

        /* Headless there is no mouse to grab */
        if(auto* mouse = cam.keyboard.enabled ? p.service<comp_app::MouseInput>() : nullptr)
        {
            if(was_in_menu != (input.input_mode == PlayerInput::input_mode_t::menu) ||
                    (was_physics != info.mode.physics))
            {
                if(input.input_mode == PlayerInput::input_mode_t::game && info.mode.physics)
                    mouse->setMouseGrab(true);
                else
                    mouse->setMouseGrab(false);
            }
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

    /* A remote seat's newest input, as if sampled here */
    void take_remote_input(u64 id, PlayerInput& input, PlayerCamera& cam)
    {
        auto it = m_remote_inputs.find(id);
        if(it == m_remote_inputs.end())
            return;
        auto const& in      = it->second;
        cam.camera.rotation = in.view;
        cam.camera_.refresh_basis(cam.camera, cam.camera_opts);
        input.intent.move = in.move;
        auto const held   = [&in](InputIntent i) {
            return (in.intents & i) != InputIntent::none;
        };
        input.intent.jump = held(InputIntent::jump);
        input.intent.fire = held(InputIntent::fire);
        auto const pressed =
            std::exchange(m_remote_presses[id], InputIntent::none);
        auto const press = [pressed](bool& intent, InputIntent i) {
            intent = intent || (pressed & i) != InputIntent::none;
        };
        press(input.intent.swap, InputIntent::swap);
        press(input.intent.reload, InputIntent::reload);
        press(input.intent.grenade, InputIntent::grenade);
        press(input.intent.melee, InputIntent::melee);
        input.intent.crouch = held(InputIntent::crouch);
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
        /* Each way at its own clip's speed; |move| <= 1 keeps diagonals
         * within the faster of the two */
        auto const&  basis = cam.camera_.cached;
        auto const&  biped = info.biped;
        Vecf2 const& move  = input.intent.move;
        info.crouched      = input.intent.crouch;
        /* Crouch shares LCtrl with the boost, which it overrides */
        f32 const    pace  = info.crouched ? biped.crouch_speed : input.accel;
        f32 const    along = move.x >= 0.f ? biped.run_speed : biped.back_speed;
        Vecf3 const  dir   = (planar(basis.forward) * move.x * along +
                           planar(basis.right) * move.y * biped.strafe_speed) *
                          pace * info.speed_scale();

        auto const* data     = p.get<PhysicsData>(id);
        bool const  grounded = data && data->enabled && data->grounded;
        f32 const   jump     = take_jump(m_jumps[id], input.intent, grounded, t)
                                   ? info.biped.jump_velocity
                                   : 0.f;

        Physics::Event    ev{Physics::Event::Velocity};
        Physics::Velocity velocity{
            .entity_id  = id,
            .velocity   = dir,
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

    /* The eye sinks or rises over the biped's transition, feet kept */
    static void ease_crouch(PlayerCamera& cam, PlayerInfo& info, bool down, duration const& dt)
    {
        f32 const eye  = info.eye_height();
        f32 const step = std::chrono::duration<f32>(dt).count() /
                         std::max(info.biped.crouch_time, 1e-3f);
        info.crouched = down;
        info.crouch   = std::clamp(info.crouch + (down ? step : -step), 0.f, 1.f);
        cam.camera.position.z += info.eye_height() - eye;
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
            .steer    = input.intent.steer,
            .aim      = cam.camera_.cached.forward,
        };
        physics.process(ev, &drive);
    }

    static void update_camera(PlayerCamera& cam)
    {
        /* CameraControl draws cutscenes */
        if(cam.cinematic)
            return;
        build_camera_matrix(cam, cam.camera.position + cam.view_offset);
    }

    /* Behind and above the body along where the player looks, short of any
     * wall in the way */
    Vecf3 death_view(Proxy& p, u64 id, PlayerCamera const& cam, duration const& dt)
    {
        f32& since = m_dead_for[id];
        since += std::chrono::duration<f32>(dt).count();
        f32 const pull  = glm::smoothstep(0.f, 1.f, std::min(since / death_pull_time, 1.f));
        Vecf3 const ahead = glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
        Vecf3 const back  = glm::normalize(-ahead + Vecf3{0.f, 0.f, death_rise});
        f32 reach = death_distance * pull;
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{
            .from   = cam.camera.position,
            .to     = cam.camera.position + back * reach,
            .ignore = id,
        };
        p.template subsystem<PhysicsBus>().process(ev, &ray);
        if(ray.hit)
            reach = std::max(reach * ray.fraction - death_wall_gap, 0.f);
        return back * reach;
    }

    /* Biped under its camera, yaw only; riders are placed by Gameplay */
    static void place_biped(
        PlayerCamera const& cam, PlayerInfo const& info, Model& model)
    {
        Vecf3 const forward =
            glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
        model.position =
            cam.camera.position - Vecf3{0, 0, info.eye_height()};
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
    std::unique_ptr<TickQueue> m_ticks;
    std::shared_ptr<GameEventBus::queue_type<PlayerTickInputEvent>>
                                        m_remote_queue;
    std::map<u64, PlayerTickInputEvent> m_remote_inputs; /*!< Newest per body */
    std::map<u64, InputIntent> m_remote_presses; /*!< Not yet acted on */
    std::map<u64, f32>         m_dead_for; /*!< Seconds dead, for the camera */
    std::map<u64, Vecf2>       m_look; /*!< Device look since the last tick */
    std::map<u64, jump_t>     m_jumps;
    std::array<menu_pad_t, 4> m_pads{};
};

void alloc_player_controller(compo::EntityContainer& e)
{
    using namespace Coffee::Input;

    auto& controller   = e.register_subsystem_inplace<PlayerController>();
    controller.m_ticks = std::make_unique<TickQueue>(e.subsystem_cast<GameEventBus>());
    controller.m_remote_queue =
        e.subsystem_cast<GameEventBus>()
            .addQueuedEventFunction<PlayerTickInputEvent>(
                0, [&controller](GameEvent&, PlayerTickInputEvent* input) {
                    if(input->origin != MapLoadEvent::Remote)
                        return;
                    /* Presses between the ticks taken here count too */
                    auto& last = controller.m_remote_inputs[input->player];
                    controller.m_remote_presses[input->player] |=
                        input->intents & ~last.intents;
                    last = *input;
                });

    enum class input_mode_t
    {
        physics_only,
        flycam_only,
        gameplay_mode,
        menu_mode,
        all,
    };

    /* The keyboard seat: whichever camera takes the keyboard */
    auto keyboard_input = [&e](input_mode_t mode) -> PlayerInput* {
        for(auto entity :
            e.select<PlayerCamera, PlayerInput, PlayerInfo, NetworkInfo>())
        {
            auto [cam, input, info, net] = entity.components();
            if(cam.keyboard.enabled && info.permissions.camera)
            {
                const bool in_game = input.input_mode == PlayerInput::input_mode_t::game;
                if(mode == input_mode_t::all)
                    ;
                else if(mode == input_mode_t::menu_mode && !in_game)
                    ;
                else if(mode == input_mode_t::gameplay_mode && in_game)
                    ;
                else if(mode == input_mode_t::physics_only && in_game && info.mode.physics)
                    ;
                else if(mode == input_mode_t::flycam_only && in_game && !info.mode.physics)
                    ;
                else
                    continue;
                net.changes.viewport = net.changes.transform = true;
                return &input;
            }
        }
        return nullptr;
    };

    auto* input_bus = e.service<comp_app::BasicEventBus<CIEvent>>();
    input_bus->addEventHandler(
        1024, StandardCamera::KeyboardInput([keyboard_input] {
            auto* input = keyboard_input(input_mode_t::all);
            return input ? &input->keys : nullptr;
        }));
    input_bus->addEventHandler(
        1024, StandardCamera::MouseInput([keyboard_input] {
            auto* input = keyboard_input(input_mode_t::physics_only);
            return input ? &input->look_delta : nullptr;
        }, CIMouseButtonEvent::NoneBtn));
    input_bus->addEventHandler(
        1024, StandardCamera::MouseInput([keyboard_input] {
            auto* input = keyboard_input(input_mode_t::flycam_only);
            return input ? &input->look_delta : nullptr;
        }, CIMouseButtonEvent::LeftButton));
    /* Latched from the event, so a tap shorter than a frame still jumps */
    input_bus->addEventFunction<CIKeyEvent>(
        1024, [keyboard_input](CIEvent&, CIKeyEvent* key) {
            if(key->key != CK_Space ||
               !(key->mod & CIKeyEvent::PressedModifier) ||
               (key->mod & CIKeyEvent::RepeatedModifier))
                return;
            if(auto* input = keyboard_input(input_mode_t::gameplay_mode))
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
