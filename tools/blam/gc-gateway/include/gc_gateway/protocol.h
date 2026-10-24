#pragma once

/* UDP framing between a GameCube client and BlamGCGateway, which joins a
 * BlamGraphics server over GameNetworkingSockets on the console's behalf.
 * Every datagram is a header and a payload; all fields are little-endian, as
 * are the BlamGraphics messages carried inside. GNS does the reliability on
 * the server side of the gateway; on the console side, reliable messages
 * carry a sequence number and are resent until acknowledged, in order. */

#include <cstdint>

namespace gc_gateway {

constexpr std::uint32_t magic        = 0x57474347; /* "GCGW" */
constexpr std::uint16_t default_port = 27790;

/* Payloads are kept under a typical MTU; the gateway splits batched server
 * messages (MessageBase::Multiple) into datagrams that fit */
constexpr std::uint32_t max_datagram = 1400;

enum kind_t : std::uint16_t
{
    /* console -> gateway: hello_t, (re)connect to a server */
    hello = 1,
    /* gateway -> console: status_t, sent on every connection change and as
     * the answer to a hello or keepalive */
    status = 2,
    /* both ways: one BlamGraphics message (MessageBase + values) */
    message = 3,
    /* console -> gateway: no payload, keeps the session alive */
    keepalive = 4,
    /* both ways: reliable_t, then one BlamGraphics message. Delivered once
     * and in order: the receiver takes only the next sequence number */
    reliable_message = 5,
    /* both ways: ack_t, the next sequence number the sender of it expects */
    ack = 6,
};

enum flags_t : std::uint16_t
{
    none     = 0x0,
    reliable = 0x1, /* message: send it reliably on the GNS side */
};

/* Reliable messages in flight at once, and how often they're resent */
constexpr std::uint32_t reliable_window = 32;
constexpr std::uint32_t resend_ms       = 200;

struct header_t
{
    std::uint32_t magic;
    std::uint16_t kind;
    std::uint16_t flags;
};

struct hello_t
{
    char server[64]; /* "ip:port", empty = the gateway's default server */
};

struct status_t
{
    enum state_t : std::uint32_t
    {
        connecting = 0,
        connected  = 1,
        closed     = 2,
    };

    std::uint32_t state;
};

struct reliable_t
{
    std::uint32_t seq;
};

struct ack_t
{
    std::uint32_t next;
};

static_assert(sizeof(header_t) == 8);
static_assert(sizeof(hello_t) == 64);
static_assert(sizeof(status_t) == 4);

} // namespace gc_gateway
