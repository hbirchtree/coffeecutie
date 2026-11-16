#include "game/teleporters.h"

#include "components.h"
#include "data.h"
#include "game_events.h"
#include "network/networking.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <set>
#include <vector>

namespace {

using flag_t = blam::scn::multiplayer_flag;

/* The map's teleporter flags, either end */
std::vector<flag_t const*> teleporter_flags(BlamFiles<halo_version> const& files)
{
    std::vector<flag_t const*> out;
    auto const* scenario = files.container.scenario().value_or(nullptr);
    if(!scenario)
        return out;
    auto flags = scenario->netgame.flags.data(files.container.magic);
    if(!flags.has_value())
        return out;
    for(auto const& flag : flags.value())
        if(flag.type == flag_t::teleport_from || flag.type == flag_t::teleport_to)
            out.push_back(&flag);
    return out;
}

/* Feet on a flag: over it, about level with it */
bool on(flag_t const& flag, Vecf3 const& feet, f32 reach, f32 below, f32 above)
{
    Vecf3 const d = feet - flag.pos;
    return d.x * d.x + d.y * d.y <= reach * reach && d.z >= -below && d.z <= above;
}

} // namespace

bool teleporter_hop(BlamFiles<halo_version> const& files, Vecf3 const& before, Vecf3 const& after)
{
    /* Eyes, and reports a few frames apart: looser than the pads themselves */
    constexpr f32 reach = 3.f;
    auto const    flags = teleporter_flags(files);
    for(auto const* from : flags)
        if(from->type == flag_t::teleport_from && on(*from, before, reach, reach, reach))
            for(auto const* to : flags)
                if(to->type == flag_t::teleport_to && to->team_index == from->team_index &&
                   on(*to, after, reach, reach, reach))
                    return true;
    return false;
}

using TeleporterManifest = compo::SubsystemManifest<
    type_list_t<PlayerInfo, const PlayerCamera, const BipedStatus, const Model>,
    type_list_t<GameEventBus, const LoadingStatus, const NetworkState, const BlamFiles<halo_version>>,
    empty_list_t>;

/* Each peer moves its own players through: the server only checks the hop.
 * An exit with something on it lets no one through; players standing on it
 * are telefragged by the authority once someone has waited a moment */
struct Teleporters : compo::RestrictedSubsystem<Teleporters, TeleporterManifest>
{
    using type  = Teleporters;
    using Proxy = compo::proxy_of<TeleporterManifest>;

