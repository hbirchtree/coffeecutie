#pragma once

/* A BlamGraphics client over the broadband adapter: plain UDP to a
 * BlamGCGateway, which holds the GameNetworkingSockets connection to the
 * server. Speaks just enough of the protocol to join, follow the server's
 * objects and players, and send our camera back. Messages are BlamGraphics'
 * little-endian structs, (de)coded field by field. */

#include <gc_gateway/protocol.h>
#include <network/network_events.h>

#include <coffee/core/CDebug>

#include <blam/common/endian.h>
#include <blam/common/tag_index.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <network.h>

#include <cstring>
#include <deque>

#include <functional>
#include <string>
#include <utility>

namespace gx_net {

using blam::from_le;
using blam::to_le;
using Coffee::cDebug;

/* BlamGraphics' own message layouts; they're little-endian, so fields are
 * read and written one by one at their offsets */
namespace nm = net_messages;
using msg    = nm::MessageBase;

constexpr u32 message_base_size = sizeof(nm::MessageBase);

using typing::vector_types::Quatf;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf4;

/* Same as BlamGraphics' MapFingerprint, which the server checks */
struct fingerprint_t
{
    u32 tag_hash[2]{};
    u32 tag_count{0};
    u32 base_tag{0};
};

struct object_sync_t
{
    u32       net_id;
    glm::vec3 position;
    glm::quat rotation;
};

struct camera_sync_t
{
    u32       player_idx;
    glm::vec3 position; /* the eye, Halo world space */
    glm::vec3 forward;
};

struct entity_spawn_t
{
    u32               net_id;
    blam::tag_index<> tag_id;
    glm::vec3 position;
    glm::quat rotation;
};

/* Combat, as the server reports it: tag handles stay as sent (little-endian
 * like the map's own), so they compare with the map's directly */
struct equipment_t
{
    u32               player_idx;
    blam::tag_index<> held, slung;
};

struct shot_t
{
    u32               player_idx;
    glm::vec3         position, direction;
    blam::tag_index<> weapon; // the grenade's proj when thrown
    bool              thrown;
};

struct hit_t
{
    u32               shooter_player;
    blam::tag_index<> projectile;
    glm::vec3         point;
};

struct handlers_t
{
    std::function<void(u32 player_idx)>          joined;
    std::function<void(object_sync_t const&)>    object;
    std::function<void(camera_sync_t const&)>    camera;
    std::function<void(entity_spawn_t const&)>   spawn;
    std::function<void(glm::vec3 const&, float)> player_spawn;
    /* The server moving our own camera, e.g. onto a spawn point */
    std::function<void(camera_sync_t const&)> teleport;
    std::function<void(equipment_t const&)>   equipment;
    std::function<void(shot_t const&)>        shot;
    std::function<void(hit_t const&)>         hit;
    std::function<void(u32 net_id)>           despawn;
};

/* BlamGraphics cameras rotate a Y-up basis; this maps it onto Halo's world */
inline glm::mat3 const camera_basis{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}};

inline glm::quat camera_rotation(glm::vec3 const& forward)
{
    glm::vec3 const d  = glm::transpose(camera_basis) * forward;
    glm::vec3 const up = glm::transpose(camera_basis) * glm::vec3{0, 0, 1};
    return glm::quat_cast(glm::mat3(glm::lookAt(glm::vec3{0}, d, up)));
}

inline glm::vec3 camera_forward(glm::quat const& q)
{
    return camera_basis *
           (glm::transpose(glm::mat3_cast(q)) * glm::vec3{0, 0, -1});
}

class client_t
{
  public:
    enum class state_t
    {
        offline,
        waiting_gateway,
        connecting,
        joined,
    };

