# Networking encryption

How client/server game traffic is protected when a browser reaches a native
server through `webrtc-gateway`, and what the signed-metadata handshake in the
join URL does and does not prove.

This describes what the code does today. Known weaknesses and the work to
address them live in [NETWORKING_TODO.md](NETWORKING_TODO.md).

## Topology

The only topology this document covers is the one the gateway supports:

```
browser client  <--DataChannel-->  webrtc-gateway  <--UDP-->  native server
                (ICE/DTLS/SCTP)                    (plain datagrams)
```

The gateway is a *WebRTC-terminating relay*. It accepts a real DataChannel
connection and re-emits each message as a plain UDP datagram toward the
registered server, and vice versa (`tools/webrtc-gateway/main.go:1`). A browser
cannot open a raw UDP socket, so this bridging is structural, not incidental:
**the gateway is necessarily a DTLS endpoint, not a passive forwarder.**

It stays on the data path for the whole session. The `gns-connected` path that
retires the relay requires GameNetworkingSockets (GNS) to establish its own
direct P2P connection, and for a `transport=udp` server the client takes the
`m_webrtcDirectMode` branch, which never enables ICE or STUN
(`networking.cpp:1213`).

## Two encryption layers

| Layer | Protects | Endpoints | Configurable off? |
| --- | --- | --- | --- |
| DTLS (WebRTC) | DataChannel messages | browser ↔ **gateway** | no |
| GNS session crypto | GNS packet payloads | client ↔ **server** | no (see below) |

**DTLS** covers only the first hop. The gateway decrypts it — that is its job.

**GNS crypto** is the layer that actually spans client to server. GNS negotiates
an ephemeral elliptic-curve Diffie–Hellman key exchange and encrypts with
AES-256-GCM (`steamnetworkingsockets_connections.cpp:1243`, `:1281`, `:1664`).
The gateway relays those packets without inspecting them — it "never touches
whatever protocol rides inside those datagrams". Encryption cannot be disabled
by accident: `k_ESteamNetworkingConfig_Unencrypted` documents `0: Not allowed
(the default)`, and nothing in this codebase sets it.

So game traffic is encrypted on both hops, and the inner GNS layer is in
principle end-to-end.

## Server authentication: signed metadata

A web client joins with a single URL carrying gateway, server ID and key:

```
ws://gateway.example#serverId;auth=ed25519:<base64 public key>
ws://gateway.example#serverId;auth=hmac:<base64 shared secret>
```

The key lives in the URL **fragment**, which browsers never put on the wire, and
is parsed locally (`webrtc_identity.cpp:207`). The gateway never sees it.

Two interchangeable modes:

| Mode | Server holds | Client holds | Signature |
| --- | --- | --- | --- |
| `hmac` | shared secret | the same secret | HMAC-SHA256, hex |
| `ed25519` | private key PEM | public key | Ed25519, base64 |

For `ed25519` the server generates and persists its key on first use if no
`--gateway-auth-key` is given (`webrtc_identity.cpp:96`), so signing is on by
default for a registered server.

The server publishes metadata about itself to the gateway
(`networking.cpp:479`), signed over a canonical JSON encoding with recursively
sorted keys so both sides hash identical bytes (`canonical_metadata_json`):

```json
{"map":"bloodgulch","map_type":"multiplayer","player_count":3,
 "player_count_max":16,"identity":"ed:1f4c…"}
```

The `auth` object is appended after signing; verification strips it and
recomputes.

The client receives this in the gateway's `answer` and verifies it **against the
key from its own URL fragment** (`networking.cpp:1157`). Note that Ed25519
metadata also carries `auth.public_key`, but the verify path ignores it
(`webrtc_identity.cpp:346`) — trusting the key shipped alongside the signature
would make the check meaningless. Verification failure aborts the join.

## The identity check

