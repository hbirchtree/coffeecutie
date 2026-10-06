#pragma once

/* A client driven by a script instead of a player: strafes back and forth
 * around a home spot and shoots at the first remote biped it can see. It
 * aims the way a player does, through mouse counts, unless told to cheat. */

#include "game.h"
#include "look.h"
#include "peer.h"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <optional>
#include <random>

namespace poc {

inline vec3 eye(HitModel const& model, BipedPose const& pose)
{
    vec3 const head =
        vec3(HitModel::root(pose) * glm::vec4(model.head_center, 1.f));
    /* Middle of the head, nudged forward so the ray starts outside it */
    return head + vec3(std::cos(pose.yaw), std::sin(pose.yaw), 0.f) * 0.15f;
}

/* Low-arc launch direction to land on `to` */
inline vec3 ballistic(vec3 const& from, vec3 const& to, float speed, float g)
{
    vec3 const  d    = to - from;
    vec3 const  h    = vec3(d.x, d.y, 0.f);
    float const x    = glm::length(h);
    float const v2   = speed * speed;
    float const disc = v2 * v2 - g * (g * x * x + 2.f * d.z * v2);
    float const angle =
        disc < 0.f || x <= 0.f ? glm::quarter_pi<float>()
                               : std::atan((v2 - std::sqrt(disc)) / (g * x));
    vec3 const hn = x > 0.f ? h / x : vec3(1.f, 0.f, 0.f);
    return glm::normalize(
        hn * std::cos(angle) + vec3(0.f, 0.f, std::sin(angle)));
}

enum class Cheat : std::uint8_t
{
    none,
    write_view, /* sloppy mouse, then writes the exact view to fire */
    silent,     /* sloppy mouse, fires at the head regardless of the view */
    snap,       /* looks around, snaps onto the head in one tick and fires */
    robotic,    /* turns at a fixed number of counts per tick */
};

char const* to_string(Cheat c);

struct BotSpec
{
    WeaponKind weapon{WeaponKind::pistol};
    float      strafe{1.5f};        /* +- wu along the strafe axis */
    float      strafe_speed{2.25f}; /* wu/s */
    int        interval{8};         /* ticks between shots */
    bool       fires{false};
    bool       aim_head{true};
    bool       at_ground{false}; /* shoot the ground in front instead */
    float      aim_error_deg{0.f};
    Cheat      cheat{Cheat::none};
};

struct Bot
{
    BotSpec         spec;
    HitModel const* model;
    Terrain const*  terrain;
    vec3            home;
    vec3            face; /* point it faces when idle */
    vec3            strafe_axis;
    unsigned        seed{1};

    tick_t                   start{-1};
    std::optional<BipedPose> last_seen;
    std::optional<View>      last_want;
    std::mt19937             rng{seed};

    BipedPose spawn() const
    {
        return {home, std::atan2(face.y - home.y, face.x - home.x), true};
    }

    /* Track `want`: close half the error and lead by half its motion, which
     * leaves the view on a steadily moving target at the end of the tick */
    LookInput human(View const& cur, View const& want, LookConfig const& c)
    {
        constexpr float gain = 0.5f;
        View feed{};
        if(last_want)
            feed = {
                wrap_angle(want.yaw - last_want->yaw),
                want.pitch - last_want->pitch};
        float const cap = glm::radians(12.f);
        std::normal_distribution<float> wobble(
            0.f, glm::radians(0.03f + spec.aim_error_deg * 0.1f));
        View step{
            std::clamp(
                feed.yaw * (1.f - gain) +
                    wrap_angle(want.yaw - cur.yaw) * gain,
                -cap,
                cap) + wobble(rng),
            std::clamp(
                feed.pitch * (1.f - gain) + (want.pitch - cur.pitch) * gain,
                -cap,
                cap) + wobble(rng),
        };
        return mouse_towards(cur, {cur.yaw + step.yaw, cur.pitch + step.pitch}, c);
    }