    /* gateway "ip:port", server "ip:port" or empty for the gateway default */
    bool start(std::string const& gateway, std::string const& server)
    {
        char ip[16] = {}, mask[16] = {}, gw[16] = {};
        if(if_config(ip, mask, gw, true, 20) < 0)
        {
            cDebug("net: no broadband adapter or DHCP");
            return false;
        }
        cDebug("net: up as {}, gateway {}", ip, gateway);

        auto const colon = gateway.find(':');
        std::memset(&m_gateway, 0, sizeof(m_gateway));
        m_gateway.sin_family = AF_INET;
        m_gateway.sin_len    = sizeof(m_gateway);
        m_gateway.sin_port   = htons(
            colon == std::string::npos
                ? gc_gateway::default_port
                : static_cast<u16>(std::stoi(gateway.substr(colon + 1))));
        m_gateway.sin_addr.s_addr = inet_addr(gateway.substr(0, colon).c_str());

        m_socket = net_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(m_socket < 0)
            return false;
        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_len    = sizeof(local);
        local.sin_port   = htons(gc_gateway::default_port);
        if(net_bind(
               m_socket, reinterpret_cast<sockaddr*>(&local), sizeof(local)) <
           0)
            cDebug("net: bind failed, replies may not arrive");

        m_server = server;
        m_state  = state_t::waiting_gateway;
        return true;
    }

    state_t state() const
    {
        return m_state;
    }

    /* Drain received datagrams; every ~second, (re)greet the gateway. The
     * gateway answers every keepalive, so a quiet gateway means our session
     * is gone: start over rather than wait */
    void poll(fingerprint_t const& fingerprint, handlers_t const& on, u32 frame)
    {
        if(m_state == state_t::offline)
            return;
        m_frame = frame;
        u8 buf[gc_gateway::max_datagram + 64];
        for(;;)
        {
            sockaddr_in from{};
            socklen_t   from_len = sizeof(from);
            s32 const   got      = net_recvfrom(
                m_socket,
                buf,
                sizeof(buf),
                MSG_DONTWAIT,
                reinterpret_cast<sockaddr*>(&from),
                &from_len);
            if(got <= 0)
                break;
            m_rx++;
            m_last_rx = frame;
            handle(buf, static_cast<u32>(got), fingerprint, on);
        }
        if(m_ack_due)
        {
            gc_gateway::ack_t const ack{le(m_rx_seq)};
            send_frame(gc_gateway::ack, 0, &ack, sizeof(ack));
            m_ack_due = false;
        }
        if(frame % resend_frames == 0)
            for(auto const& p : m_pending)
                send_frame(
                    gc_gateway::reliable_message,
                    0,
                    p.payload.data(),
                    static_cast<u32>(p.payload.size()));
        if(m_state != state_t::waiting_gateway &&
           frame - m_last_rx > silence_frames)
        {
            cDebug("net: gateway silent, joining again");
            m_state = state_t::waiting_gateway;
        }
        if(frame % stats_frames == 0)
            cDebug(
                "net: rx={} tx={} tx_failed={} (last error {})",
                m_rx,
                m_tx,
                m_tx_failed,
                m_tx_error);
        if(frame % 50 == 0)
        {
            if(m_state == state_t::waiting_gateway)
                send_hello();
            else
                send_frame(gc_gateway::keepalive, 0, nullptr, 0);
        }
    }

    /* Our eye position and view direction, Halo world space */
    /* walk: the server simulates our body from send_input() */
    void send_camera(
        glm::vec3 const& eye, glm::vec3 const& forward, bool walk = false)
    {
        // The server only moves the players a connection owns, by index
        if(m_state != state_t::joined || m_player_idx == no_player)
            return;
        glm::quat const q = camera_rotation(forward);
        send_message(
            nm::CameraSync{
                .position      = le(Vecf4(eye, 0.f)),
                .rotation      = le(Quatf(q.w, q.x, q.y, q.z)),
                .target_player = le(m_player_idx),
                .fade          = le(1.f),
                .flags         = walk ? le(nm::CameraSync::flags_t(
                                    nm::CameraSync::physics |
                                    nm::CameraSync::simulated))
                                      : nm::CameraSync::none,
            },
            walk);
    }

