#include "app_combat.h"

#include "session.h"
#include "world.h"

#include "../components.h"
#include "../data.h"
#include "../map_marker.h"
#include "../selected_version.h"

#if defined(FEATURE_ENABLE_ImGui)
#include <imgui.h>
#endif

#include <fmt/format.h>

#include <cstdio>
#include <cstdlib>

namespace {

using namespace poc;
using type_safety::empty_list_t;
using type_safety::type_list_t;

using PocCombatManifest = compo::SubsystemManifest<
    type_list_t<DebugDraw, const PlayerCamera, const PlayerInfo>,
    type_list_t<DebugMarkers>,
    empty_list_t>;

struct PocCombat
    : compo::RestrictedSubsystem<PocCombat, PocCombatManifest>
{
    using type  = PocCombat;
    using Proxy = compo::proxy_of<PocCombatManifest>;

    PocCombat()
    {
        /* After the ImGui frame has begun (map browser is 2048) */
        priority = 2049;
        if(auto v = std::getenv("POC_COMBAT_AUTOHOST"))
            m_autohost = std::atoi(v);
        if(auto v = std::getenv("POC_COMBAT_AUTOFIRE"))
            m_autofire = std::atoi(v);
        m_address.resize(64);
        std::snprintf(m_address.data(), m_address.size(), "127.0.0.1:27720");
    }

    ~PocCombat()
    {
        stop();
    }

    bool main_thread_only() const override
    {
        return true;
    }

    template<typename Ver>
    void on_map(blam::map_container<Ver> const& map)
    {
        stop();
        std::string error;
        m_world = load_world(map, error);
        m_status = m_world ? "map: " + m_world->biped_name : error;
        /* Leaves a dummy plug time to place the camera first */
        m_autohost_at      = std::chrono::steady_clock::now() + std::chrono::seconds(3);
        m_pending_autohost = m_autohost >= 0;
    }

    void on_unload()
    {
        stop();
        m_world.reset();
        m_status = "no map";
    }

    void start_restricted(Proxy& p, compo::time_point const&)
    {
        find_camera(p);
        if(m_pending_autohost && m_eye &&
           std::chrono::steady_clock::now() >= m_autohost_at)
        {
            m_pending_autohost = false;
            m_bot_count        = m_autohost;
            host();
        }
        if(m_session)
            m_session->update([this](Peer<ClientGame>& peer) {
                local_input(peer);
            });
#if defined(FEATURE_ENABLE_ImGui)
        ui();
#endif
        draw(p);
    }

    void end_restricted(Proxy&, compo::time_point const&)
    {
    }

  private:
    /* ---- session control ---- */

    bool ensure_gns()
    {
        if(m_gns_ready)
            return true;
        std::string error;
        m_gns_ready = GnsSession::init(error);
        if(!m_gns_ready)
            m_status = "GNS: " + error;
        return m_gns_ready;
    }

    BipedPose player_pose() const
    {
        if(!m_eye || !m_world)
            return {};
        float const eye_height = m_world->model.head_center.z;
        return {
            *m_eye - vec3(0.f, 0.f, eye_height),
            std::atan2(m_forward.y, m_forward.x),
            true};
    }

    void host()
    {
        if(!m_world || !ensure_gns())
            return;
        stop();
        GnsSession::simulate(m_sim);
        m_config.policy = static_cast<Policy>(m_policy);
        m_session       = std::make_unique<Session>(*m_world, m_config);
        if(!m_session->host(static_cast<std::uint16_t>(m_port)))
        {
            m_status = fmt::format("could not listen on {}", m_port);
            m_session.reset();
            return;
        }
        m_session->join(fmt::format("127.0.0.1:{}", m_port), player_pose());
        spawn_bots();
        m_status = fmt::format(
            "hosting on {}, {} bots", m_port, m_session->bots().size());
        fmt::print(
            "poc: {} at ({:.2f}, {:.2f}, {:.2f})\n",
            m_status,
            m_eye->x,
            m_eye->y,
            m_eye->z);
    }

    void join()
    {
        if(!m_world || !ensure_gns())
            return;
        stop();
        GnsSession::simulate(m_sim);
        m_session = std::make_unique<Session>(*m_world, m_config);
        std::string const address(m_address.c_str());
        if(!m_session->join(address, player_pose()))
        {
            m_status = "could not connect to " + address;
            m_session.reset();
            return;
        }
        m_status = "joined " + address;
    }

    void stop()
    {
        if(m_session)
            m_session->report().print(false);
        m_session.reset();
    }

    /* In a row across the view, strafing sideways */
    void spawn_bots()
    {
        if(!m_session || !m_eye || !m_world)
            return;
        vec3 const fwd = glm::normalize(vec3(m_forward.x, m_forward.y, 0.f) + vec3(1e-6f));
        vec3 const side = vec3(-fwd.y, fwd.x, 0.f);
        for(int i = 0; i < m_bot_count; i++)
        {
            float const across = (i - (m_bot_count - 1) * 0.5f) * 2.5f;
            vec3 const  probe  = *m_eye + fwd * (m_bot_distance + i % 2) +
                               side * across + vec3(0.f, 0.f, 2.f);
            auto home = m_world->terrain.ground(probe, 8.f);
            if(!home)
                continue;
            fmt::print("poc: bot at ({:.2f}, {:.2f}, {:.2f})\n", home->x, home->y, home->z);
            BotSpec spec{
                .weapon       = static_cast<WeaponKind>(m_bot_weapon),
                .strafe       = m_bot_strafe ? 1.5f : 0.f,
                .interval     = m_bot_interval,
                .fires        = m_bot_fire,
                .aim_error_deg = m_bot_aim_error,
            };
            m_session->add_bot(Bot{
                .spec        = spec,
                .model       = &m_world->model,
                .terrain     = &m_world->terrain,
                .home        = *home,
                .face        = player_pose().pos,
                .strafe_axis = side,
                .seed        = static_cast<unsigned>(i + 1),
            });
        }
    }

    /* ---- local player ---- */

    template<typename P>
    void find_camera(P& p)
    {
        m_eye.reset();
        for(auto ent : p.template select<PlayerCamera>())
        {
            auto const* info = p.template get<PlayerInfo>(ent.id());
            if(!info || info->seat_idx != 0)
                continue;
            PlayerCamera const& cam = ent.template get<PlayerCamera>();
            m_eye     = vec3(cam.camera.position);
            m_forward = glm::transpose(glm::mat3(cam.rotation)) *
                        vec3(0.f, 0.f, -1.f);
            break;
        }
    }

    /* The camera turns on its own; send what turned it, as the counts our
     * look model needs to follow it. Raw device counts would go here. */
    void local_input(Peer<ClientGame>& peer)
    {
        if(!m_eye)
            return;
        InputFrame in{
            .position = player_pose().pos,
            .weapon   = static_cast<WeaponKind>(m_weapon),
        };
        in.look = mouse_towards(
            peer.game.view(), view_of(m_forward), peer.game.look_config());

        bool fire        = m_fire_requested;
        m_fire_requested = false;
        if(m_autofire > 0 && ++m_autofire_tick % m_autofire == 0)
            fire = true;
        in.fire     = fire;
        in.pre_fire = in.look;
        peer.bus.push(std::move(in));
    }

#if defined(FEATURE_ENABLE_ImGui)
    void ui()
    {
        auto& io = ImGui::GetIO();
        if(!io.WantCaptureMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_fire_requested = true;
        if(!io.WantCaptureKeyboard && ImGui::IsKeyPressed(ImGuiKey_F, false))
            m_fire_requested = true;

        if(!ImGui::Begin("Combat PoC"))
        {
            ImGui::End();
            return;
        }
        ImGui::TextUnformatted(m_status.c_str());
        ImGui::TextDisabled("Needs debug markers on to draw anything");

        static char const* policies[] = {"server_now", "rewind", "shooter_right"};
        static char const* weapons[] = {
            "pistol", "sniper", "plasma", "rocket", "grenade"};

        if(ImGui::CollapsingHeader("Network", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;
            changed |= ImGui::SliderFloat("RTT ms", &m_sim.rtt_ms, 0.f, 500.f);
            changed |= ImGui::SliderFloat("Jitter ms", &m_sim.jitter_ms, 0.f, 50.f);
            changed |= ImGui::SliderFloat("Loss %", &m_sim.loss_pct, 0.f, 20.f);
            ImGui::TextDisabled("GNS settings are process-wide");
            if(changed && m_gns_ready)
                GnsSession::simulate(m_sim);

            ImGui::Combo("Policy", &m_policy, policies, 3);
            ImGui::SliderFloat("Interp ms", &m_config.interp_ms, 0.f, 300.f);
            ImGui::SliderFloat("Max rewind ms", &m_config.max_rewind_ms, 0.f, 1000.f);
            ImGui::SliderFloat("Claim tolerance", &m_config.claim_tolerance, 0.f, 1.f);
            ImGui::SliderFloat("Catch-up ms", &m_config.catchup_max_ms, 0.f, 500.f);
            ImGui::TextDisabled("Policy and timings apply on host/join");
            ImGui::InputInt("Port", &m_port);

            if(ImGui::Button("Host"))
                host();
            ImGui::SameLine();
            ImGui::InputText("##address", m_address.data(), m_address.size());
            ImGui::SameLine();
            if(ImGui::Button("Join"))
                join();
            if(m_session && ImGui::Button("Stop"))
                stop();
        }

        if(ImGui::CollapsingHeader("Bots", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderInt("Count", &m_bot_count, 0, 6);
            ImGui::SliderFloat("Distance", &m_bot_distance, 2.f, 30.f);
            ImGui::Checkbox("Strafe", &m_bot_strafe);
            ImGui::SameLine();
            ImGui::Checkbox("Fire back", &m_bot_fire);
            ImGui::Combo("Bot weapon", &m_bot_weapon, weapons, 5);
            ImGui::SliderInt("Bot interval", &m_bot_interval, 5, 90);
            ImGui::SliderFloat("Bot aim error", &m_bot_aim_error, 0.f, 5.f);
            if(m_session && m_session->server() && ImGui::Button("Add bots in view"))
                spawn_bots();
        }

        if(ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Combo("Weapon", &m_weapon, weapons, 5);
            ImGui::TextDisabled("Fire: left mouse or F");
            ImGui::Checkbox("Draw what I see", &m_draw_client);
            ImGui::SameLine();
            ImGui::Checkbox("Draw server now", &m_draw_server);
            if(m_session && m_session->local())
            {
                auto& game = m_session->local()->game;
                ImGui::Text(
                    "id %u  ping %.0f ms  view tick %.1f  %s",
                    game.self(),
                    game.ping(),
                    game.view_tick(),
                    game.alive() ? "alive" : "dead");
            }
            if(m_session)
                for(auto const& [id, a] : m_session->report().aim)
                    ImGui::Text(
                        "aim %u: lost %u rebases %u/%u snaps %u robotic %u "
                        "silent %u no-trigger %u max %.1f deg/tick",
                        id,
                        a.lost,
                        a.suspicious_rebases,
                        a.rebases,
                        a.snap_fires,
                        a.constant_runs,
                        a.silent_aims,
                        a.no_trigger,
                        a.max_turn_deg);
        }

        if(m_session && ImGui::CollapsingHeader("Shots", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto const& shots = m_session->report().shots;
            biped_id const self = m_session->local() ? m_session->local()->game.self() : no_biped;
            int shown = 0;
            for(auto it = shots.rbegin(); it != shots.rend() && shown < 12; ++it)
            {
                if(it->shooter != self)
                    continue;
                shown++;
                char const* seen = it->client.kind == SurfaceHit::biped
                                       ? (it->client.head ? "HEAD" : "body")
                                   : it->client.kind == SurfaceHit::terrain ? "terrain"
                                                                            : "air";
                ImGui::Text(
                    "#%u %-7s saw %-7s -> %s%s (rewind %.1f, off %.3f)",
                    it->shot,
                    to_string(it->weapon),
                    seen,
                    it->resolved ? to_string(it->server.verdict) : "...",
                    it->resolved && it->server.verdict == wire::Verdict::hit
                        ? (it->server.head ? " HEAD" : " body")
                        : "",
                    it->server.rewind_ticks,
                    it->server.claim_distance);
            }
        }
        ImGui::End();
    }
#endif

    /* ---- drawing ---- */

    struct Marker
    {
        DebugMarkers::strip_slot_t slot;
        u64                        entity;
    };

    template<typename P>
    void put(P& p, DebugMarkers& dm, std::array<Vecf3, 16> const& verts, Vecf3 color)
    {
        if(m_used == m_markers.size())
        {
            auto slot = dm.acquire_strip(16);
            if(!slot.valid())
                return;
            compo::EntityRecipe recipe;
            recipe.components = {compo::type_hash_v<DebugDraw>()};
            auto ent          = p.create_entity(recipe);
            m_markers.push_back({slot, ent.id()});
        }
        auto& m = m_markers[m_used++];
        if(DebugDraw* draw = p.template get<DebugDraw>(m.entity))
        {
            draw->data.arrays = {
                .count  = m.slot.vert_count,
                .offset = m.slot.vert_offset,
            };
            draw->color_ptr = m.slot.color_idx;
        }
        dm.put_strip(m.slot.vert_offset, m.slot.color_idx, verts, color);
    }

    template<typename P>
    void draw_biped(P& p, DebugMarkers& dm, BipedPose const& pose, Vecf3 body, Vecf3 head)
    {
        auto const& model = m_world->model;
        mat4 const  root  = HitModel::root(pose);
        for(auto const& node : model.nodes)
        {
            if(node.bsps.empty())
                continue;
            mat4 const m    = root * node.bind;
            auto       box  = DebugMarkers::box_vertices(node.lo, node.hi);
            for(auto& v : box)
                v = Vecf3(m * glm::vec4(v, 1.f));
            auto const* mat = model.material(node.material);
            put(p, dm, box, mat && mat->head ? head : body);
        }
    }

    template<typename P>
    void draw_point(P& p, DebugMarkers& dm, vec3 at, float r, Vecf3 color)
    {
        put(p, dm, DebugMarkers::box_vertices(at - r, at + r), color);
    }

    void draw(Proxy& p)
    {
        DebugMarkers* dm;
        p.subsystem(dm);
        m_used = 0;
        if(dm->available() && m_session && m_world)
        {
            dm->map();
            if(m_draw_client && m_session->local())
            {
                auto& local = *m_session->local();
                local.physics.each_biped([&](biped_id, BipedPose const& pose) {
                    if(pose.alive)
                        draw_biped(p, *dm, pose, {0.2f, 1.f, 0.3f}, {1.f, 1.f, 0.2f});
                });
                local.physics.each_projectile([&](proj_id, vec3 const& at) {
                    draw_point(p, *dm, at, 0.04f, {1.f, 0.6f, 0.1f});
                });
            }
            if(m_draw_server && m_session->server())
            {
                biped_id const self =
                    m_session->local() ? m_session->local()->game.self() : no_biped;
                m_session->server()->physics.each_biped(
                    [&](biped_id id, BipedPose const& pose) {
                        if(pose.alive && id != self)
                            draw_biped(p, *dm, pose, {1.f, 0.2f, 0.2f}, {1.f, 0.f, 1.f});
                    });
                m_session->server()->physics.each_projectile([&](proj_id, vec3 const& at) {
                    draw_point(p, *dm, at, 0.03f, {1.f, 0.f, 0.f});
                });
            }

            /* Where our last shots landed: what we saw vs. what counted */
            auto const& shots = m_session->report().shots;
            biped_id const self =
                m_session->local() ? m_session->local()->game.self() : no_biped;
            int shown = 0;
            for(auto it = shots.rbegin(); it != shots.rend() && shown < 6; ++it)
            {
                if(it->shooter != self)
                    continue;
                shown++;
                draw_point(p, *dm, it->client.point, 0.02f, {1.f, 1.f, 1.f});
                if(it->resolved)
                    draw_point(
                        p,
                        *dm,
                        it->server.server_point,
                        0.03f,
                        it->server.verdict == wire::Verdict::hit
                            ? Vecf3{0.f, 1.f, 1.f}
                            : Vecf3{0.3f, 0.3f, 1.f});
            }
            dm->unmap();
        }

        /* Hide what wasn't written this frame */
        for(size_t i = m_used; i < m_markers.size(); i++)
            if(DebugDraw* draw = p.template get<DebugDraw>(m_markers[i].entity))
                draw->data.arrays.count = 0;
    }

    std::optional<World>     m_world;
    std::unique_ptr<Session> m_session;
    std::string              m_status{"no map"};
    bool                     m_gns_ready{false};

    std::optional<vec3> m_eye;
    vec3                m_forward{1.f, 0.f, 0.f};
    bool                m_fire_requested{false};

    NetSimulation m_sim{.rtt_ms = 150.f};
    GameConfig    m_config;
    int           m_policy{static_cast<int>(Policy::shooter_right)};
    int           m_port{27720};
    std::string   m_address;
    int           m_weapon{0};

    int   m_bot_count{2};
    float m_bot_distance{6.f};
    bool  m_bot_strafe{true};
    bool  m_bot_fire{false};
    int   m_bot_weapon{0};
    int   m_bot_interval{30};
    float m_bot_aim_error{0.5f};

    int  m_autohost{-1};
    int  m_autofire{0};
    int  m_autofire_tick{0};
    bool m_pending_autohost{false};
    std::chrono::steady_clock::time_point m_autohost_at{};

    bool                m_draw_client{true};
    bool                m_draw_server{true};
    std::vector<Marker> m_markers;
    size_t              m_used{0};
};

} // namespace

void alloc_poc_combat(compo::EntityContainer& e)
{
    auto& combat = e.register_subsystem_inplace<PocCombat>();
    auto& gbus   = e.subsystem_cast<GameEventBus>();
    gbus.addEventFunction<MapLoadEvent>(
        0, [&combat](GameEvent&, MapLoadEvent*) { combat.on_unload(); });
    gbus.addEventFunction<MapChangedEvent<halo_version>>(
        0, [&combat](GameEvent&, MapChangedEvent<halo_version>* changed) {
            combat.on_map(changed->container);
        });
}
