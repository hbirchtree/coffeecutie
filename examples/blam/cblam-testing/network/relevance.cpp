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
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;

using RelevanceManifest = compo::SubsystemManifest<
    type_safety::type_list_t<
        NetworkInfo,
        const PlayerInfo,
        const PlayerCamera,
        const Model,
        const ObjectPhysics,
        const BipedStatus,
        const BipedEquipment>,
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
        f32   magnification{1.f}; /*!< Its scope's, 1 unzoomed */
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
                .idx           = info.player_idx,
                .eye           = cam.camera.position,
                .forward       = glm::transpose(Matf3(cam.camera.rotation)) * Vecf3{0.f, 0.f, -1.f},
                .vehicle       = info.riding.vehicle,
                .magnification = magnification_of(p.get<BipedEquipment>(player.id())),
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
            /* Units fight and vehicles carry them; an item matters most
             * while it flies */
            bool const flying = physics && glm::dot(physics->linear_velocity, physics->linear_velocity) >
                                               flying_speed * flying_speed;
            bool const vehicle = physics && physics->drive;
            bool const unit    = player || p.get<BipedStatus>(object.id());
            f32 const weight   = player     ? player_weight
                                 : vehicle  ? vehicle_weight
                                 : unit     ? unit_weight
                                 : flying   ? flying_weight
                                            : object_weight;
            for(auto const& viewer : viewers)
            {
                if(player && player->player_idx == viewer.idx)
                    continue;
                bool const ridden = viewer.vehicle != 0 && viewer.vehicle == object.id();
                auto const seen   = sight_of(viewer, at, unit ? near_range : near_item_range);
                f32 scale         = 1.f;
                /* Shots and grenades unseen matter little; far vehicles less */
                if(flying && !seen.in_view)
                    scale = unseen_flying_scale;
                else if(vehicle && !ridden && seen.distance > view_falloff)
                    scale = far_vehicle_scale;
                f32 const relevance = ridden ? 1.f : seen.relevance;
                f32 const priority  = weight * relevance * scale;
                net.relevance.push_back({
                    .viewer        = viewer.idx,
                    .relevance     = relevance,
                    .priority      = priority,
                    .update_period = period_of(priority),
                });
            }
        }
    }

    struct sight_t
    {
        f32  relevance{1.f};
        f32  distance{0.f};
        bool in_view{true};
    };

    /* 1 within reach or in view nearby, falling off with distance, half
     * as much out of view; through a scope, its narrow view reaches as much
     * further as it magnifies, and what it doesn't frame matters less */
    static sight_t sight_of(viewer_t const& viewer, Vecf3 const& at, f32 near)
    {
        Vecf3 const to = at - viewer.eye;
        f32 const   d  = glm::length(to);
        if(d <= near)
            return {.relevance = 1.f, .distance = d, .in_view = true};
        f32 const  facing  = glm::dot(to / d, viewer.forward);
        bool const zoomed  = viewer.magnification > 1.f;
        bool const scoped  = zoomed && facing >= scope_cos;
        bool const in_view = facing >= view_cos;
        f32 const  falloff = scoped    ? view_falloff * viewer.magnification
                             : in_view ? view_falloff
                                       : behind_falloff;
        f32 r = 1.f / (1.f + (d / falloff) * (d / falloff));
        if(!in_view)
            r *= behind_scale;
        else if(zoomed && !scoped)
            r *= unframed_scale;
        return {.relevance = r, .distance = d, .in_view = in_view};
    }

    /* The held weapon's magnification at its zoom level, as the player's
     * own camera has it */
    static f32 magnification_of(BipedEquipment const* equip)
    {
        auto const* held = equip ? equip->held(equip->load_generation) : nullptr;
        if(!held || !held->weapon || equip->zoom == 0)
            return 1.f;
        i16 const   levels = std::max<i16>(held->weapon->zoom_levels, 1);
        Vecf2 const range  = held->weapon->zoom_magnification_range;
        return std::max(
            levels > 1 ? glm::mix(range.x, range.y, (equip->zoom - 1.f) / (levels - 1.f)) : range.x, 1.f);
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
    static constexpr f32 unit_weight    = .9f;
    static constexpr f32 flying_weight  = .8f;
    static constexpr f32 object_weight  = .5f;
    static constexpr f32 flying_speed   = 2.f;  /* wu/s: thrown, tumbling, not settling */
    static constexpr f32 scope_cos      = .985f; /* cos 10: what a scope frames */
    static constexpr f32 near_range     = 8.f;  /* wu, always relevant: players and units */
    static constexpr f32 near_item_range = 4.f; /* wu, the same for anything else */
    static constexpr f32 unseen_flying_scale = .3f; /* shots and grenades out of view */
    static constexpr f32 far_vehicle_scale   = .6f; /* vehicles past view_falloff, not ridden */
    static constexpr f32 unframed_scale      = .5f; /* in view but outside a scope's frame */
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
