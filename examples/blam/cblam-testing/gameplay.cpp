#include "gameplay.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "physics.h"
#include "selected_version.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/debug/formatting.h>

#include <map>
#include <set>
#include <vector>

using GameplayManifest = compo::SubsystemManifest<
    type_list_t<
        PlayerInfo,
        const PlayerInput,
        PlayerCamera,
        NetworkInfo,
        Model,
        const ObjectPhysics,
        PhysicsData,
        const AnimationPlayback>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const BlamFiles<halo_version>,
        const LoadingStatus>,
    empty_list_t>;

/* Seat labels as scripts spell them, whatever the case */
static bool iequals(std::string_view a, std::string_view b)
{
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
               return std::tolower(static_cast<unsigned char>(x)) ==
                      std::tolower(static_cast<unsigned char>(y));
           });
}

struct Gameplay : compo::RestrictedSubsystem<Gameplay, GameplayManifest>
{
    using type  = Gameplay;
    using Proxy = compo::proxy_of<GameplayManifest>;

    Gameplay()
    {
        /* After physics has moved vehicles, before the graphics loop
         * builds cameras from them */
        this->priority = 898;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        m_now = t;
        auto& physics = p.template subsystem<PhysicsBus>();
        auto& game    = p.template subsystem<GameEventBus>();
        if(auto const generation =
               p.template subsystem<BlamFiles<halo_version>>().load_generation;
           generation != m_markers_generation)
        {
            m_markers.clear();
            m_tracks.clear();
            m_leaving.clear();
            m_markers_generation = generation;
        }

        for(auto player : p.template select<
                          PlayerInfo,
                          PlayerInput,
                          PlayerCamera,
                          NetworkInfo>())
        {
            auto [info, input, cam, net] = player.components();
            if(info.is_remote())
                continue;
            bool const playing =
                input.input_mode == PlayerInput::input_mode_t::game;

            /* Forge: carry what the cursor (screen centre) points at */
            Physics::Event grab_ev{Physics::Event::Grab};
            Physics::Grab  grab{
                 .entity_id = player.id(),
                 .origin    = cam.camera.position,
                 .to_world  = Quatf(glm::transpose(Matf3(cam.rotation))),
                 .held      = playing && input.intent.grab &&
                         info.permissions.camera && !info.mode.physics &&
                         info.riding.vehicle == 0,
            };
            physics.process(grab_ev, &grab);

            bool& was_using = m_using[player.id()];
            if(playing && input.intent.use && !was_using)
                use(p, game, player.id(), info, cam);
            was_using = input.intent.use;

            if(info.riding.vehicle != 0)
                ride(p, player.id(), info, cam, net);
        }

        for(auto const& enter : std::exchange(m_enters, {}))
            enter_vehicle(p, enter);
        for(auto const& exit : std::exchange(m_exits, {}))
            exit_vehicle(p, exit);
        for(auto it = m_leaving.begin(); it != m_leaving.end();)
        {
            if(it->second > t)
            {
                ++it;
                continue;
            }
            leave_vehicle(p, it->first);
            it = m_leaving.erase(it);
        }

        for(auto rider : p.template select<PlayerInfo, Model>())
        {
            auto [info, model] = rider.components();
            if(info.riding.vehicle != 0)
                seat(p, info, model);
        }
    }

    /* A rider sits at its seat's marker, moving with the vehicle */
    void seat(Proxy& p, PlayerInfo const& info, Model& model)
    {
        auto const* vehicle = p.template get<Model>(info.riding.vehicle);
        if(!vehicle)
            return;
        auto const& marker = seat_marker(p, *vehicle, info.riding.seat);
        if(!marker)
            return;
        Matf4 const world = vehicle->transform * *marker;
        model.position    = Vecf3(world[3]);
        model.rotation    = glm::normalize(Quatf(Matf3(world)));
        model.update_matrix();
    }

