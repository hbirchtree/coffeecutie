#pragma once

/* A BlamGraphics client over the broadband adapter: plain UDP to a
 * BlamGCGateway, which holds the GameNetworkingSockets connection to the
 * server. Speaks just enough of the protocol to join, follow the server's
 * objects and players, and send our camera back. Messages are BlamGraphics'
 * little-endian structs, (de)coded field by field. */

#include <gc_gateway/protocol.h>

#include <coffee/core/CDebug>

#include <blam/common/endian.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <network.h>

#include <cstring>
#include <functional>
#include <string>

namespace gx_net {

using blam::from_le;
using blam::to_le;
using Coffee::cDebug;

/* MessageBase::Type in BlamGraphics' networking.cpp */
enum message_type : u32
{
    msg_game_join           = 1,
    msg_game_load_state     = 2,
    msg_player_join         = 3,
    msg_player_join_confirm = 4,
    msg_game_leave          = 6,
    msg_camera_sync         = 8,
    msg_entity_spawn        = 9,
    msg_player_spawn        = 10,
    msg_object_sync         = 11,
    msg_map_verify          = 16,
};

constexpr u32 message_base_size = 16;
constexpr u32 camera_sync_size  = 48;
constexpr u32 object_sync_size  = 80;
constexpr u32 entity_spawn_size = 56;

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
    u32       net_id;
    u32       tag_id;
    glm::vec3 position;
    glm::quat rotation;
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

