#include "game/teleporters.h"

#include "components.h"
#include "data.h"
#include "game_events.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
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
    type_list_t<PlayerInfo, const PlayerCamera, const BipedStatus>,
    type_list_t<GameEventBus, const LoadingStatus, const BlamFiles<halo_version>>,
    empty_list_t>;

/* Each peer moves its own players through: the server only checks the hop */
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
        }
        if(m_flags.empty())
            return;
        for(auto player : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam]   = player.components();
            auto const* status = p.template get<BipedStatus>(player.id());
            info.teleport_flash = std::max(info.teleport_flash - dt / flash_seconds, 0.f);
            if(info.is_remote() || !info.mode.physics || info.riding.vehicle || (status && status->dead))
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
            if(!from || std::exchange(inside, true))
            {
                inside = from != nullptr;
                continue;
            }
            if(auto const* to = destination(from->team_index))
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
    }

    /* The channel's exits in turn */
    flag_t const* destination(u16 channel)
    {
        std::vector<flag_t const*> exits;
        for(auto const* flag : m_flags)
            if(flag->type == flag_t::teleport_to && flag->team_index == channel)
                exits.push_back(flag);
        if(exits.empty())
            return nullptr;
        return exits[m_next[channel]++ % exits.size()];
    }

    static constexpr f32 pad_reach = .6f;  /* wu across the pad */
    static constexpr f32 pad_below = .5f;  /* wu the feet may be under the flag */
    static constexpr f32 pad_above = 1.f;  /* wu over it */
    static constexpr f32 exit_lift = .05f; /* wu */
    /* matg's multiplayer sounds: sfx\ui\teleporter_activate */
    static constexpr u16 teleporter_sound = 27;
    static constexpr f32 flash_seconds    = .5f;

    time_point                 m_last{};
    u32                        m_load_generation{0};
    std::vector<flag_t const*> m_flags;
    std::map<u64, bool>        m_inside;
    std::map<u16, u32>         m_next;
};

void alloc_teleporters(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<Teleporters>();
}