    /* Model space transform of a seat's marker in the bind pose */
    std::optional<Matf4> const& seat_marker(
        Proxy& p, Model const& vehicle, i16 seat_index)
    {
        static std::optional<Matf4> const none;
        auto const seats = seats_of(p, vehicle);
        if(!seats || seat_index < 0 ||
           static_cast<size_t>(seat_index) >= seats->size())
            return none;
        return marker_transform(
            p,
            vehicle,
            std::string((*seats)[seat_index].marker_name.str()));
    }

    /* Model space transform of a named marker in the bind pose */
    std::optional<Matf4> const& marker_transform(
        Proxy& p, Model const& vehicle, std::string const& name)
    {
        auto key = std::pair{vehicle.tag, name};
        if(auto it = m_markers.find(key); it != m_markers.end())
            return it->second;
        auto& out = m_markers[key];
        if(vehicle.tag)
            out = bind_marker(
                *vehicle.tag,
                name,
                p.template subsystem<BlamFiles<halo_version>>().container.magic);
        return out;
    }

    /* A rider gets out; anyone else gets in the closest vehicle in reach */
    void use(
        Proxy&              p,
        GameEventBus&       game,
        u64                 player,
        PlayerInfo const&   info,
        PlayerCamera const& cam)
    {
        if(info.riding.vehicle != 0)
        {
            GameEvent            ev{.type = GameEvent::UnitExitVehicle};
            UnitExitVehicleEvent exit{.unit = player};
            game.process(ev, &exit);
            return;
        }
        Vecf3 const feet =
            cam.camera.position - Vecf3{0, 0, info.biped.eye_height};
        u64 nearest  = 0;
        f32 distance = vehicle_reach;
        for(auto object : p.template select<Model, ObjectPhysics>())
        {
            auto [model, physics] = object.components();
            if(!physics.drive)
                continue;
            if(f32 d = glm::distance(model.position, feet); d < distance)
            {
                nearest  = object.id();
                distance = d;
            }
        }
        if(nearest == 0)
            return;
        GameEvent             ev{.type = GameEvent::UnitEnterVehicle};
        UnitEnterVehicleEvent enter{.unit = player, .vehicle = nearest};
        auto const&           vehicle = *p.template get<Model>(nearest);
        auto const            taken   = taken_seats(p, nearest);
        f32                   closest = std::numeric_limits<f32>::max();
        auto const            seats   = seats_of(p, vehicle);
        for(i16 i = 0; seats && i < static_cast<i16>(seats->size()); i++)
        {
            if(taken.contains(i))
                continue;
            auto const& at = entry_point(p, vehicle, i);
            if(!at)
                continue;
            if(f32 d = glm::distance(*at, feet); d < closest)
            {
                closest    = d;
                enter.seat = (*seats)[i].label;
            }
        }
        if(closest == std::numeric_limits<f32>::max())
            return;
        game.process(ev, &enter);
    }

    using seats_t = semantic::Span<blam::scn::unit::seat_t const>;

    /* The vehicle's seats, from its unit tag */
    std::optional<seats_t> seats_of(Proxy& p, Model const& vehicle)
    {
        if(!vehicle.origin_object)
            return std::nullopt;
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto unit = vehicle.origin_object->data<blam::scn::unit>(magic);
        if(!unit.has_value())
            return std::nullopt;
        auto seats = unit.value()[0].seats.data(magic);
        if(!seats.has_value())
            return std::nullopt;
        return seats.value();
    }

    std::set<i16> taken_seats(Proxy& p, u64 vehicle)
    {
        std::set<i16> taken;
        for(auto rider : p.template select<PlayerInfo>())
            if(auto const& riding = rider.template get<PlayerInfo>().riding;
               riding.vehicle == vehicle)
                taken.insert(riding.seat);
        return taken;
    }

    /* Where a seat is got into and out of */
    std::optional<Vecf3> entry_point(Proxy& p, Model const& vehicle, i16 seat)
    {
        auto const seats = seats_of(p, vehicle);
        if(!seats || seat < 0 || static_cast<size_t>(seat) >= seats->size())
            return std::nullopt;
        std::string const name((*seats)[seat].marker_name.str());
        auto const*       marker = &marker_transform(p, vehicle, name + " enter");
        if(!*marker)
            marker = &marker_transform(p, vehicle, name);
        if(!*marker)
            return std::nullopt;
        return Vecf3(vehicle.transform * (**marker)[3]);
    }