A verified payload yields an `identity` string, derived from the server's key
material (`webrtc_identity.cpp:255`, `:294`) — a prefix plus 96 bits of digest,
sized to fit a GNS generic-string identity, which holds 31 characters.

The client installs it as the expected peer identity and, once GNS reports
`Connected`, compares it against the peer's reported identity, closing the
connection on mismatch (`networking.cpp:1505`).

**This comparison is not cryptographic.** In GNS open-source builds there is no
certificate authority, so peers present unsigned certs and identity is
self-asserted; `AllowRemoteUnsignedCert` returns `k_EUnsignedCert_Allow`
unconditionally, with upstream's own comment: *"We don't have a cert authority.
We probably ought to make this customizable"*
(`steamnetworkingsockets_connections.cpp:1806`). GNS exposes
`k_nSteamNetworkConnectionInfoFlags_Unauthenticated` to signal this; the app
does not read it.

So the identity check confirms the peer *claims* the expected identity. It is a
consistency check against accidental misrouting, not proof of identity.

## What this protects against

- **Passive network eavesdropping**, on either hop. Both are encrypted.
- **Joining the wrong server by accident** — a stale or mistyped server ID
  routes to a server whose signature will not verify.
- **A wrong or missing key.** A client with a key the server never signed with
  is refused before any game data flows, and a server that does not sign at all
  is refused when the client expects auth. This is what the `AUTH_MODE` path of
  the web-client test exercises.
- **Third parties substituting a server.** Anyone who is not the gateway cannot
  redirect a client to a server they control, because they cannot produce a
  valid signature. This covers the bootstrap: once the GNS handshake starts,
  another *registered* server that learned this session's ID could interfere
  without forging anything — see "Should be fixed" in
  [NETWORKING_TODO.md](NETWORKING_TODO.md).

## What this does not protect against

**The gateway itself.** It occupies every position needed to read and modify
game traffic:

1. It is a legitimate DTLS endpoint, so it holds the first hop's keys by design.
2. It relays GNS's own rendezvous handshake (`gns-rendezvous`, routed by session
   ID), so it sits on the key-exchange path too.
3. GNS accepts unsigned certs, so it can present its own cert asserting the
   server's identity string.

Together these let the gateway terminate both GNS sessions and see plaintext.
Nothing in the signed metadata prevents this: the signature covers server
description fields only, and does not bind the DTLS fingerprint, the GNS cert,
or the session.

**Replay.** The signed payload carries no nonce, timestamp or session binding,
and the gateway caches it across a persistent `/server-signal` connection. A
captured blob stays valid indefinitely. What a signature proves is *"a server
holding this key existed at some point"*, not *"this connection terminates
there"*.

### Is this end-to-end encryption?

No. Traffic is encrypted in transit on both hops, but E2EE requires the
endpoints to authenticate each other's keys, and here the component in the
middle is able to supply both sides' keys. The accurate description is
encryption *to the relay* on each hop, with an inner GNS layer that would be
end-to-end if its certs were authenticated. It is closer to the
SMTP-between-servers model than to Signal.

The practical consequence: **the gateway operator is inside the trust boundary.**
Run it yourself, or treat it as you would any host that terminates your TLS.

## Code map

| Concern | Location |
| --- | --- |
| URL fragment parsing | `webrtc_identity.cpp:207` |
| Canonical JSON, signing, verification | `webrtc_identity.cpp:250`–`:369` |
| Identity derivation | `webrtc_identity.cpp:255`, `:294` |
| Key auto-generation | `webrtc_identity.cpp:96` |
| Server metadata build/publish | `networking.cpp:479`, `:527` |
| Client verification | `networking.cpp:1157` |
| Identity comparison on connect | `networking.cpp:1505` |
| Relay and transport modes | `tools/webrtc-gateway/main.go:1`, `:117` |

Paths above are relative to this directory, except `steamnetworkingsockets_*`
and `k_ESteamNetworkingConfig_*` references, which are in the GameNetworkingSockets
dependency rather than this repository.
