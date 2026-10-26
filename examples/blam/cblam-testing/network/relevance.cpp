#include "relevance.h"

#include "components.h"
#include "network/networking.h"
#include "ticks.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <glm/glm.hpp>

namespace {

using libc_types::f32;
using libc_types::u32;
using libc_types::u64;
using typing::vector_types::Matf3;
using typing::vector_types::Vecf3;

using RelevanceManifest = compo::SubsystemManifest<
    type_safety::type_list_t<
        NetworkInfo,
        const PlayerInfo,
        const PlayerCamera,
        const Model,
        const ObjectPhysics>,
    type_safety::type_list_t<const NetworkState, const TickClock>,
    type_safety::empty_list_t>;

/*! Rates, per remote player, the objects and players the server sends.
 *  What a player is close to or looking at comes every tick; what is far
 *  and behind them as seldom as a second, as Halo Reach's network
 *  priorities did. Ahead of Networking, which sends by it */
struct NetworkRelevance : compo::RestrictedSubsystem<NetworkRelevance, RelevanceManifest>
{
    using type  = NetworkRelevance;
    using Proxy = compo::proxy_of<RelevanceManifest>;

    NetworkRelevance()
    {
        this->priority = 1270;
    }

    struct viewer_t
    {
        u32   idx;
        Vecf3 eye;
        Vecf3 forward;
        u64   vehicle; /*!< What it rides, 0 on foot */
    };

    void start_restricted(Proxy& p, time_point const&)
    {
        if(!p.subsystem<NetworkState>().clock_authority)
            return;
        u64 const tick = p.subsystem<TickClock>().tick();
        if(tick == m_rated_tick)
            return;
        m_rated_tick = tick;

        std::vector<viewer_t> viewers;
        for(auto player : p.select<PlayerInfo, PlayerCamera>())
        {
            auto const& [info, cam] = player.components();
            if(!info.is_remote())
                continue;
            viewers.push_back({
                .idx     = info.player_idx,
                .eye     = cam.camera.position,
                .forward = glm::transpose(Matf3(cam.camera.rotation)) * Vecf3{0.f, 0.f, -1.f},
                .vehicle = info.riding.vehicle,
            });
        }

        for(auto object : p.select<NetworkInfo, Model>())
        {
            auto& net = object.get<NetworkInfo>();
            net.relevance.clear();
            auto const* player  = p.get<PlayerInfo>(object.id());
            auto const* physics = p.get<ObjectPhysics>(object.id());
            if(!player && !(physics && physics->mass > 0.f && net.instance_id != 0))
                continue;
            /* A player is where its eye is; anything else where it stands */
            Vecf3 const at = player ? p.get<PlayerCamera>(object.id())->camera.position
                                    : object.get<Model>().position;
            f32 const weight = player                       ? player_weight
                               : physics && physics->drive ? vehicle_weight
                                                           : object_weight;
            for(auto const& viewer : viewers)
            {
                if(player && player->player_idx == viewer.idx)
                    continue;
                f32 const relevance =
                    viewer.vehicle != 0 && viewer.vehicle == object.id()
                        ? 1.f
                        : relevance_of(viewer, at);
                f32 const priority = weight * relevance;
                net.relevance.push_back({
                    .viewer        = viewer.idx,
                    .relevance     = relevance,
                    .priority      = priority,
                    .update_period = period_of(priority),
                });
            }
        }
    }

    /* 1 within reach or in view nearby, falling off with distance, half
     * as much out of view */
    static f32 relevance_of(viewer_t const& viewer, Vecf3 const& at)
    {
        Vecf3 const to = at - viewer.eye;
        f32 const   d  = glm::length(to);
        if(d <= near_range)
            return 1.f;
        bool const in_view = glm::dot(to / d, viewer.forward) >= view_cos;
        f32 const  falloff = in_view ? view_falloff : behind_falloff;
        f32 const  r       = 1.f / (1.f + (d / falloff) * (d / falloff));
        return in_view ? r : r * behind_scale;
    }

    /* Every tick at full priority, out to a second as it falls */
    static std::chrono::milliseconds period_of(f32 priority)
    {
        f32 const ticks = std::clamp(1.f / std::max(priority, 1e-3f), 1.f, max_period_ticks);
        return std::chrono::milliseconds(
            static_cast<libc_types::i64>(std::round(ticks) * TickClock::tick_seconds * 1000.f));
    }

    static constexpr f32 player_weight  = 1.f;
    static constexpr f32 vehicle_weight = .9f;
    static constexpr f32 object_weight  = .5f;
    static constexpr f32 near_range     = 8.f;  /* wu, always relevant */
    static constexpr f32 view_cos       = .5f;  /* cos 60: a wide field of view */
    static constexpr f32 view_falloff   = 30.f; /* wu, half relevance in view */
    static constexpr f32 behind_falloff = 15.f; /* wu, half relevance behind */
    static constexpr f32 behind_scale   = .5f;
    static constexpr f32 max_period_ticks = 30.f;

    u64 m_rated_tick{~0ull};
};

} // namespace

void alloc_network_relevance(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<NetworkRelevance>();
}