    /* Seats flagged for it get the chase camera behind the vehicle along
     * the view; the rest see from the rider's eyes */
    void ride(Proxy& p, u64 rider, PlayerInfo& info, PlayerCamera& cam, NetworkInfo& net)
    {
        using seat_flags = blam::scn::unit::seat_t::seat_flags_t;
        auto const* vehicle = p.template get<Model>(info.riding.vehicle);
        if(!vehicle)
        {
            info.riding = {};
            return;
        }
        auto const seats = seats_of(p, *vehicle);
        auto const seat  = info.riding.seat;
        bool const known = seats && seat >= 0 && static_cast<size_t>(seat) < seats->size();
        auto const& marker = seat_marker(p, *vehicle, seat);
        info.riding.chase  = !known || !marker ||
                            (static_cast<u32>((*seats)[seat].flags) &
                             static_cast<u32>(seat_flags::third_person_camera));
        info.riding.armed = known && (static_cast<u32>((*seats)[seat].flags) &
                                      static_cast<u32>(seat_flags::allows_weapons));
        info.riding.pitch_range = known && (*seats)[seat].pitch_range.x < (*seats)[seat].pitch_range.y
                                      ? (*seats)[seat].pitch_range
                                      : Vecf2{-1.5f, 1.5f};
        /* The seat's own camera marker, else the rider's head */
        auto const* eye = known ? &marker_transform(
                                      p, *vehicle, std::string((*seats)[seat].camera_marker_name.str()))
                                : nullptr;
        if(!info.riding.chase && eye && *eye)
            cam.camera.position = Vecf3(vehicle->transform * (**eye)[3]);
        else if(!info.riding.chase)
            cam.camera.position = rider_eye(p, rider, info).value_or(
                Vecf3(vehicle->transform * (*marker)[3]) +
                Vecf3{0.f, 0.f, info.biped.eye_height});
        else
        {
            Vecf3 const view =
                glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
            Vecf2 const flat  = Vecf2(view);
            f32 const   yaw   = glm::dot(flat, flat) > 1e-6f ? std::atan2(flat.y, flat.x) : 0.f;
            f32 const   pitch = std::asin(std::clamp(view.z, -1.f, 1.f));
            if(auto behind = known ? track_point(p, *vehicle, (*seats)[seat], pitch) : std::nullopt)
                cam.camera.position =
                    vehicle->position + glm::angleAxis(yaw, Vecf3{0.f, 0.f, 1.f}) * *behind;
            else
                cam.camera.position =
                    vehicle->position + Vecf3{0.f, 0.f, chase_height} -
                    view * chase_distance;
        }
        net.changes.transform = net.changes.viewport = true;
    }

    /* Where the seat's camera track puts the eye for a pitch, about the
     * vehicle in the look's frame: its points run from looking straight
     * down to straight up */
    std::optional<Vecf3> track_point(
        Proxy& p, Model const& vehicle, blam::scn::unit::seat_t const& seat, f32 pitch)
    {
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const& magic = files.container.magic;
        auto [it, fresh]  = m_tracks.try_emplace(&seat);
        if(fresh)
        {
            blam::tag_index_view<halo_version> index(files.container);
            auto tracks = seat.camera_tracks.data(magic);
            if(tracks.has_value() && !tracks.value().empty())
                if(auto track = index.template data<blam::scn::camera_track>(tracks.value()[0].track);
                   track.has_value())
                    if(auto points = track.value()->control_points.data(magic); points.has_value())
                        for(auto const& point : points.value())
                            it->second.push_back(point.position);
        }
        auto const& points = it->second;
        if(points.size() < 2)
            return std::nullopt;
        f32 const at = (pitch / glm::pi<f32>() + .5f) * static_cast<f32>(points.size() - 1);
        auto const i = static_cast<size_t>(std::clamp(at, 0.f, static_cast<f32>(points.size() - 2)));
        return glm::mix(points[i], points[i + 1], std::clamp(at - static_cast<f32>(i), 0.f, 1.f));
    }

