#include "physics.h"

#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>

namespace poc {
namespace {

float lerp_angle(float a, float b, float t)
{
    float d = std::remainder(b - a, glm::two_pi<float>());
    return a + d * t;
}

float distance_to_segment(vec3 const& p, vec3 const& a, vec3 const& b)
{
    vec3 const  ab = b - a;
    float const l2 = glm::dot(ab, ab);
    float const t =
        l2 > 0.f ? std::clamp(glm::dot(p - a, ab) / l2, 0.f, 1.f) : 0.f;
    return glm::length(a + ab * t - p);
}

} // namespace

void Physics::History::record(tick_t tick, BipedPose const& pose)
{
    current = pose;
    ring[static_cast<size_t>(tick) % size] = {tick, pose};
}

std::optional<BipedPose> Physics::History::at(float tick) const
{
    tick_t const lo_t = static_cast<tick_t>(std::floor(tick));
    auto const&  lo   = ring[static_cast<size_t>(std::max(lo_t, 0)) % size];
    auto const&  hi   = ring[static_cast<size_t>(std::max(lo_t + 1, 0)) % size];
    if(lo.tick != lo_t)
        return std::nullopt;
    if(hi.tick != lo_t + 1)
        return lo.pose;
    float const t = tick - static_cast<float>(lo_t);
    BipedPose   out;
    out.pos   = glm::mix(lo.pose.pos, hi.pose.pos, t);
    out.yaw   = lerp_angle(lo.pose.yaw, hi.pose.yaw, t);
    out.alive = lo.pose.alive;
    return out;
}

std::optional<BipedPose> Physics::pose_of(biped_id id, float rewind) const
{
    auto it = m_bipeds.find(id);
    if(it == m_bipeds.end())
        return std::nullopt;
    if(rewind < 0.f || !m_keep_history)
        return it->second.current;
    return it->second.at(rewind);
}

void Physics::handle(PhysPose const& ev, Bus&)
{
    auto& h = m_bipeds[ev.id];
    if(m_keep_history)
        h.record(ev.tick, ev.pose);
    else
        h.current = ev.pose;
}

void Physics::handle(PhysRemoveBiped const& ev, Bus&)
{
    m_bipeds.erase(ev.id);
}

SurfaceHit Physics::trace(
    vec3 const& start, vec3 const& end, biped_id ignore, float rewind) const
{
    SurfaceHit out;
    if(auto t = m_terrain.raycast(start, end))
    {
        out.kind   = SurfaceHit::terrain;
        out.t      = t->t;
        out.normal = t->normal;
    }
    for(auto const& [id, h] : m_bipeds)
    {
        if(id == ignore)
            continue;
        auto pose = pose_of(id, rewind);
        if(!pose || !pose->alive)
            continue;
        auto hit = m_model.raycast(*pose, start, end);
        if(!hit || hit->t >= out.t)
            continue;
        auto const* mat = m_model.material(hit->material);
        out.kind        = SurfaceHit::biped;
        out.t           = hit->t;
        out.normal      = hit->normal;
        out.target      = id;
        out.node        = hit->node;
        out.local       = hit->local;
        out.head        = mat && mat->head;
        out.body_mult   = mat ? mat->body_mult : 1.f;
        out.shield_mult = mat ? mat->shield_mult : 1.f;
    }
    out.point = start + (end - start) * out.t;
    return out;
}

void Physics::handle(PhysRay const& ev, Bus& bus)
{
    PhysRayResult res{.query = ev.query};
    float         rewind = ev.rewind_tick;
    if(rewind >= 0.f && m_keep_history)
    {
        /* Clamp into what the history holds: up to the newest tick */
        rewind         = std::min(rewind, static_cast<float>(m_tick));
        res.rewound_to = rewind;
    } else
        rewind = -1.f;
    res.hit = trace(ev.origin, ev.end, ev.ignore, rewind);

    if(ev.claim.target != no_biped)
    {
        auto pose = pose_of(ev.claim.target, rewind);
        if(pose && pose->alive)
        {
            res.claim_valid = true;
            res.claim_world =
                m_model.node_to_world(*pose, ev.claim.node, ev.claim.local);
            res.claim_ray_distance =
                distance_to_segment(res.claim_world, ev.origin, ev.end);
            /* Pull up short of the surface so the test can't hit the
             * biped's own skin through terrain precision */
            vec3 const dir  = res.claim_world - ev.origin;
            float const len = glm::length(dir);
            if(len > 0.05f)
                res.claim_occluded = m_terrain
                                         .raycast(
                                             ev.origin,
                                             ev.origin + dir * ((len - 0.05f) / len))
                                         .has_value();
        }
    }
    bus.push(std::move(res));
}

void Physics::handle(PhysSpawnProjectile const& ev, Bus& bus)
{
    Projectile p{.spawn = ev, .pos = ev.origin, .vel = ev.velocity};
    /* Catch-up: whole ticks first, then the remainder */
    float left = ev.catchup;
    while(left > 0.f)
    {
        float const step = std::min(left, 1.f);
        left -= step;
        if(advance(p, step * tick_seconds, m_tick, bus))
            return;
    }
    m_projectiles.emplace(ev.id, p);
}

void Physics::handle(PhysRemoveProjectile const& ev, Bus&)
{
    m_projectiles.erase(ev.id);
}

void Physics::handle(PhysSplash const& ev, Bus& bus)
{
    PhysSplashResult res{.query = ev.query};
    for(auto const& [id, h] : m_bipeds)
    {
        if(!h.current.alive)
            continue;
        vec3 const  target = vec3(
            HitModel::root(h.current) * glm::vec4(m_model.body_center, 1.f));
        float const dist = glm::length(target - ev.center);
        if(dist > ev.radius)
            continue;
        /* Lift the origin off the surface it detonated on */
        vec3 const from = ev.center + glm::normalize(target - ev.center) * 0.05f;
        if(m_terrain.raycast(from, target))
            continue;
        res.victims.push_back({id, dist});
    }
    bus.push(std::move(res));
}

bool Physics::advance(Projectile& p, float dt, tick_t tick, Bus& bus)
{
    vec3 const g     = vec3(0.f, 0.f, -p.spawn.gravity);
    vec3 const next  = p.pos + p.vel * dt + 0.5f * g * dt * dt;
    p.vel           += g * dt;
    p.age           += dt;

    float const rewind =
        p.spawn.lag > 0.f ? static_cast<float>(tick) - p.spawn.lag : -1.f;
    SurfaceHit hit = trace(p.pos, next, p.spawn.owner, rewind);
    if(hit.kind != SurfaceHit::none)
    {
        bus.push(PhysProjectileImpact{
            .id           = p.spawn.id,
            .owner        = p.spawn.owner,
            .tick         = tick,
            .cosmetic     = p.spawn.cosmetic,
            .fuse_expired = false,
            .hit          = hit,
        });
        return true;
    }
    p.pos = next;
    if(p.age >= p.spawn.fuse)
    {
        SurfaceHit air;
        air.point = p.pos;
        bus.push(PhysProjectileImpact{
            .id           = p.spawn.id,
            .owner        = p.spawn.owner,
            .tick         = tick,
            .cosmetic     = p.spawn.cosmetic,
            .fuse_expired = true,
            .hit          = air,
        });
        return true;
    }
    return false;
}

void Physics::handle(PhysStep const& ev, Bus& bus)
{
    m_tick = ev.tick;
    for(auto it = m_projectiles.begin(); it != m_projectiles.end();)
    {
        if(advance(it->second, tick_seconds, ev.tick, bus))
            it = m_projectiles.erase(it);
        else
            ++it;
    }
}

} // namespace poc