    void operator()(Peer<ClientGame>& peer)
    {
        auto&             game = peer.game;
        LookConfig const& look = game.look_config();
        if(!game.joined())
            return;
        if(start < 0)
            start = peer.tick;
        tick_t const t = peer.tick - start;

        BipedPose pose = spawn();
        if(spec.strafe > 0.f && spec.strafe_speed > 0.f)
        {
            /* Triangle wave */
            float const period = 4.f * spec.strafe / spec.strafe_speed;
            float const phase  = std::fmod(t * tick_seconds, period) / period;
            float const f =
                phase < 0.5f ? phase * 4.f - 1.f : 3.f - phase * 4.f;
            pose.pos += strafe_axis * (f * spec.strafe);
            if(auto g = terrain->ground(pose.pos + vec3(0.f, 0.f, 1.f), 3.f))
                pose.pos = *g;
        }

        InputFrame in{.position = pose.pos, .weapon = spec.weapon};

        auto remotes = game.remotes();
        std::optional<BipedPose> target;
        if(!remotes.empty())
            target = game.displayed(remotes.front());

        /* Where to point */
        vec3 const  from = game.eye();
        auto const& w    = weapon(spec.weapon);
        vec3        dir  = glm::normalize(face - pose.pos);
        if(target)
        {
            vec3 aim;
            if(spec.at_ground)
                aim = home + glm::normalize(face - home) * 3.f +
                      strafe_axis * std::sin(t * 0.37f);
            else if(spec.weapon == WeaponKind::grenade)
                aim = target->pos;
            else
                aim = vec3(
                    HitModel::root(*target) *
                    glm::vec4(
                        spec.aim_head ? model->head_center : model->body_center,
                        1.f));
            /* Lead projectiles with the motion we can see */
            if(!w.hitscan && last_seen && spec.weapon != WeaponKind::grenade)
                aim += (target->pos - last_seen->pos) * tick_rate *
                       (glm::length(aim - from) / w.speed);
            dir = spec.weapon == WeaponKind::grenade
                      ? ballistic(from, aim, w.speed, w.gravity)
                      : glm::normalize(aim - from);
        }
        last_seen = target;

        bool const warm = t >= static_cast<tick_t>(tick_rate * 1.5f);
        bool const fire = spec.fires && warm && (t % spec.interval) == 0 &&
                          game.alive() && target && target->alive;

        View const cur  = game.view();
        View const want = view_of(dir);
        View const sloppy{want.yaw + glm::radians(4.f), want.pitch};
        switch(spec.cheat)
        {
        case Cheat::none:
            in.look = human(cur, want, look);
            break;
        case Cheat::write_view:
            in.look = human(cur, sloppy, look);
            if(fire)
                in.set_view = want;
            break;
        case Cheat::silent:
            in.look = human(cur, sloppy, look);
            if(fire)
                in.fire_direction = dir;
            break;
        case Cheat::snap:
        {
            /* Wander off the target between shots, snap right before one */
            View const away{
                want.yaw + glm::radians(15.f) * std::sin(t * 0.21f),
                want.pitch + glm::radians(6.f)};
            if(((t + 1) % spec.interval) == 0)
                in.look = mouse_towards(cur, want, look);
            else if(!fire)
                in.look = human(cur, away, look);
            break;
        }
        case Cheat::robotic:
        {
            /* Fixed 30 counts per tick on each axis until within a step */
            LookInput full = mouse_towards(cur, want, look);
            auto clamp30   = [](std::int16_t v) {
                return static_cast<std::int16_t>(std::clamp<int>(v, -30, 30));
            };
            in.look = {clamp30(full.mouse_dx), clamp30(full.mouse_dy)};
            break;
        }
        }
        last_want = want;

        in.fire     = fire;
        in.pre_fire = in.look; /* trigger at the end of the tick */
        peer.bus.push(std::move(in));
    }
};

inline char const* to_string(Cheat c)
{
    switch(c)
    {
    case Cheat::none:
        return "none";
    case Cheat::write_view:
        return "write_view";
    case Cheat::silent:
        return "silent";
    case Cheat::snap:
        return "snap";
    case Cheat::robotic:
        return "robotic";
    }
    return "?";
}

} // namespace poc