    /* A rider sees out along its seat to begin with; a remote one's view
     * is its own to send */
    void face_seat(Proxy& p, u64 rider, Model const& vehicle, i16 seat)
    {
        auto*       cam    = p.template get<PlayerCamera>(rider);
        auto const& marker = seat_marker(p, vehicle, seat);
        if(!cam || !marker)
            return;
        Vecf3 const forward = Matf3(vehicle.transform * *marker) * Vecf3{1.f, 0.f, 0.f};
        f32 const   yaw     = std::atan2(forward.y, forward.x);
        cam->camera.rotation = glm::angleAxis(glm::pi<f32>() - yaw, Vecf3{0.f, 1.f, 0.f});
        if(auto* net = p.template get<NetworkInfo>(rider))
            net->changes.viewport = true;
    }

    /* The rider's camera marker, where its seated pose holds it */
    std::optional<Vecf3> rider_eye(Proxy& p, u64 rider, PlayerInfo const& info)
    {
        auto const* body = p.template get<Model>(rider);
        auto const* anim = p.template get<AnimationPlayback>(rider);
        i32 const   node = info.biped.camera_node;
        if(!body || !anim || node < 0 || static_cast<size_t>(node) >= anim->pose.size())
            return std::nullopt;
        return Vecf3((body->transform * anim->pose[node] * info.biped.camera_marker)[3]);
    }

    /* Seats come from the vehicle's unit tag; a seat is taken while another
     * player rides in it */
    void enter_vehicle(Proxy& p, UnitEnterVehicleEvent const& enter)
    {
        auto* info    = p.template get<PlayerInfo>(enter.unit);
        auto* vehicle = p.template get<Model>(enter.vehicle);
        if(!info || !vehicle || !vehicle->origin_object ||
           info->riding.vehicle != 0)
            return;
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto unit = vehicle->origin_object->data<blam::scn::unit>(magic);
        if(!unit.has_value())
            return;
        auto seats = unit.value()[0].seats.data(magic);
        if(!seats.has_value())
            return;
        std::set<i16> taken;
        for(auto rider : p.template select<PlayerInfo>())
            if(auto const& riding = rider.template get<PlayerInfo>().riding;
               riding.vehicle == enter.vehicle)
                taken.insert(riding.seat);
        using flags_t    = blam::scn::unit::seat_t::seat_flags_t;
        auto const label = enter.seat.str();
        for(i16 i = 0; i < static_cast<i16>(seats.value().size()); i++)
        {
            auto const& seat = seats.value()[i];
            bool const  driver =
                (seat.flags & flags_t::driver) != flags_t::none;
            if(taken.contains(i) ||
               (label.empty() ? !driver : !iequals(seat.label.str(), label)))
                continue;
            info->riding = {
                .vehicle = enter.vehicle,
                .seat    = i,
                .driver  = driver,
            };
            if(!info->is_remote())
                face_seat(p, enter.unit, *vehicle, i);
            cDebug(
                "Player {} takes seat {} of {}",
                info->player_idx,
                seat.label.str(),
                enter.vehicle);
            return;
        }
    }