    /* Drain received datagrams; every ~second, (re)greet the gateway */
    void poll(fingerprint_t const& fingerprint, handlers_t const& on, u32 frame)
    {
        if(m_state == state_t::offline)
            return;
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
            handle(buf, static_cast<u32>(got), fingerprint, on);
        }
        if(frame % 50 == 0)
        {
            if(m_state == state_t::waiting_gateway)
                send_hello();
            else
                send_frame(gc_gateway::keepalive, 0, nullptr, 0);
        }
    }

    /* Our eye position and view direction, Halo world space */
    void send_camera(glm::vec3 const& eye, glm::vec3 const& forward)
    {
        if(m_state != state_t::joined)
            return;
        u8              msg[message_base_size + camera_sync_size] = {};
        glm::quat const q = camera_rotation(forward);
        put_header(msg, msg_camera_sync);
        put_f32(msg + 16, eye.x);
        put_f32(msg + 20, eye.y);
        put_f32(msg + 24, eye.z);
        put_f32(msg + 32, q.x);
        put_f32(msg + 36, q.y);
        put_f32(msg + 40, q.z);
        put_f32(msg + 44, q.w);
        put_u32(msg + 48, 0xFFFF); /* the server fills in our index */
        put_f32(msg + 52, 1.f);
        send_frame(gc_gateway::message, 0, msg, sizeof(msg));
    }

  private:
    static void put_u32(u8* p, u32 v)
    {
        v = to_le(v);
        std::memcpy(p, &v, 4);
    }

    static void put_f32(u8* p, f32 v)
    {
        put_u32(p, std::bit_cast<u32>(v));
    }

    static u32 get_u32(u8 const* p)
    {
        u32 v;
        std::memcpy(&v, p, 4);
        return from_le(v);
    }

    static f32 get_f32(u8 const* p)
    {
        return std::bit_cast<f32>(get_u32(p));
    }

    static std::string_view text(u8 const* p, size_t max)
    {
        auto const* c = reinterpret_cast<char const*>(p);
        return {c, strnlen(c, max)};
    }

    static glm::vec3 get_vec3(u8 const* p)
    {
        return {get_f32(p), get_f32(p + 4), get_f32(p + 8)};
    }

    static glm::quat get_quat(u8 const* p) /* glm stores x, y, z, w */
    {
        return glm::quat::wxyz(
            get_f32(p + 12), get_f32(p), get_f32(p + 4), get_f32(p + 8));
    }

    static void put_header(u8* msg, u32 type, u32 num_values = 1)
    {
        put_u32(msg, type);
        put_u32(msg + 4, 0);
        put_u32(msg + 8, 0);
        put_u32(msg + 12, num_values);
    }

    void send_frame(u16 kind, u16 flags, void const* data, u32 size)
    {
        u8                   buf[gc_gateway::max_datagram];
        gc_gateway::header_t hdr{
            to_le(gc_gateway::magic), to_le(kind), to_le(flags)};
        std::memcpy(buf, &hdr, sizeof(hdr));
        if(size)
            std::memcpy(buf + sizeof(hdr), data, size);
        net_sendto(
            m_socket,
            buf,
            sizeof(hdr) + size,
            0,
            reinterpret_cast<sockaddr*>(&m_gateway),
            sizeof(m_gateway));
    }

    void send_reliable(u8 const* msg, u32 size)
    {
        send_frame(gc_gateway::message, gc_gateway::reliable, msg, size);
    }

    void send_hello()
    {
        gc_gateway::hello_t hello{};
        std::strncpy(hello.server, m_server.c_str(), sizeof(hello.server) - 1);
        send_frame(gc_gateway::hello, 0, &hello, sizeof(hello));
    }

    /* The same handshake BlamGraphics clients do once their map is loaded */
    void join(fingerprint_t const& fp)
    {
        u8 join[message_base_size + 32] = {};
        put_header(join, msg_player_join);
        std::strncpy(reinterpret_cast<char*>(join + 16), "GameCube", 31);
        send_reliable(join, sizeof(join));

        u8 verify[message_base_size + 16] = {};
        put_header(verify, msg_map_verify);
        put_u32(verify + 16, fp.tag_hash[0]);
        put_u32(verify + 20, fp.tag_hash[1]);
        put_u32(verify + 24, fp.tag_count);
        put_u32(verify + 28, fp.base_tag);
        send_reliable(verify, sizeof(verify));

        u8 loaded[message_base_size + 4] = {};
        put_header(loaded, msg_game_load_state);
        put_u32(loaded + 16, 100);
        send_reliable(loaded, sizeof(loaded));
    }

    void handle(
        u8 const* data, u32 size, fingerprint_t const& fp, handlers_t const& on)
    {
        if(size < sizeof(gc_gateway::header_t) ||
           get_u32(data) != gc_gateway::magic)
            return;
        u16 kind;
        std::memcpy(&kind, data + 4, 2);
        kind              = from_le(kind);
        u8 const* payload = data + sizeof(gc_gateway::header_t);
        u32 const psize   = size - sizeof(gc_gateway::header_t);

        if(kind == gc_gateway::status && psize >= 4)
        {
            auto const st = get_u32(payload);
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
        if(kind != gc_gateway::message || psize < message_base_size)
            return;

        u32 const type  = get_u32(payload);
        u32 const count = get_u32(payload + 12);
        u8 const* v     = payload + message_base_size;
        u32 const vsize = psize - message_base_size;
        switch(type)
        {
        case msg_game_join:
            cDebug("net: server runs {}", text(v, 32));
            break;
        case msg_game_leave:
            cDebug("net: server says {}", text(v, 128));
            break;
        case msg_player_join_confirm:
            m_player_idx = get_u32(v);
            cDebug("net: we are player {}", m_player_idx);
            if(on.joined)
                on.joined(m_player_idx);
            break;
        case msg_object_sync:
            for(u32 i = 0; i < count && (i + 1) * object_sync_size <= vsize;
                i++)
            {
                u8 const* o = v + i * object_sync_size;
                if(on.object)
                    on.object(
                        {get_u32(o + 72), get_vec3(o + 24), get_quat(o + 8)});
            }
            break;
        case msg_camera_sync:
            for(u32 i = 0; i < count && (i + 1) * camera_sync_size <= vsize;
                i++)
            {
                u8 const*           c = v + i * camera_sync_size;
                camera_sync_t const sync{
                    get_u32(c + 32),
                    get_vec3(c),
                    camera_forward(get_quat(c + 16))};
                auto const& handler =
                    sync.player_idx == m_player_idx ? on.teleport : on.camera;
                if(handler)
                    handler(sync);
            }
            break;
        case msg_entity_spawn:
            for(u32 i = 0; i < count && (i + 1) * entity_spawn_size <= vsize;
                i++)
            {
                u8 const* e = v + i * entity_spawn_size;
                if(on.spawn)
                    on.spawn(
                        {get_u32(e + 40),
                         get_u32(e + 36),
                         get_vec3(e),
                         get_quat(e + 16)});
            }
            break;
        case msg_player_spawn:
            if(vsize >= 24 && on.player_spawn &&
               (get_u32(v + 16) == m_player_idx || get_u32(v + 16) == 0xFFFF))
                on.player_spawn(get_vec3(v), get_f32(v + 20));
            break;
        default:
            break;
        }
    }

    s32         m_socket{-1};
    sockaddr_in m_gateway{};
    std::string m_server;
    state_t     m_state{state_t::offline};
    u32         m_player_idx{0xFFFF};
};

} // namespace gx_net