    /* Held controls while walking; move is forward and right, -1..1 */
    void send_input(glm::vec2 const& move, glm::vec3 const& forward, bool jump)
    {
        if(m_state != state_t::joined || m_player_idx == no_player)
            return;
        glm::quat const q = camera_rotation(forward);
        send_message(
            nm::TickInput{
                .tick       = le(++m_input_tick),
                .view       = le(Quatf(q.w, q.x, q.y, q.z)),
                .move       = le(Vecf2(move.x, move.y)),
                .intents    = jump ? le(InputIntent::jump) : InputIntent::none,
                .player_idx = le(m_player_idx),
            },
            false);
    }

  private:
    /* Messages are little-endian on the wire: swap the fields used here,
     * in place, after copying a message out or before copying one in */
    template<typename T>
    static T le(T v)
    {
        if constexpr(std::is_enum_v<T>)
            return static_cast<T>(from_le(std::to_underlying(v)));
        else
            return from_le(v);
    }

    static Vecf4 le(Vecf4 v)
    {
        for(u32 i = 0; i < 4; i++)
            v[i] = from_le(v[i]);
        return v;
    }

    static Vecf2 le(Vecf2 v)
    {
        return Vecf2(from_le(v.x), from_le(v.y));
    }

    static Quatf le(Quatf q)
    {
        return Quatf(from_le(q.w), from_le(q.x), from_le(q.y), from_le(q.z));
    }

    template<typename T>
    static T read(u8 const* p)
    {
        T v;
        std::memcpy(&v, p, sizeof(T));
        return v;
    }

    static std::string_view text(char const* c, size_t max)
    {
        return {c, strnlen(c, max)};
    }

    template<typename T>
    void send_message(T&& value, bool reliable)
    {
        nm::Message<T> m(std::move(value));
        m.type       = le(m.type);
        m.num_values = le(m.num_values);
        if(!reliable)
            return send_frame(gc_gateway::message, 0, &m, sizeof(m));
        pending_t p{m_tx_seq++, {}};
        p.payload.resize(sizeof(gc_gateway::reliable_t) + sizeof(m));
        gc_gateway::reliable_t const rel{le(p.seq)};
        std::memcpy(p.payload.data(), &rel, sizeof(rel));
        std::memcpy(p.payload.data() + sizeof(rel), &m, sizeof(m));
        send_frame(
            gc_gateway::reliable_message,
            0,
            p.payload.data(),
            static_cast<u32>(p.payload.size()));
        m_pending.push_back(std::move(p));
    }

    void send_frame(u16 kind, u16 flags, void const* data, u32 size)
    {
        u8                   buf[gc_gateway::max_datagram];
        gc_gateway::header_t hdr{
            to_le(gc_gateway::magic), to_le(kind), to_le(flags)};
        std::memcpy(buf, &hdr, sizeof(hdr));
        if(size)
            std::memcpy(buf + sizeof(hdr), data, size);
        s32 const sent = net_sendto(
            m_socket,
            buf,
            sizeof(hdr) + size,
            0,
            reinterpret_cast<sockaddr*>(&m_gateway),
            sizeof(m_gateway));
        m_tx++;
        if(sent < 0)
        {
            m_tx_failed++;
            m_tx_error = sent;
        }
    }

    void send_hello()
    {
        m_last_rx = m_frame; // give the gateway time to answer
        // A hello starts the gateway's sequences over, and ours with it
        m_rx_seq = m_tx_seq = 0;
        m_pending.clear();
        m_player_idx = no_player;
        gc_gateway::hello_t hello{};
        std::strncpy(hello.server, m_server.c_str(), sizeof(hello.server) - 1);
        send_frame(gc_gateway::hello, 0, &hello, sizeof(hello));
    }