    /* The seat's exit animation plays out before the rider stands up */
    void exit_vehicle(Proxy& p, UnitExitVehicleEvent const& exit)
    {
        auto* info = p.template get<PlayerInfo>(exit.unit);
        if(!info || info->riding.vehicle == 0 || info->riding.exiting)
            return;
        f32 const seconds = exit_length(p, *info);
        if(seconds <= 0.f)
        {
            leave_vehicle(p, exit.unit);
            return;
        }
        info->riding.exiting = true;
        m_leaving[exit.unit] =
            m_now + std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::duration<f32>(seconds));
    }

    /* How long the rider's graph plays the seat's exit for, 0 if none */
    f32 exit_length(Proxy& p, PlayerInfo const& info)
    {
        auto const* vehicle = p.template get<Model>(info.riding.vehicle);
        auto const* graph   = info.biped.anim_graph;
        auto const  seats   = vehicle ? seats_of(p, *vehicle) : std::nullopt;
        if(!graph || !seats || info.riding.seat < 0 ||
           static_cast<size_t>(info.riding.seat) >= seats->size())
            return 0.f;
        auto const label = (*seats)[info.riding.seat].label.str();
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto units = graph->units.data(magic);
        auto anims = graph->animations.data(magic);
        if(!units.has_value() || !anims.has_value())
            return 0.f;
        for(auto const& unit : units.value())
        {
            if(unit.label.str() != label)
                continue;
            auto slots = unit.animations.data(magic);
            if(!slots.has_value() ||
               slots.value().size() <= blam::antr::unit::exit)
                return 0.f;
            i16 const index = slots.value()[blam::antr::unit::exit].animation;
            if(index < 0 || static_cast<size_t>(index) >= anims.value().size())
                return 0.f;
            auto const& clip = anims.value()[index];
            return static_cast<f32>(clip.frame_count) / clip.frame_rate();
        }
        return 0.f;
    }

    /* Standing at the seat's entry point, else on the vehicle's left */
    void leave_vehicle(Proxy& p, u64 unit)
    {
        auto* info = p.template get<PlayerInfo>(unit);
        auto* cam  = p.template get<PlayerCamera>(unit);
        if(!info || !cam || info->riding.vehicle == 0)
            return;
        if(auto const* vehicle = p.template get<Model>(info->riding.vehicle))
        {
            Vecf3 feet;
            if(auto at = entry_point(p, *vehicle, info->riding.seat))
                feet = *at;
            else
            {
                Vecf3 left = vehicle->rotation * Vecf3{0.f, 1.f, 0.f};
                left.z     = 0.f;
                if(glm::dot(left, left) > 1e-6f)
                    left = glm::normalize(left);
                feet = vehicle->position + left * exit_distance;
            }
            cam->camera.position =
                feet + Vecf3{0.f, 0.f, info->biped.eye_height + .3f};

            /* Out at the speed the vehicle was moving at that spot */
            auto const* physics =
                p.template get<ObjectPhysics>(info->riding.vehicle);
            auto* data = p.template get<PhysicsData>(unit);
            if(physics && data)
            {
                Vecf3 const centre =
                    vehicle->position +
                    vehicle->rotation *
                        (physics->mass_points
                             ? physics->mass_points->center_of_mass
                             : Vecf3{});
                data->launch =
                    physics->linear_velocity +
                    glm::cross(physics->angular_velocity, feet - centre);
            }
        }
        cDebug("Player {} leaves {}", info->player_idx, info->riding.vehicle);
        info->riding = {};
        if(auto* net = p.template get<NetworkInfo>(unit))
            net->changes.viewport = net->changes.transform = true;
    }

    static constexpr f32 vehicle_reach  = 2.5f;
    static constexpr f32 chase_height   = 1.2f;
    static constexpr f32 chase_distance = 3.5f;
    static constexpr f32 exit_distance  = 1.6f;

    /* Events from anyone (scripts, players), acted on with the proxy */
    std::vector<UnitEnterVehicleEvent> m_enters;
    std::vector<UnitExitVehicleEvent>  m_exits;
    std::map<u64, bool>                m_using;
    std::map<std::pair<blam::tag_t const*, std::string>, std::optional<Matf4>>
        m_markers;
    std::map<blam::scn::unit::seat_t const*, std::vector<Vecf3>> m_tracks;
    /* Riders playing their exit, and when they stand up */
    std::map<u64, time_point> m_leaving;
    time_point                m_now{};
    u32 m_markers_generation{0};

};

void alloc_gameplay(compo::EntityContainer& e)
{
    auto& gameplay = e.register_subsystem_inplace<Gameplay>();
    auto& game     = e.subsystem_cast<GameEventBus>();
    game.addEventFunction<UnitEnterVehicleEvent>(
        1024, [&gameplay](GameEvent&, UnitEnterVehicleEvent* enter) {
            gameplay.m_enters.push_back(*enter);
        });
    game.addEventFunction<UnitExitVehicleEvent>(
        1024, [&gameplay](GameEvent&, UnitExitVehicleEvent* exit) {
            gameplay.m_exits.push_back(*exit);
        });
}