    Teleporters()
    {
        this->priority = 905;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        f32 const dt = m_last != time_point{} ? std::chrono::duration<f32>(t - m_last).count() : 0.f;
        m_last       = t;
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(m_load_generation != files.load_generation)
        {
            m_load_generation = files.load_generation;
            m_flags           = teleporter_flags(files);
            m_inside.clear();
            m_next.clear();
            m_waited.clear();
        }
        if(m_flags.empty())
            return;
        bool const authority = !p.template subsystem<NetworkState>().replica;
        std::set<u64> waiting;
        for(auto player : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam]   = player.components();
            auto const* status = p.template get<BipedStatus>(player.id());
            info.teleport_flash = std::max(info.teleport_flash - dt / flash_seconds, 0.f);
            if((!info.is_remote() && !info.mode.physics) || info.riding.vehicle || (status && status->dead))
            {
                m_inside.erase(player.id());
                continue;
            }
            Vecf3 const   feet = cam.camera.position - Vecf3{0.f, 0.f, info.eye_height()};
            flag_t const* from = nullptr;
            for(auto const* flag : m_flags)
                if(flag->type == flag_t::teleport_from && on(*flag, feet, pad_reach, pad_below, pad_above))
                    from = flag;
            /* Once through, it steps off where it came out before going again */
            bool& inside = m_inside[player.id()];
            if(!from || inside)
            {
                inside = from != nullptr && inside;
                continue;
            }
            auto const [to, blockers] = destination(p, from->team_index, player.id());
            if(!to)
            {
                if(authority && !blockers.empty())
                    telefrag(p, player.id(), blockers, dt, waiting);
                continue;
            }
            if(info.is_remote())
                continue;
            inside = true;
            m_next[from->team_index]++;
            {
                GameEvent           ev{.type = GameEvent::PlayerTeleport};
                PlayerTeleportEvent move{
                    .entity_id = player.id(),
                    .position  = to->pos + Vecf3{0.f, 0.f, info.biped.height / 2 + exit_lift},
                    .rotation  = glm::angleAxis(glm::pi<f32>() - to->yaw, Vecf3{0.f, 1.f, 0.f}),
                };
                p.template subsystem<GameEventBus>().inject(ev, &move);
                /* Heard and seen by whoever went through */
                GameEvent     say_ev{.type = GameEvent::Announce};
                AnnounceEvent say{.sound = teleporter_sound};
                p.template subsystem<GameEventBus>().inject(say_ev, &say);
                info.teleport_flash = 1.f;
            }
        }
        /* Stepping off the pad starts the wait over */
        std::erase_if(m_waited, [&waiting](auto const& w) { return !waiting.contains(w.first); });
    }

    /* The channel's next clear exit; else who stands on the first exit
     * that only players on foot block */
    std::pair<flag_t const*, std::vector<u64>> destination(Proxy& p, u16 channel, u64 self)
    {
        std::vector<flag_t const*> exits;
        for(auto const* flag : m_flags)
            if(flag->type == flag_t::teleport_to && flag->team_index == channel)
                exits.push_back(flag);
        std::optional<std::vector<u64>> fraggable;
        for(size_t i = 0; i < exits.size(); i++)
        {
            auto const* to = exits[(m_next[channel] + i) % exits.size()];
            auto const [solid, players] = on_exit(p, *to, self);
            if(!solid && players.empty())
                return {to, {}};
            if(!solid && !fraggable)
                fraggable = players;
        }
        return {nullptr, fraggable.value_or(std::vector<u64>{})};
    }

    /* What stands on an exit: a vehicle or anything not a player on foot
     * makes it solid */
    std::pair<bool, std::vector<u64>> on_exit(Proxy& p, flag_t const& exit, u64 self)
    {
        bool             solid = false;
        std::vector<u64> players;
        Vecf3 const      middle = exit.pos + Vecf3{0.f, 0.f, exit_middle};
        for(auto ent : p.template select<BipedStatus, Model>())
        {
            auto [status, model] = ent.components();
            if(ent.id() == self || status.dead)
                continue;
            auto const* info = p.template get<PlayerInfo>(ent.id());
            if(info && info->riding.vehicle)
                continue;
            if(!on(exit, model.position, exit_reach, exit_below, exit_above))
                continue;
            if(info)
                players.push_back(ent.id());
            else
                solid = true;
        }
        for(auto ent : p.template select<Model>())
        {
            if((ent.tags() & ObjectVehicle) == 0)
                continue;
            auto const& model = ent.template get<Model>();
            if(glm::distance(model.position, middle) < model.render_radius + exit_reach)
                solid = true;
        }
        return {solid, players};
    }

    /* Someone waiting on a pad kills those standing on the exit, after a
     * moment */
    void telefrag(Proxy& p, u64 waiter, std::vector<u64> const& blockers, f32 dt, std::set<u64>& waiting)
    {
        waiting.insert(waiter);
        if((m_waited[waiter] += dt) < telefrag_delay)
            return;
        m_waited.erase(waiter);
        for(u64 unit : blockers)
        {
            auto const*       info = p.template get<PlayerInfo>(unit);
            GameEvent         ev{.type = GameEvent::UnitVitality};
            UnitVitalityEvent kill{
                .unit    = unit,
                .player  = info ? info->player_idx : ObjectHitEvent::no_player,
                .body    = 0.f,
                .shield  = 0.f,
                .dead    = true,
                .violent = true,
            };
            p.template subsystem<GameEventBus>().inject(ev, &kill);
        }
    }

    static constexpr f32 pad_reach = .6f;  /* wu across the pad */
    static constexpr f32 pad_below = .5f;  /* wu the feet may be under the flag */
    static constexpr f32 pad_above = 1.f;  /* wu over it */
    static constexpr f32 exit_lift = .05f; /* wu */
    static constexpr f32 exit_reach  = .5f;  /* wu across an exit that counts as on it */
    static constexpr f32 exit_below  = .5f;
    static constexpr f32 exit_above  = 1.f;
    static constexpr f32 exit_middle = .35f; /* wu up, where a vehicle would be in the way */
    static constexpr f32 telefrag_delay = 1.f; /* s waited before those on the exit die */
    /* matg's multiplayer sounds: sfx\ui\teleporter_activate */
    static constexpr u16 teleporter_sound = 27;
    static constexpr f32 flash_seconds    = .5f;

    time_point                 m_last{};
    u32                        m_load_generation{0};
    std::vector<flag_t const*> m_flags;
    std::map<u64, bool>        m_inside;
    std::map<u16, u32>         m_next;
    std::map<u64, f32>         m_waited; /*!< s each waiting player has stood blocked */
};

void alloc_teleporters(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<Teleporters>();
}
