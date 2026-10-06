#pragma once

/* Stand-in for a real physics backend. Consumes Phys* events, answers with
 * *Result/Impact events. Owns everything spatial: terrain, the biped hit
 * model, per-biped pose history (for rewinding) and projectiles. */

#include "events.h"
#include "hit_model.h"

#include <array>
#include <map>
#include <optional>

namespace poc {

class Physics
{
  public:
    Physics(Terrain const& terrain, HitModel const& model, bool keep_history)
        : m_terrain(terrain)
        , m_model(model)
        , m_keep_history(keep_history)
    {
    }

    void handle(PhysPose const& ev, Bus& bus);
    void handle(PhysRemoveBiped const& ev, Bus& bus);
    void handle(PhysRay const& ev, Bus& bus);
    void handle(PhysSpawnProjectile const& ev, Bus& bus);
    void handle(PhysRemoveProjectile const& ev, Bus& bus);
    void handle(PhysSplash const& ev, Bus& bus);
    void handle(PhysStep const& ev, Bus& bus);

    /* Read-only views for drawing; not part of the event flow */
    template<typename F>
    void each_biped(F&& f) const
    {
        for(auto const& [id, h] : m_bipeds)
            f(id, h.current);
    }

    template<typename F>
    void each_projectile(F&& f) const
    {
        for(auto const& [id, p] : m_projectiles)
            f(id, p.pos);
    }

  private:
    struct History
    {
        static constexpr size_t size = 128; /* ~4 s at 30 Hz */

        struct Entry
        {
            tick_t    tick{-1};
            BipedPose pose;
        };

        std::array<Entry, size> ring{};
        BipedPose               current;

        void                     record(tick_t tick, BipedPose const& pose);
        std::optional<BipedPose> at(float tick) const;
    };

    struct Projectile
    {
        PhysSpawnProjectile spawn;
        vec3                pos;
        vec3                vel;
        float               age{0.f};
    };

    /* Nearest of terrain and every biped (except ignore) along start->end */
    SurfaceHit trace(
        vec3 const& start, vec3 const& end, biped_id ignore, float rewind)
        const;
    std::optional<BipedPose> pose_of(biped_id id, float rewind) const;

    /* Advances one projectile; returns true if it went off */
    bool advance(Projectile& p, float dt, tick_t tick, Bus& bus);

    Terrain const&              m_terrain;
    HitModel const&             m_model;
    bool                        m_keep_history;
    std::map<biped_id, History> m_bipeds;
    std::map<proj_id, Projectile> m_projectiles;
    tick_t                      m_tick{0};
};

} // namespace poc