    /* The same handshake BlamGraphics clients do once their map is loaded */
    void join(fingerprint_t const& fp)
    {
        nm::PlayerJoin join{};
        std::strncpy(
            join.player_name.data.data(),
            "GameCube",
            sizeof(join.player_name) - 1);
        send_message(std::move(join), true);

        send_message(
            nm::MapVerify{
                .fingerprint =
                    {.tag_hash  = {le(fp.tag_hash[0]), le(fp.tag_hash[1])},
                     .tag_count = le(fp.tag_count),
                     .base_tag  = le(fp.base_tag)},
            },
            true);

        send_message(nm::GameLoadState{.progress = le(100u)}, true);
    }

    void handle(
        u8 const* data, u32 size, fingerprint_t const& fp, handlers_t const& on)
    {
        if(size < sizeof(gc_gateway::header_t) ||
           le(read<u32>(data)) != gc_gateway::magic)
            return;
        u16 kind;
        std::memcpy(&kind, data + 4, 2);
        kind              = from_le(kind);
        u8 const* payload = data + sizeof(gc_gateway::header_t);
        u32 const psize   = size - sizeof(gc_gateway::header_t);

        if(kind == gc_gateway::status && psize >= 4)
        {
            auto const st = le(read<u32>(payload));
            if(st == gc_gateway::status_t::connected &&
               m_state != state_t::joined)
            {
                cDebug("net: joined the server");
                m_state = state_t::joined;
                join(fp);
            } else if(st == gc_gateway::status_t::connecting)
                m_state = state_t::connecting;
            else if(st == gc_gateway::status_t::closed)
            {
                if(m_state == state_t::joined)
                    cDebug("net: lost the server");
                m_state = state_t::waiting_gateway;
            }
            return;
        }
        if(kind == gc_gateway::ack && psize >= sizeof(gc_gateway::ack_t))
        {
            u32 const next = le(read<gc_gateway::ack_t>(payload).next);
            while(!m_pending.empty() && m_pending.front().seq < next)
                m_pending.pop_front();
            return;
        }
        if(kind == gc_gateway::reliable_message &&
           psize >= sizeof(gc_gateway::reliable_t))
        {
            // Only the next one; anything else is answered with where we are
            m_ack_due     = true;
            u32 const seq = le(read<gc_gateway::reliable_t>(payload).seq);
            if(seq != m_rx_seq)
                return;
            m_rx_seq++;
            return handle_message(
                payload + sizeof(gc_gateway::reliable_t),
                psize - sizeof(gc_gateway::reliable_t),
                on);
        }
        if(kind == gc_gateway::message)
            handle_message(payload, psize, on);
    }

