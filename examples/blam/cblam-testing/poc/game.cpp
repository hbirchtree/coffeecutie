#include "game.h"

#include <fmt/format.h>
#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>

namespace poc {

char const* to_string(Policy p)
{
    switch(p)
    {
    case Policy::server_now:
        return "server_now";
    case Policy::rewind:
        return "rewind";
    case Policy::shooter_right:
        return "shooter_right";
    }
    return "?";
}

char const* to_string(WeaponKind w)
{
    switch(w)
    {
    case WeaponKind::pistol:
        return "pistol";
    case WeaponKind::sniper:
        return "sniper";
    case WeaponKind::plasma:
        return "plasma";
    case WeaponKind::rocket:
        return "rocket";
    case WeaponKind::grenade:
        return "grenade";
    }
    return "?";
}

char const* to_string(wire::Verdict v)
{
    using enum wire::Verdict;
    switch(v)
    {
    case hit:
        return "hit";
    case miss:
        return "miss";
    case server_miss:
        return "server_miss";
    case claim_off_ray:
        return "claim_off_ray";
    case claim_occluded:
        return "claim_occluded";
    case claim_target_gone:
        return "claim_target_gone";
    case rewind_too_far:
        return "rewind_too_far";
    case shooter_dead:
        return "shooter_dead";
    case no_trigger:
        return "no_trigger";
    }
    return "?";
}

/* Loosely Halo-shaped numbers; speeds in world units (10 ft) per second */
Weapon const& weapon(WeaponKind kind)
{
    static Weapon const table[] = {
        /* pistol */ {true, 25.f, 3.f, 0.f, 0.f, 0.f, 0.f, 0.f, 100.f},
        /* sniper */ {true, 160.f, 3.f, 0.f, 0.f, 0.f, 0.f, 0.f, 200.f},
        /* plasma */ {false, 12.f, 1.f, 9.f, 0.f, 5.f, 0.f, 0.f, 0.f},
        /* rocket */ {false, 100.f, 1.f, 7.f, 0.f, 8.f, 1.25f, 100.f, 0.f},
        /* grenade */ {false, 0.f, 1.f, 6.f, gravity, 3.f, 1.8f, 120.f, 0.f},
    };
    return table[static_cast<size_t>(kind)];
}

/* ---- report ---- */

Report::Shot* Report::find_shot(biped_id shooter, std::uint32_t shot)
{
    for(auto& s : shots)
        if(s.shooter == shooter && s.shot == shot)
            return &s;
    return nullptr;
}

Report::Projectile* Report::find_projectile(biped_id owner, std::uint32_t ref)
{
    for(auto& p : projectiles)
        if(p.owner == owner && p.client_ref == ref)
            return &p;
    return nullptr;
}

namespace {

char const* hit_name(SurfaceHit const& h)
{
    switch(h.kind)
    {
    case SurfaceHit::none:
        return "air";
    case SurfaceHit::terrain:
        return "terrain";
    case SurfaceHit::biped:
        return h.head ? "HEAD" : "body";
    }
    return "?";
}

} // namespace

void Report::print(bool verbose) const
{
    std::map<wire::Verdict, int> verdicts;
    int    client_hits = 0, client_heads = 0, server_hits = 0, server_heads = 0;
    int    agree = 0, resolved = 0;
    double rewind_sum = 0, claim_sum = 0, terrain_sum = 0, terrain_max = 0;
    int    claim_n = 0, terrain_n = 0;
    for(auto const& s : shots)
    {
        bool const ch = s.client.kind == SurfaceHit::biped;
        client_hits += ch;
        client_heads += ch && s.client.head;
        if(verbose)
            fmt::print(
                "  shot {}#{:<3} {:<6} ping {:3.0f} view {:7.2f} client {:<7} "
                "server {:<17}{} rewind {:5.2f} claim_d {:.3f}\n",
                s.shooter,
                s.shot,
                to_string(s.weapon),
                s.ping,
                s.view_tick,
                hit_name(s.client),
                s.resolved ? to_string(s.server.verdict) : "(unresolved)",
                s.resolved && s.server.verdict == wire::Verdict::hit
                    ? (s.server.head ? " HEAD" : " body")
                    : "     ",
                s.server.rewind_ticks,
                s.server.claim_distance);
        if(!s.resolved)
            continue;
        resolved++;
        verdicts[s.server.verdict]++;
        bool const sh = s.server.verdict == wire::Verdict::hit;
        server_hits += sh;
        server_heads += sh && s.server.head;
        agree += ch == sh && (!ch || s.client.head == s.server.head);
        rewind_sum += s.server.rewind_ticks;
        if(s.client.kind == SurfaceHit::terrain && !sh)
        {
            double const e = glm::length(s.client.point - s.server.server_point);
            terrain_sum += e;
            terrain_max = std::max(terrain_max, e);
            terrain_n++;
        }
        if(s.server.claim_distance >= 0.f)
        {
            claim_sum += s.server.claim_distance;
            claim_n++;
        }
    }
    if(!shots.empty())
    {
        fmt::print(
            "hitscan: {} shots ({} resolved) | client hits {} (head {}) | "
            "server hits {} (head {}) | agree {}/{}\n",
            shots.size(),
            resolved,
            client_hits,
            client_heads,
            server_hits,
            server_heads,
            agree,
            resolved);
        fmt::print(
            "         avg rewind {:.2f} ticks | avg claim distance {:.4f} wu |",
            resolved ? rewind_sum / resolved : 0.,
            claim_n ? claim_sum / claim_n : 0.);
        for(auto [v, n] : verdicts)
            fmt::print(" {}={}", to_string(v), n);
        fmt::print("\n");
        if(terrain_n)
            fmt::print(
                "         terrain shots {} | client vs server point avg {:.4f} "
                "max {:.4f} wu\n",
                terrain_n,
                terrain_sum / terrain_n,
                terrain_max);
    }

    int    pred_bipeds = 0, server_bipeds = 0, both_done = 0, same_target = 0;
    double dist_sum = 0, dist_max = 0, div_sum = 0;
    int    div_n = 0;
    for(auto const& p : projectiles)
    {
        pred_bipeds += p.predicted_done && p.predicted.kind == SurfaceHit::biped;
        server_bipeds += p.server_done && p.server_target != no_biped;
        if(p.spawn_divergence >= 0.f)
        {
            div_sum += p.spawn_divergence;
            div_n++;
        }
        double d = -1;
        if(p.predicted_done && p.server_done)
        {
            both_done++;
            d = glm::length(p.predicted.point - p.server_point);
            dist_sum += d;
            dist_max = std::max(dist_max, d);
            same_target += p.predicted.target == p.server_target;
        }
        if(verbose)
            fmt::print(
                "  proj {}#{:<3} {:<7} ping {:3.0f} catchup {:4.2f} "
                "spawn_div {:6.3f} predicted {:<7} server {:<7} apart {:.3f}\n",
                p.owner,
                p.client_ref,
                to_string(p.weapon),
                p.ping,
                p.catchup,
                p.spawn_divergence,
                p.predicted_done ? hit_name(p.predicted) : "-",
                !p.server_done                  ? "-"
                : p.server_target == no_biped   ? "terrain"
                : p.server_head                 ? "HEAD"
                                                : "body",
                d);
    }
    if(!projectiles.empty())
        fmt::print(
            "projectiles: {} | predicted biped hits {} | server biped hits {} | "
            "same target {}/{} | impact apart avg {:.3f} max {:.3f} wu | "
            "spawn divergence avg {:.3f} wu\n",
            projectiles.size(),
            pred_bipeds,
            server_bipeds,
            same_target,
            both_done,
            both_done ? dist_sum / both_done : 0.,
            dist_max,
            div_n ? div_sum / div_n : 0.);

    for(auto const& [id, a] : aim)
        fmt::print(
            "aim {}: inputs {} lost {} rebases {} (suspicious {}) | max turn "
            "{:.1f} deg/tick | snap-fires {} | robotic runs {} | silent aim {} "
            "| no trigger {} | reported drift avg {:.3f} max {:.3f} deg\n",
            id,
            a.inputs,
            a.lost,
            a.rebases,
            a.suspicious_rebases,
            a.max_turn_deg,
            a.snap_fires,
            a.constant_runs,
            a.silent_aims,
            a.no_trigger,
            a.drift_n ? a.drift_sum_deg / a.drift_n : 0.,
            a.drift_max_deg);

    std::map<biped_id, int> kills;
    float                   total = 0.f;
    for(auto const& d : damage)
    {
        total += d.amount;
        if(d.killed)
            kills[d.source]++;
        if(verbose)
            fmt::print(
                "  damage {} -> {} {:<7} {:6.1f}{}{}{} (body {:.1f} shield {:.1f})\n",
                d.source,
                d.target,
                to_string(d.weapon),
                d.amount,
                d.head ? " head" : "",
                d.splash ? " splash" : "",
                d.killed ? " KILL" : "",
                d.body,
                d.shield);
    }
    fmt::print("damage: {} events, {:.1f} total |", damage.size(), total);
    for(auto [id, n] : kills)
        fmt::print(" biped {} kills {}", id, n);
    fmt::print("\n");
}

/* ---- server ---- */

void ServerGame::set_vitality(float body, float shield)
{
    m_start_body   = body;
    m_start_shield = shield;
}

ServerGame::Player* ServerGame::by_conn(conn_id conn)
{
    for(auto& [id, p] : m_players)
        if(p.conn == conn)
            return &p;
    return nullptr;
}

void ServerGame::handle(NetConnected const&, Bus&)
{
    /* Nothing until the client says hello */
}

void ServerGame::handle(NetDisconnected const& ev, Bus& bus)
{
    if(auto* p = by_conn(ev.conn))
    {
        bus.push(PhysRemoveBiped{p->id});
        m_players.erase(p->id);
    }
}

void ServerGame::handle(NetStats const& ev, Bus&)
{
    if(auto* p = by_conn(ev.conn))
        p->ping = ev.ping_ms;
}

void ServerGame::handle(NetReceived const& ev, Bus& bus)
{
    std::span<std::uint8_t const> bytes(ev.bytes);
    auto                          type = wire::type_of(bytes);
    if(!type)
        return;

    if(*type == wire::Type::hello)
    {
        auto msg = wire::decode<wire::Hello>(bytes);
        if(!msg || by_conn(ev.from))
            return;
        Player p{
            .conn  = ev.from,
            .id    = m_next_id++,
            .spawn = msg->spawn,
            .pose  = msg->spawn,
            .body  = m_start_body >= 0.f ? m_start_body : m_model.max_body,
            .shield =
                m_start_shield >= 0.f ? m_start_shield : m_model.max_shield,
            .view = msg->view,
        };
        m_players.emplace(p.id, p);
        bus.push(NetSend{
            .to    = ev.from,
            .bytes = wire::encode(wire::Welcome{.you = p.id, .tick = m_now}),
        });
        return;
    }

    if(Player* p = by_conn(ev.from))
        dispatch(*p, bytes, bus);
}

void ServerGame::dispatch(
    Player& p, std::span<std::uint8_t const> bytes, Bus& bus)
{
    /* A fire message can overtake the (unreliable) input for its tick;
     * hold it until that input is in */
    auto wait_for = [&](std::uint32_t seq) {
        if(!m_config.validate_input || seq <= p.seq)
            return false;
        if(p.waiting.size() < 32)
            p.waiting.emplace_back(bytes.begin(), bytes.end());
        return true;
    };

    switch(*wire::type_of(bytes))
    {
    case wire::Type::pose:
        if(auto msg = wire::decode<wire::Pose>(bytes))
            on_pose(p, *msg);
        break;
    case wire::Type::input:
        if(auto hdr = wire::decode<wire::InputHeader>(bytes))
        {
            auto entries = wire::decode_array<wire::InputEntry>(bytes, *hdr);
            on_input(p, entries, bus);
        }
        break;
    case wire::Type::fire_hitscan:
        if(auto msg = wire::decode<wire::FireHitscan>(bytes);
           msg && !wait_for(msg->seq))
            on_fire(p, *msg, bus);
        break;
    case wire::Type::fire_projectile:
        if(auto msg = wire::decode<wire::FireProjectile>(bytes);
           msg && !wait_for(msg->seq))
            on_fire(p, *msg, bus);
        break;
    default:
        break;
    }
}

void ServerGame::on_pose(Player& p, wire::Pose const& msg)
{
    if(p.death_tick < 0)
    {
        p.pose       = msg.pose;
        p.pose.alive = true;
    }
    if(!m_config.validate_input || msg.seq != p.seq)
        return;

    auto&       aim  = m_report.aim[p.id];
    float const diff = glm::degrees(angle_between(msg.view, p.view));
    if(p.need_rebase)
    {
        /* Input went missing; take the reported view, but a jump bigger
         * than the lost ticks could have turned is a free flick */
        aim.rebases++;
        if(diff > m_config.rebase_deg_per_tick * std::max(p.lost_run, 1))
            aim.suspicious_rebases++;
        p.view        = msg.view;
        p.need_rebase = false;
        p.lost_run    = 0;
        return;
    }
    aim.drift_max_deg = std::max(aim.drift_max_deg, diff);
    aim.drift_sum_deg += diff;
    aim.drift_n++;
}

void ServerGame::on_input(
    Player& p, std::span<wire::InputEntry const> entries, Bus& bus)
{
    if(!m_config.validate_input)
        return;
    auto&       aim  = m_report.aim[p.id];
    float const snap = m_config.snap_deg, settle = m_config.settle_deg;

    for(auto const& e : entries)
    {
        if(e.seq <= p.seq)
            continue; /* a resend we already have */
        if(p.seq != 0 && e.seq != p.seq + 1)
        {
            auto const lost = static_cast<int>(e.seq - p.seq - 1);
            aim.lost += lost;
            p.lost_run += lost;
            p.need_rebase = true;
        }

        View const before = p.view;
        p.view            = apply_look(before, e.look, m_config.look);
        p.ticks[e.seq % p.ticks.size()] = {
            e.seq, before, p.view, e.fire, e.pre_fire, m_now};

        float const turn = glm::degrees(angle_between(before, p.view));
        aim.max_turn_deg = std::max(aim.max_turn_deg, turn);

        /* Snap: a fast turn that stops dead, with a shot at the stop
         * (or on the fast tick itself). People overshoot and settle. */
        if(p.fired_fast && turn < settle)
            aim.snap_fires++;
        p.fired_fast = false;
        if(p.last_turn_deg > snap && turn < settle && e.fire)
            aim.snap_fires++;
        else if(turn > snap && e.fire)
            p.fired_fast = true;
        p.last_turn_deg = turn;

        /* The same non-zero mouse delta tick after tick */
        bool const moving = e.look.mouse_dx != 0 || e.look.mouse_dy != 0;
        if(moving && e.look == p.last_look)
        {
            if(++p.same_look == m_config.constant_run)
                aim.constant_runs++;
        } else
            p.same_look = 1;
        p.last_look = e.look;

        p.seq = e.seq;
        aim.inputs++;
    }

    /* Fire messages that were waiting on this input */
    auto waiting = std::move(p.waiting);
    p.waiting.clear();
    for(auto const& bytes : waiting)
        dispatch(p, bytes, bus);
}

std::optional<vec3> ServerGame::fired_along(
    Player& p, std::uint32_t seq, vec3 const& claimed)
{
    auto&       aim = m_report.aim[p.id];
    auto const& t   = p.ticks[seq % p.ticks.size()];
    if(t.seq != seq || !t.fire)
    {
        aim.no_trigger++;
        return std::nullopt;
    }
    vec3 const dir =
        forward(apply_look(t.before, t.pre_fire, m_config.look));
    if(glm::degrees(angle_between(dir, claimed)) > m_config.silent_deg)
        aim.silent_aims++;
    return dir;
}

void ServerGame::on_fire(Player& p, wire::FireHitscan const& msg, Bus& bus)
{
    auto reject = [&](wire::Verdict v) {
        wire::ShotResult r{.shot = msg.shot, .verdict = v, .target = no_biped};
        if(auto* rec = m_report.find_shot(p.id, msg.shot))
        {
            rec->server   = r;
            rec->resolved = true;
        }
        bus.push(NetSend{.to = p.conn, .bytes = wire::encode(r)});
    };

    if(p.death_tick >= 0)
    {
        /* Died after pulling the trigger (trade) is fine when the shooter
         * is right; the shot left roughly half a round trip ago */
        float const fired_at = m_now - ticks(p.ping / 2.f);
        if(m_config.policy != Policy::shooter_right ||
           p.death_tick < fired_at - 1.f)
            return reject(wire::Verdict::shooter_dead);
    }

    /* The rewind window runs from when the trigger input came in: the
     * (reliable) fire message may be a late retransmit */
    float reference = static_cast<float>(m_now);
    if(m_config.validate_input)
        if(auto const& t = p.ticks[msg.seq % p.ticks.size()]; t.seq == msg.seq)
            reference = static_cast<float>(t.arrived);

    float rewind = -1.f;
    if(m_config.policy != Policy::server_now)
    {
        float const allowed = std::min(
            ticks(p.ping + m_config.interp_ms + m_config.rewind_slack_ms),
            ticks(m_config.max_rewind_ms));
        float const oldest = reference - allowed;
        float       view   = msg.view_tick;
        if(view < oldest)
        {
            if(m_config.policy == Policy::shooter_right)
                return reject(wire::Verdict::rewind_too_far);
            view = oldest;
        }
        rewind = std::min(view, static_cast<float>(m_now));
    }

    /* Shoot along the view the input built, whatever the message says */
    wire::FireHitscan fire = msg;
    if(m_config.validate_input)
    {
        auto dir = fired_along(p, msg.seq, msg.direction);
        if(!dir)
            return reject(wire::Verdict::no_trigger);
        fire.direction = *dir;
    }

    auto const& w     = weapon(fire.weapon);
    auto        query = m_next_query++;
    m_shots[query]    = {p.id, p.conn, fire};
    bus.push(PhysRay{
        .query       = query,
        .origin      = fire.origin,
        .end         = fire.origin + glm::normalize(fire.direction) * w.range,
        .rewind_tick = rewind,
        .ignore      = p.id,
        .claim = m_config.policy == Policy::shooter_right ? fire.claim
                                                          : RayClaim{},
    });
}

void ServerGame::handle(PhysRayResult const& ev, Bus& bus)
{
    auto it = m_shots.find(ev.query);
    if(it == m_shots.end())
        return;
    PendingShot const s = it->second;
    m_shots.erase(it);

    using enum wire::Verdict;
    wire::ShotResult r{
        .shot           = s.fire.shot,
        .verdict        = miss,
        .target         = no_biped,
        .head           = false,
        .rewind_ticks   = ev.rewound_to >= 0.f ? m_now - ev.rewound_to : 0.f,
        .claim_distance = ev.claim_valid ? ev.claim_ray_distance : -1.f,
        .server_point   = ev.hit.point,
    };
    float      shield_mult = 1.f, body_mult = 1.f;
    bool const claimed     = s.fire.claim.target != no_biped;

    /* Shots from one tick are traced together; one may have killed this
     * shooter after the fire message was accepted */
    auto shooter = m_players.find(s.shooter);
    bool const shooter_died =
        shooter == m_players.end() || shooter->second.death_tick >= 0;

    if(shooter_died && m_config.policy != Policy::shooter_right)
        r.verdict = shooter_dead;
    else if(m_config.policy == Policy::shooter_right)
    {
        if(!claimed)
            r.verdict = miss;
        else if(!ev.claim_valid)
            r.verdict = claim_target_gone;
        else if(ev.claim_ray_distance > m_config.claim_tolerance)
            r.verdict = claim_off_ray;
        else if(ev.claim_occluded)
            r.verdict = claim_occluded;
        else
        {
            /* Head/body from the claimed node, not the client's word */
            auto node = s.fire.claim.node;
            HitMaterial const* mat =
                node >= 0 && static_cast<size_t>(node) < m_model.nodes.size()
                    ? m_model.material(m_model.nodes[node].material)
                    : nullptr;
            r.verdict      = hit;
            r.target       = s.fire.claim.target;
            r.head         = mat && mat->head;
            r.server_point = ev.claim_world;
            shield_mult    = mat ? mat->shield_mult : 1.f;
            body_mult      = mat ? mat->body_mult : 1.f;
        }
    } else if(ev.hit.kind == SurfaceHit::biped)
    {
        r.verdict   = hit;
        r.target    = ev.hit.target;
        r.head      = ev.hit.head;
        shield_mult = ev.hit.shield_mult;
        body_mult   = ev.hit.body_mult;
    } else
        r.verdict = claimed ? server_miss : miss;

    if(auto* rec = m_report.find_shot(s.shooter, s.fire.shot))
    {
        rec->server   = r;
        rec->resolved = true;
    }
    bus.push(NetSend{.to = s.conn, .bytes = wire::encode(r)});
    if(r.verdict == hit)
        apply_damage(
            r.target,
            s.shooter,
            s.fire.weapon,
            weapon(s.fire.weapon).damage,
            r.head,
            shield_mult,
            body_mult,
            false,
            bus);
}

void ServerGame::on_fire(Player& p, wire::FireProjectile const& msg, Bus& bus)
{
    auto const& w = weapon(msg.weapon);
    if(p.death_tick >= 0 || w.hitscan)
        return;

    /* Catch-up: the projectile left the shooter's barrel about one trip
     * ago, so it starts that far along its path */
    float const catchup =
        m_config.policy == Policy::server_now
            ? 0.f
            : std::clamp(
                  static_cast<float>(m_now) - msg.present_tick,
                  0.f,
                  ticks(m_config.catchup_max_ms));

    /* Shooter is right for projectiles too: trace bipeds as far behind the
     * projectile as the shooter saw them behind it. The server's copy is
     * `catchup` ticks old now, so it lines up with the shooter's view from
     * `catchup` ticks after view_tick. */
    float const lag =
        m_config.policy == Policy::shooter_right
            ? std::clamp(
                  static_cast<float>(m_now) - catchup - msg.view_tick,
                  0.f,
                  ticks(m_config.max_rewind_ms))
            : 0.f;

    vec3 vel = msg.velocity;
    if(m_config.validate_input)
    {
        auto dir = fired_along(p, msg.seq, msg.velocity);
        if(!dir)
            return;
        vel = *dir * w.speed;
    }
    float const speed = glm::length(vel);
    if(speed > w.speed * 1.01f)
        vel *= w.speed / speed;

    proj_id const id   = m_next_proj++;
    m_projectiles[id] = {p.id, msg.weapon, msg.client_ref};
    if(auto* rec = m_report.find_projectile(p.id, msg.client_ref))
    {
        rec->id      = id;
        rec->catchup = catchup;
    }
    bus.push(NetSend{
        .bytes = wire::encode(wire::ProjectileSpawn{
            .id         = id,
            .owner      = p.id,
            .client_ref = msg.client_ref,
            .weapon     = msg.weapon,
            .tick       = m_now,
            .catchup    = catchup,
            .origin     = msg.origin,
            .velocity   = vel,
        }),
    });
    bus.push(PhysSpawnProjectile{
        .id       = id,
        .owner    = p.id,
        .origin   = msg.origin,
        .velocity = vel,
        .gravity  = w.gravity,
        .fuse     = w.fuse,
        .catchup  = catchup,
        .lag      = lag,
    });
}

void ServerGame::handle(PhysProjectileImpact const& ev, Bus& bus)
{
    auto it = m_projectiles.find(ev.id);
    if(it == m_projectiles.end())
        return;
    LiveProjectile const lp = it->second;
    m_projectiles.erase(it);
    auto const& w = weapon(lp.weapon);

    bool const direct = ev.hit.kind == SurfaceHit::biped;
    if(auto* rec = m_report.find_projectile(lp.owner, lp.client_ref))
    {
        rec->server_done   = true;
        rec->server_point  = ev.hit.point;
        rec->server_target = direct ? ev.hit.target : no_biped;
        rec->server_head   = direct && ev.hit.head;
    }
    bus.push(NetSend{
        .bytes = wire::encode(wire::ProjectileImpact{
            .id     = ev.id,
            .tick   = ev.tick,
            .point  = ev.hit.point,
            .target = direct ? ev.hit.target : no_biped,
            .head   = direct && ev.hit.head,
        }),
    });

    if(direct && w.damage > 0.f)
        apply_damage(
            ev.hit.target,
            lp.owner,
            lp.weapon,
            w.damage,
            ev.hit.head,
            ev.hit.shield_mult,
            ev.hit.body_mult,
            false,
            bus);
    if(w.splash_radius > 0.f)
    {
        auto query        = m_next_query++;
        m_splashes[query] = {
            lp.owner, lp.weapon, direct ? ev.hit.target : no_biped};
        bus.push(PhysSplash{
            .query  = query,
            .center = ev.hit.point + ev.hit.normal * 0.05f,
            .radius = w.splash_radius,
        });
    }
}

void ServerGame::handle(PhysSplashResult const& ev, Bus& bus)
{
    auto it = m_splashes.find(ev.query);
    if(it == m_splashes.end())
        return;
    PendingSplash const s = it->second;
    m_splashes.erase(it);
    auto const& w = weapon(s.weapon);
    for(auto const& v : ev.victims)
    {
        if(v.id == s.direct)
            continue;
        float const amount =
            w.splash_damage * (1.f - v.distance / w.splash_radius);
        apply_damage(v.id, s.owner, s.weapon, amount, false, 1.f, 1.f, true, bus);
    }
}

void ServerGame::apply_damage(
    biped_id   target,
    biped_id   source,
    WeaponKind kind,
    float      amount,
    bool       head,
    float      shield_mult,
    float      body_mult,
    bool       splash,
    Bus&       bus)
{
    auto it = m_players.find(target);
    if(it == m_players.end() || it->second.death_tick >= 0 || amount <= 0.f)
        return;
    Player& p = it->second;

    /* Shields soak first; the head bonus only lands on bare flesh */
    float dealt = 0.f;
    if(p.shield > 0.f)
    {
        float const s = amount * shield_mult;
        if(s <= p.shield)
        {
            p.shield -= s;
            dealt = s;
        } else
        {
            float const over = (s - p.shield) / std::max(shield_mult, 1e-3f);
            dealt            = p.shield + over * body_mult;
            p.body -= over * body_mult;
            p.shield = 0.f;
        }
    } else
    {
        float const b = amount * body_mult *
                        (head && !splash ? weapon(kind).head_mult : 1.f);
        p.body -= b;
        dealt = b;
    }

    bool const killed = p.body <= 0.f;
    if(killed)
    {
        p.body       = 0.f;
        p.death_tick = m_now;
        p.pose.alive = false;
    }
    wire::Damage d{
        .target = target,
        .source = source,
        .weapon = kind,
        .head   = head,
        .splash = splash,
        .killed = killed,
        .amount = dealt,
        .body   = p.body,
        .shield = p.shield,
    };
    m_report.damage.push_back(d);
    bus.push(NetSend{.bytes = wire::encode(d)});
}

void ServerGame::tick(tick_t now, Bus& bus)
{
    m_now = now;
    std::vector<wire::SnapshotEntry> entries;
    for(auto& [id, p] : m_players)
    {
        if(p.death_tick >= 0 && now - p.death_tick >= m_config.respawn_ticks)
        {
            p.death_tick = -1;
            p.pose       = p.spawn;
            p.body   = m_start_body >= 0.f ? m_start_body : m_model.max_body;
            p.shield = m_start_shield >= 0.f ? m_start_shield : m_model.max_shield;
        }
        p.pose.alive = p.death_tick < 0;
        bus.push(PhysPose{id, now, p.pose});
        entries.push_back({id, p.pose, p.body, p.shield});
    }
    bus.push(NetSend{
        .reliable = false,
        .bytes    = wire::encode_array(
            wire::SnapshotHeader{
                .tick  = now,
                .count = static_cast<std::uint16_t>(entries.size())},
            std::span<wire::SnapshotEntry const>(entries)),
    });
}

/* ---- client ---- */

void ClientGame::handle(NetConnected const&, Bus& bus)
{
    bus.push(NetSend{.bytes = wire::encode(wire::Hello{.spawn = m_pose, .view = m_view})});
}

void ClientGame::handle(NetDisconnected const&, Bus& bus)
{
    for(auto const& [id, pose] : m_displayed)
        bus.push(PhysRemoveBiped{id});
    m_displayed.clear();
    m_snapshots.clear();
    m_self = no_biped;
}

void ClientGame::handle(NetStats const& ev, Bus&)
{
    m_ping = ev.ping_ms;
}

void ClientGame::on_snapshot(
    wire::SnapshotHeader const& hdr, std::vector<wire::SnapshotEntry> entries)
{
    /* Unreliable: drop stale or reordered ones */
    if(!m_snapshots.empty() && hdr.tick <= m_snapshots.back().tick)
        return;
    m_snapshots.push_back({hdr.tick, clock::now(), std::move(entries)});
    while(m_snapshots.size() > 64)
        m_snapshots.pop_front();
}

void ClientGame::handle(NetReceived const& ev, Bus& bus)
{
    std::span<std::uint8_t const> bytes(ev.bytes);
    auto                          type = wire::type_of(bytes);
    if(!type)
        return;
    switch(*type)
    {
    case wire::Type::welcome:
        if(auto msg = wire::decode<wire::Welcome>(bytes))
            m_self = msg->you;
        break;
    case wire::Type::snapshot:
        if(auto hdr = wire::decode<wire::SnapshotHeader>(bytes))
            on_snapshot(
                *hdr, wire::decode_array<wire::SnapshotEntry>(bytes, *hdr));
        break;
    case wire::Type::projectile_spawn:
    {
        auto msg = wire::decode<wire::ProjectileSpawn>(bytes);
        if(!msg)
            break;
        float const age_ticks =
            present_tick() - static_cast<float>(msg->tick) + msg->catchup;
        if(msg->owner == m_self)
        {
            /* Ours is already flying; how far apart are the two clocks? */
            auto it = m_predicted.find(msg->client_ref);
            if(it == m_predicted.end())
                break;
            float const predicted_ticks =
                std::chrono::duration<float>(clock::now() - it->second.fired)
                    .count() *
                tick_rate;
            if(auto* rec = m_report.find_projectile(m_self, msg->client_ref))
                rec->spawn_divergence = std::abs(age_ticks - predicted_ticks) *
                                        tick_seconds * it->second.speed;
            break;
        }
        auto const& w = weapon(msg->weapon);
        bus.push(PhysSpawnProjectile{
            .id       = msg->id,
            .owner    = msg->owner,
            .origin   = msg->origin,
            .velocity = msg->velocity,
            .gravity  = w.gravity,
            .fuse     = w.fuse,
            .catchup  = std::max(age_ticks, 0.f),
            .cosmetic = true,
        });
        break;
    }
    case wire::Type::projectile_impact:
        if(auto msg = wire::decode<wire::ProjectileImpact>(bytes))
            bus.push(PhysRemoveProjectile{msg->id});
        break;
    default:
        /* shot_result and damage are already in the report from the
         * server's side; health comes in with snapshots */
        break;
    }
}

void ClientGame::handle(PhysRayResult const& ev, Bus& bus)
{
    auto it = m_shots.find(ev.query);
    if(it == m_shots.end())
        return;
    PendingShot const s = it->second;
    m_shots.erase(it);

    wire::FireHitscan msg{
        .shot       = s.shot,
        .seq        = s.seq,
        .weapon     = s.weapon,
        .view_tick  = s.view_tick,
        .origin     = s.origin,
        .direction  = s.direction,
        .claim      = {},
        .claim_head = false,
    };
    if(ev.hit.kind == SurfaceHit::biped)
    {
        msg.claim      = {ev.hit.target, ev.hit.node, ev.hit.local};
        msg.claim_head = ev.hit.head;
    }
    m_report.shots.push_back({
        .shooter   = m_self,
        .shot      = s.shot,
        .weapon    = s.weapon,
        .ping      = m_ping,
        .view_tick = s.view_tick,
        .client    = ev.hit,
    });
    bus.push(NetSend{.bytes = wire::encode(msg)});
}

void ClientGame::handle(PhysProjectileImpact const& ev, Bus&)
{
    if(!(ev.id & predicted_bit))
        return;
    auto ref = ev.id & ~predicted_bit;
    m_predicted.erase(ref);
    if(auto* rec = m_report.find_projectile(m_self, ref))
    {
        rec->predicted_done = true;
        rec->predicted      = ev.hit;
    }
}

vec3 ClientGame::eye() const
{
    vec3 const head =
        vec3(HitModel::root(m_pose) * glm::vec4(m_model.head_center, 1.f));
    /* Nudged forward so the shot starts outside our own head */
    return head + forward(m_view) * 0.15f;
}

void ClientGame::handle(InputFrame const& ev, Bus& bus)
{
    View const before = m_view;
    m_view = ev.set_view ? *ev.set_view
                         : apply_look(m_view, ev.look, m_config.look);
    m_pose.pos = ev.position;
    m_pose.yaw = m_view.yaw;
    if(m_self == no_biped)
        return;

    /* This tick's input, sent along with the few before it */
    bool const fire = ev.fire && joined() && alive();
    m_recent.push_back({++m_seq, ev.look, ev.pre_fire, fire, ev.weapon});
    while(m_recent.size() > 4)
        m_recent.pop_front();
    std::vector<wire::InputEntry> const recent(m_recent.begin(), m_recent.end());
    bus.push(NetSend{
        .reliable = false,
        .bytes    = wire::encode_array(
            wire::InputHeader{.count = static_cast<std::uint8_t>(recent.size())},
            std::span<wire::InputEntry const>(recent)),
    });
    bus.push(NetSend{
        .reliable = false,
        .bytes    = wire::encode(
            wire::Pose{.pose = m_pose, .view = m_view, .seq = m_seq}),
    });
    if(!fire)
        return;

    View const at_trigger =
        ev.set_view ? m_view : apply_look(before, ev.pre_fire, m_config.look);
    vec3 const direction = ev.fire_direction ? glm::normalize(*ev.fire_direction)
                                             : forward(at_trigger);
    vec3 const  origin = eye();
    auto const& w      = weapon(ev.weapon);
    if(w.hitscan)
    {
        auto query     = m_next_query++;
        m_shots[query] = {
            m_next_shot++, m_seq, ev.weapon, m_view_tick, origin, direction};
        bus.push(PhysRay{
            .query  = query,
            .origin = origin,
            .end    = origin + direction * w.range,
            .ignore = m_self,
        });
        return;
    }

    auto const ref    = m_next_ref++;
    vec3 const vel    = direction * w.speed;
    m_predicted[ref]  = {ref, w.speed, clock::now()};
    m_report.projectiles.push_back({
        .owner      = m_self,
        .client_ref = ref,
        .weapon     = ev.weapon,
        .ping       = m_ping,
    });
    bus.push(PhysSpawnProjectile{
        .id       = predicted_bit | ref,
        .owner    = m_self,
        .origin   = origin,
        .velocity = vel,
        .gravity  = w.gravity,
        .fuse     = w.fuse,
        .cosmetic = true,
    });
    bus.push(NetSend{
        .bytes = wire::encode(wire::FireProjectile{
            .client_ref   = ref,
            .seq          = m_seq,
            .weapon       = ev.weapon,
            .present_tick = present_tick(),
            .view_tick    = m_view_tick,
            .origin       = origin,
            .velocity     = vel,
        }),
    });
}

bool ClientGame::alive() const
{
    if(m_snapshots.empty())
        return false;
    for(auto const& e : m_snapshots.back().entries)
        if(e.id == m_self)
            return e.pose.alive;
    return false;
}

std::vector<biped_id> ClientGame::remotes() const
{
    std::vector<biped_id> out;
    for(auto const& [id, pose] : m_displayed)
        out.push_back(id);
    return out;
}

std::optional<BipedPose> ClientGame::displayed(biped_id id) const
{
    auto it = m_displayed.find(id);
    if(it == m_displayed.end())
        return std::nullopt;
    return it->second;
}

float ClientGame::present_tick() const
{
    if(m_snapshots.empty())
        return 0.f;
    auto const& last = m_snapshots.back();
    float const elapsed =
        std::chrono::duration<float>(clock::now() - last.received).count() *
        tick_rate;
    return static_cast<float>(last.tick) + elapsed +
           m_ping / 2.f / 1000.f * tick_rate;
}

std::optional<BipedPose> ClientGame::interpolate(biped_id id, float tick) const
{
    auto find = [id](Snapshot const& s) -> wire::SnapshotEntry const* {
        for(auto const& e : s.entries)
            if(e.id == id)
                return &e;
        return nullptr;
    };
    Snapshot const* before = nullptr;
    Snapshot const* after  = nullptr;
    for(auto const& s : m_snapshots)
    {
        if(!find(s))
            continue;
        if(static_cast<float>(s.tick) <= tick)
            before = &s;
        else if(!after)
            after = &s;
    }
    if(!before && !after)
        return std::nullopt;
    if(!before || !after)
        return find(before ? *before : *after)->pose; /* hold, no extrapolation */
    auto const& a = find(*before)->pose;
    auto const& b = find(*after)->pose;
    float const t = (tick - before->tick) / float(after->tick - before->tick);
    BipedPose   out;
    out.pos   = glm::mix(a.pos, b.pos, t);
    out.yaw   = a.yaw + std::remainder(b.yaw - a.yaw, glm::two_pi<float>()) * t;
    out.alive = a.alive;
    return out;
}

void ClientGame::tick(Bus& bus)
{
    m_local_tick++;
    if(m_snapshots.empty())
        return;

    /* Playout clock in server ticks: runs at tick rate, slewed towards the
     * newest snapshot minus the interpolation delay */
    auto const& last = m_snapshots.back();
    float const elapsed =
        std::chrono::duration<float>(clock::now() - last.received).count() *
        tick_rate;
    float const target = static_cast<float>(last.tick) + elapsed -
                         m_config.interp_ms / 1000.f * tick_rate;
    if(m_view_tick < 0.f || std::abs(target - (m_view_tick + 1.f)) > 5.f)
        m_view_tick = target;
    else
        m_view_tick += 1.f + (target - (m_view_tick + 1.f)) * 0.1f;

    std::map<biped_id, BipedPose> shown;
    for(auto const& e : last.entries)
    {
        if(e.id == m_self)
            continue;
        if(auto pose = interpolate(e.id, m_view_tick))
        {
            shown[e.id] = *pose;
            bus.push(PhysPose{e.id, m_local_tick, *pose});
        }
    }
    for(auto const& [id, pose] : m_displayed)
        if(!shown.contains(id))
            bus.push(PhysRemoveBiped{id});
    m_displayed = std::move(shown);
}

} // namespace poc
