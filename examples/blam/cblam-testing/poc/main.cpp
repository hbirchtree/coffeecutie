/* BlamPoc: headless combat/netcode playground on a real map.
 *
 * One process runs a GNS server and two bot clients over loopback with GNS's
 * own lag/jitter/loss simulation. The bots shoot each other with hitscan and
 * projectile weapons; hits are traced against the map's collision BSP and
 * the multiplayer biped's coll tag. A report compares what each shooter saw
 * with what the server decided, per lag compensation policy. */

#include "session.h"
#include "world.h"

#include <blam/volta/blam_versions.h>
#include <coffee/application/application_start.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/files/cfiles.h>
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <url/url.h>

#include <chrono>
#include <thread>

using namespace poc;

namespace {

struct Options
{
    std::string   scenario{"all"};
    std::string   policy{"all"};
    NetSimulation sim{.rtt_ms = 150.f};
    GameConfig    game;
    float         duration{8.f};
    float         aim_error_deg{0.f};
    std::uint16_t port{27720};
    bool          verbose{false};
    bool          dump_hitmodel{false};
    unsigned      seed{1};
};

struct Scenario
{
    char const* name;
    char const* about;
    float       distance; /* shooter to target, wu */
    BotSpec     shooter;
    BotSpec     target;
    float       body{-1.f}; /* starting vitality, -1 = coll tag */
    float       shield{-1.f};
};

constexpr BotSpec still{.strafe = 0.f};

Scenario const scenarios[] = {
    {"duel",
     "pistol at the head of a strafing target",
     6.f,
     {.weapon = WeaponKind::pistol, .strafe = 0.f, .interval = 8, .fires = true},
     {}},
    {"sniper",
     "sniper at long range, strafing target",
     15.f,
     {.weapon = WeaponKind::sniper, .strafe = 0.f, .interval = 30, .fires = true},
     {}},
    {"terrain",
     "pistol at the ground; client vs. server impact points",
     6.f,
     {.weapon    = WeaponKind::pistol,
      .strafe    = 0.f,
      .interval  = 5,
      .fires     = true,
      .at_ground = true},
     still},
    {"plasma",
     "fast projectile at a strafing body",
     5.f,
     {.weapon   = WeaponKind::plasma,
      .strafe   = 0.f,
      .interval = 6,
      .fires    = true,
      .aim_head = false},
     {}},
    {"rocket",
     "slow projectile with splash, leading the target",
     6.f,
     {.weapon   = WeaponKind::rocket,
      .strafe   = 0.f,
      .interval = 40,
      .fires    = true,
      .aim_head = false},
     {}},
    {"grenade",
     "lobbed grenade at the target's feet",
     5.f,
     {.weapon   = WeaponKind::grenade,
      .strafe   = 0.f,
      .interval = 45,
      .fires    = true,
      .aim_head = false},
     {.strafe = 1.f, .strafe_speed = 1.5f}},
    {"cheat_view",
     "sloppy mouse, but writes the exact view to fire (memory aimbot)",
     6.f,
     {.weapon   = WeaponKind::pistol,
      .strafe   = 0.f,
      .interval = 8,
      .fires    = true,
      .cheat    = Cheat::write_view},
     {}},
    {"cheat_silent",
     "sloppy mouse, fire message aims at the head anyway (silent aim)",
     6.f,
     {.weapon   = WeaponKind::pistol,
      .strafe   = 0.f,
      .interval = 8,
      .fires    = true,
      .cheat    = Cheat::silent},
     {}},
    {"cheat_snap",
     "looks around, snaps onto the head in one tick, fires",
     6.f,
     {.weapon   = WeaponKind::pistol,
      .strafe   = 0.f,
      .interval = 12,
      .fires    = true,
      .cheat    = Cheat::snap},
     {}},
    {"cheat_robotic",
     "turns at a fixed count rate, perfect tracking",
     6.f,
     {.weapon   = WeaponKind::pistol,
      .strafe   = 0.f,
      .interval = 8,
      .fires    = true,
      .cheat    = Cheat::robotic},
     {}},
    {"trade",
     "both fire a lethal headshot on the same tick",
     8.f,
     {.weapon = WeaponKind::sniper, .strafe = 0.f, .interval = 45, .fires = true},
     {.weapon = WeaponKind::sniper, .strafe = 0.f, .interval = 45, .fires = true},
     50.f,
     0.f},
};

struct Layout
{
    vec3 shooter;
    vec3 target;
    vec3 strafe_axis;
};

/* Two flat-ish spots near a player start, with clear sight across the whole
 * strafe line */
std::optional<Layout> find_layout(World const& world, Scenario const& sc)
{
    auto const& terrain = world.terrain;
    auto const& model   = world.model;
    float const strafe  = sc.target.strafe;
    for(auto const& s : world.starts)
    {
        auto shooter = terrain.ground(s.pos + vec3(0.f, 0.f, 0.5f), 3.f);
        if(!shooter)
            continue;
        for(int d = 0; d < 16; d++)
        {
            float const a    = glm::two_pi<float>() * d / 16.f;
            vec3 const  dir  = vec3(std::cos(a), std::sin(a), 0.f);
            vec3 const  side = vec3(-dir.y, dir.x, 0.f);
            vec3 const  mid  = *shooter + dir * sc.distance;

            bool ok = true;
            vec3 target{};
            for(float f : {0.f, -1.f, 1.f, -0.5f, 0.5f})
            {
                auto g = terrain.ground(
                    mid + side * (strafe * f) + vec3(0.f, 0.f, 1.5f), 4.f);
                if(!g || std::abs(g->z - shooter->z) > 0.6f)
                {
                    ok = false;
                    break;
                }
                if(f == 0.f)
                    target = *g;
                BipedPose const sp{*shooter, a};
                BipedPose const tp{*g, a + glm::pi<float>()};
                vec3 const      from = eye(model, sp);
                for(vec3 aim : {model.head_center, model.body_center})
                    if(terrain.raycast(
                           from,
                           vec3(HitModel::root(tp) * glm::vec4(aim, 1.f))))
                        ok = false;
                if(!ok)
                    break;
            }
            if(ok)
                return Layout{*shooter, target, side};
        }
    }
    return std::nullopt;
}

void run(
    Scenario const& sc,
    Policy          policy,
    Options const&  opt,
    World const&    world,
    std::uint16_t   port)
{
    auto layout = find_layout(world, sc);
    if(!layout)
    {
        fmt::print("{}: no usable spot on this map\n", sc.name);
        return;
    }

    GameConfig config = opt.game;
    config.policy     = policy;
    Session session(world, config);

    fmt::print(
        "\n== {} ({}) | policy {} | rtt {} ms jitter {} ms loss {}% | interp "
        "{} ms ==\n",
        sc.name,
        sc.about,
        to_string(policy),
        opt.sim.rtt_ms,
        opt.sim.jitter_ms,
        opt.sim.loss_pct,
        config.interp_ms);

    fmt::print(
        "shooter at ({:.2f}, {:.2f}, {:.2f}), target at ({:.2f}, {:.2f}, "
        "{:.2f})\n",
        layout->shooter.x,
        layout->shooter.y,
        layout->shooter.z,
        layout->target.x,
        layout->target.y,
        layout->target.z);
    if(!session.host(port))
    {
        fmt::print("could not listen on {}\n", port);
        return;
    }
    if(sc.body >= 0.f)
        session.server()->game.set_vitality(sc.body, sc.shield);

    auto bot = [&](BotSpec spec, vec3 home, vec3 face, unsigned seed) {
        spec.aim_error_deg = opt.aim_error_deg;
        return Bot{
            .spec        = spec,
            .model       = &world.model,
            .terrain     = &world.terrain,
            .home        = home,
            .face        = face,
            .strafe_axis = layout->strafe_axis,
            .seed        = seed,
        };
    };
    /* Shooter connects first and gets the lower id */
    session.add_bot(bot(sc.shooter, layout->shooter, layout->target, opt.seed));
    session.add_bot(bot(sc.target, layout->target, layout->shooter, opt.seed + 1));

    using clock   = Session::clock;
    auto give_up  = clock::now() + std::chrono::seconds(10);
    auto deadline = clock::time_point::max();
    while(clock::now() < deadline)
    {
        session.update({});
        if(deadline == clock::time_point::max())
        {
            bool joined = true;
            for(auto& b : session.bots())
                joined = joined && b->peer.game.joined();
            if(joined)
                deadline = clock::now() +
                           std::chrono::milliseconds(
                               static_cast<int>(opt.duration * 1000));
            else if(clock::now() > give_up)
            {
                fmt::print("clients never joined\n");
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    fmt::print(
        "pings: shooter {:.0f} ms, target {:.0f} ms\n",
        session.bots()[0]->peer.game.ping(),
        session.bots()[1]->peer.game.ping());
    session.report().print(opt.verbose);
}

template<typename Ver>
int run_map(std::string const& path, Options const& opt)
{
    Coffee::Resource map_file(platform::url::constructors::MkUrl(path));
    if(!Coffee::FileMap(map_file))
    {
        fmt::print(stderr, "could not open {}\n", path);
        return 1;
    }
    auto map_r = blam::map_container<Ver>::from_bytes(map_file, Ver());
    if(map_r.has_error())
    {
        fmt::print(stderr, "could not parse {}\n", path);
        return 1;
    }
    blam::map_container<Ver> map = std::move(map_r.value());

    std::string error;
    auto        world = load_world(map, error);
    if(!world)
    {
        fmt::print(stderr, "{}\n", error);
        return 1;
    }
    fmt::print(
        "biped: {}\nterrain: {} player starts\n",
        world->biped_name,
        world->starts.size());
    if(opt.dump_hitmodel)
        world->model.dump();

    if(!GnsSession::init(error))
    {
        fmt::print(stderr, "GNS init failed: {}\n", error);
        return 1;
    }
    GnsSession::simulate(opt.sim);

    std::vector<Policy> policies;
    for(auto p : {Policy::server_now, Policy::rewind, Policy::shooter_right})
        if(opt.policy == "all" || opt.policy == to_string(p))
            policies.push_back(p);

    std::uint16_t port = opt.port;
    for(auto const& sc : scenarios)
    {
        if(opt.scenario != "all" && opt.scenario != sc.name)
            continue;
        for(auto p : policies)
        {
            run(sc, p, opt, *world, port++);
            /* Let the closed connections settle before the next session */
            for(int i = 0; i < 10; i++)
            {
                GnsSession::run_callbacks();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
    GnsSession::shutdown();
    return 0;
}

} // namespace

int poc_main()
{
    cxxopts::Options options(
        "BlamPoc", "Hit detection, projectiles and lag compensation playground");
    options.positional_help("[map file]");
    std::string scenario_help = "Scenario: all";
    for(auto const& sc : scenarios)
        scenario_help += fmt::format(", {}", sc.name);
    // clang-format off
    options.add_options()
        ("h,help", "Show help")
        ("halo-version", "pc, custom, xbox", cxxopts::value<std::string>()->default_value("pc"))
        ("scenario", scenario_help, cxxopts::value<std::string>()->default_value("all"))
        ("policy", "all, server_now, rewind, shooter_right", cxxopts::value<std::string>()->default_value("all"))
        ("rtt", "Simulated round trip, ms", cxxopts::value<float>()->default_value("150"))
        ("jitter", "Simulated jitter, ms", cxxopts::value<float>()->default_value("0"))
        ("loss", "Simulated packet loss, percent", cxxopts::value<float>()->default_value("0"))
        ("interp", "Client interpolation delay, ms", cxxopts::value<float>()->default_value("100"))
        ("max-rewind", "Server rewind cap, ms", cxxopts::value<float>()->default_value("1000"))
        ("rewind-slack", "Rewind allowed past ping + interp, ms", cxxopts::value<float>()->default_value("100"))
        ("tolerance", "Shooter claim tolerance, wu", cxxopts::value<float>()->default_value("0.15"))
        ("catchup", "Projectile catch-up cap, ms", cxxopts::value<float>()->default_value("150"))
        ("duration", "Seconds per run", cxxopts::value<float>()->default_value("8"))
        ("aim-error", "Bot aim error, degrees (1 sigma)", cxxopts::value<float>()->default_value("0"))
        ("port", "First UDP port", cxxopts::value<int>()->default_value("27720"))
        ("seed", "RNG seed", cxxopts::value<unsigned>()->default_value("1"))
        ("v,verbose", "Print every shot, projectile and damage event")
        ("dump-hitmodel", "Print the biped's collision nodes and materials")
        ("trust-aim", "Take shot directions from fire messages, not from look input")
        ("map", "Map file", cxxopts::value<std::string>());
    // clang-format on
    options.parse_positional({"map"});

    auto& args = Coffee::GetInitArgs();
    auto  res  = options.parse(args.size(), args.data());
    if(res.count("help") || !res.count("map"))
    {
        fmt::print("{}\n", options.help());
        return res.count("help") ? 0 : 1;
    }

    Options opt;
    opt.scenario             = res["scenario"].as<std::string>();
    opt.policy               = res["policy"].as<std::string>();
    opt.sim.rtt_ms           = res["rtt"].as<float>();
    opt.sim.jitter_ms        = res["jitter"].as<float>();
    opt.sim.loss_pct         = res["loss"].as<float>();
    opt.game.interp_ms       = res["interp"].as<float>();
    opt.game.max_rewind_ms   = res["max-rewind"].as<float>();
    opt.game.rewind_slack_ms = res["rewind-slack"].as<float>();
    opt.game.claim_tolerance = res["tolerance"].as<float>();
    opt.game.catchup_max_ms  = res["catchup"].as<float>();
    opt.duration             = res["duration"].as<float>();
    opt.aim_error_deg        = res["aim-error"].as<float>();
    opt.port    = static_cast<std::uint16_t>(res["port"].as<int>());
    opt.seed    = res["seed"].as<unsigned>();
    opt.verbose = res.count("verbose") > 0;
    opt.dump_hitmodel = res.count("dump-hitmodel") > 0;
    opt.game.validate_input = res.count("trust-aim") == 0;

    auto const path    = res["map"].as<std::string>();
    auto const version = res["halo-version"].as<std::string>();
    int        status  = 1;
    if(version == "pc")
        status = run_map<blam::pc_version_t>(path, opt);
    else if(version == "custom")
        status = run_map<blam::custom_version_t>(path, opt);
    else if(version == "xbox")
        status = run_map<blam::xbox_version_t>(path, opt);
    else
        fmt::print(stderr, "unknown halo version {}\n", version);

    fflush(stdout);
    std::quick_exit(status);
}

COFFEE_APPLICATION_MAIN_CUSTOM(poc_main, 0x1 | 0x2)