    void handle_message(u8 const* payload, u32 psize, handlers_t const& on)
    {
        if(psize < message_base_size)
            return;
        auto const header = read<nm::MessageBase>(payload);
        u32 const  count  = le(header.num_values);
        u8 const*  v      = payload + message_base_size;
        u32 const  vsize  = psize - message_base_size;
        auto       values = [&]<typename T>(auto&& fn) {
            for(u32 i = 0; i < count && (i + 1) * sizeof(T) <= vsize; i++)
                fn(read<T>(v + i * sizeof(T)));
        };
        switch(le(header.type))
        {
        case msg::GameJoin: {
            auto const m = read<nm::GameJoin>(v);
            cDebug(
                "net: server runs {}",
                text(m.map_name.data.data(), sizeof(m.map_name)));
            break;
        }
        case msg::GameLeave: {
            auto const m = read<nm::GameLeave>(v);
            cDebug(
                "net: server says {}",
                text(m.reason.data.data(), sizeof(m.reason)));
            // Kicked: joining again would only be kicked again
            m_state = state_t::offline;
            break;
        }
        case msg::PlayerJoinConfirm:
            m_player_idx = le(read<nm::PlayerJoinConfirm>(v).player_idx);
            cDebug("net: we are player {}", m_player_idx);
            if(on.joined)
                on.joined(m_player_idx);
            break;
        case msg::ObjectSync:
            values.template operator()<nm::ObjectSync>(
                [&](nm::ObjectSync const& m) {
                    if(on.object)
                        on.object(
                            {le(m.net_id),
                             glm::vec3(le(m.position)),
                             le(m.rotation)});
                });
            break;
        case msg::CameraSync:
            values.template operator()<nm::CameraSync>(
                [&](nm::CameraSync const& m) {
                    camera_sync_t const sync{
                        le(m.target_player),
                        glm::vec3(le(m.position)),
                        camera_forward(le(m.rotation))};
                    auto const& handler = sync.player_idx == m_player_idx
                                              ? on.teleport
                                              : on.camera;
                    if(handler)
                        handler(sync);
                });
            break;
        case msg::EntitySpawn:
            values.template operator()<nm::EntitySpawn>(
                [&](nm::EntitySpawn const& m) {
                    /* the tag handle stays as sent, little-endian like the
                     * map's own */
                    if(on.spawn)
                        on.spawn(
                            {le(m.tag.instance_id),
                             m.tag.tag_id,
                             glm::vec3(le(m.position)),
                             le(m.rotation)});
                });
            break;
        case msg::EquipmentSync:
            values.template operator()<nm::EquipmentSync>(
                [&](nm::EquipmentSync const& m) {
                    if(on.equipment)
                        on.equipment({le(m.player_idx), m.weapon, m.slung});
                });
            break;
        case msg::WeaponFire:
            values.template operator()<nm::WeaponFire>(
                [&](nm::WeaponFire const& m) {
                    if(on.shot)
                        on.shot(
                            {le(m.player_idx),
                             glm::vec3(le(m.position)),
                             glm::vec3(le(m.direction)),
                             m.weapon,
                             (le(std::to_underlying(m.flags)) &
                              nm::WeaponFire::thrown) != 0});
                });
            break;
        case msg::ObjectHit:
            values.template operator()<nm::ObjectHit>(
                [&](nm::ObjectHit const& m) {
                    if(on.hit)
                        on.hit(
                            {le(m.shooter_player),
                             m.projectile,
                             glm::vec3(le(m.point))});
                });
            break;
        case msg::ObjectDespawn:
            values.template operator()<nm::ObjectDespawn>(
                [&](nm::ObjectDespawn const& m) {
                    if(on.despawn)
                        on.despawn(le(m.net_id));
                });
            break;
        case msg::PlayerSpawn: {
            auto const m      = read<nm::PlayerSpawn>(v);
            u32 const  target = le(m.target_player);
            if(vsize >= sizeof(m) && on.player_spawn &&
               (target == m_player_idx || target == nm::CameraSync::self_id))
                on.player_spawn(glm::vec3(le(m.position)), le(m.facing));
            break;
        }
        default:
            break;
        }
    }

    s32         m_socket{-1};
    sockaddr_in m_gateway{};
    std::string m_server;
    state_t     m_state{state_t::offline};
    static constexpr u32 no_player = nm::CameraSync::self_id;
    u32                  m_player_idx{no_player};
    u64                  m_input_tick{0};

    /* At 50 FPS: 5s without hearing from the gateway, stats every minute */
    static constexpr u32 silence_frames = 250;
    static constexpr u32 stats_frames   = 3000;
    static constexpr u32 resend_frames  = gc_gateway::resend_ms / 20;

    struct pending_t
    {
        u32             seq;
        std::vector<u8> payload; // reliable_t + message
    };

    std::deque<pending_t> m_pending; // reliable, not yet acknowledged
    u32                   m_tx_seq{0}, m_rx_seq{0};
    bool                  m_ack_due{false};

    u32 m_frame{0}, m_last_rx{0};
    u32 m_rx{0}, m_tx{0}, m_tx_failed{0};
    s32 m_tx_error{0};
};

} // namespace gx_net
