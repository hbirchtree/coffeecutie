# Networking TODO

Outstanding work on the WebRTC gateway transport. For what the current scheme
does and what it is worth, see
[NETWORKING_ENCRYPTION.md](NETWORKING_ENCRYPTION.md).

## Future improvement

Strengthening the cryptography protecting client/server traffic.

The load-bearing item here — authenticating the GNS session against the key in
the join URL — has landed. The server issues itself a GNS certificate signed by
its Ed25519 key, and a client holding the matching public key pins it for the
connection, so a relay cannot present a certificate for a key it does not hold.
That is what took the gateway out of the trust boundary for the case it covers.

What it does not cover, and what the remaining items are about:

- **Clients without a key.** They still connect, and still cannot tell who they
  are talking to. That is deliberate — opting a server into a certificate must
  not lock out joins that were never given the key — but it means the guarantee
  is only as good as how the key reaches the client.
- **`hmac` mode.** The join URL carries a symmetric secret, so every client
  holding the link can mint what the server can. It is sound against the
  gateway, which never sees the fragment, but it cannot authenticate the server
  to another legitimate client. `ed25519` is the mode to use for untrusted
  clients; treat `hmac` as a shared-trust-group convenience.
- **The client is still anonymous to the server.** Nothing distributes client
  keys, so a malicious gateway could still connect *as a player*. A lesser harm
  than reading other players' traffic, but not nothing.

### Add freshness to the signed metadata

Small, and independent. The signed payload currently has no nonce,
timestamp or session binding, and the gateway caches it on a persistent
`/server-signal` connection, so a captured blob stays valid forever.

Have the client generate a random nonce, send it with its `/signal` request, and
have the server echo it into the signed payload. The client rejects a payload
whose nonce is not its own.

This costs a protocol change: the nonce has to reach the server, so `/signal`
and `/server-signal` both grow a field, and `publish_server_metadata`'s
publish-once-and-cache model becomes per-session signing.

Worth noting this does not by itself stop the gateway — it can forward a fresh
nonce and get a fresh signature. It closes replay by anyone else.

### Speculative: keep the rendezvous off the gateway when neither side is a browser

Unvalidated — recorded as a direction, not a plan.

The gateway has to carry the GNS handshake for a browser client, because a
browser cannot open a UDP socket and something must bridge. Two native peers
have no such constraint. If a native client and a native server used the gateway
only for discovery and NAT punch coordination, and then ran GNS's handshake
directly between themselves, the gateway would never see a cert or an ephemeral
key at all. Certificate pinning already stops it substituting its own keys;
this would stop it seeing the handshake in the first place, and would hold for
keyless clients too, which pinning does nothing for.

Most of the parts already exist:

- Both direct paths are implemented and in use: `ConnectByIPAddress`
  (`networking.cpp:1074`) against `CreateListenSocketIP` (`:1275`), plus
  `CreateListenSocketP2P` (`:1035`).
- The gateway already performs return-routability punching and observes each
  server's real UDP address, which is most of what address brokering needs.
- The architecture already anticipates migrating off the relay: `gns-connected`
  triggers `closeSessionRelay` once GNS lands a direct connection. This would
  move that migration to connect time rather than after a relayed start.

What would have to change:

- A gateway-registered server currently has **no listen socket at all** —
  connections arrive through the relay and are attached to a poll group
  individually (`networking.cpp:1355`). It would have to bind one.
- The registry would hand a native client an address to punch toward rather than
  a relayed session, so `/signal` grows a native-peer response shape.
- GNS P2P sockets and plain listen sockets do not interoperate, so whichever is
  chosen has to be used consistently on both ends. This is the detail most
  likely to sink the idea; check it first.

Two honest limits. It does nothing for the browser case, which is the reason the
gateway exists — this narrows exposure for one topology rather than fixing
anything. And the gateway still brokers discovery, so it could misdirect a
client toward a host of its choosing, and could replay signed metadata until the
freshness item above lands. The gain is narrowing what a hostile gateway can do
to misdirecting or denying, for one topology, which is worth having but is not
the same as removing it from the path.

### Not worth doing: binding the DTLS fingerprint

Recorded so it is not re-proposed. The obvious-looking fix — signing the answer
SDP's DTLS fingerprint so the client can pin it — does nothing here. The gateway
*is* the DTLS peer, legitimately and unavoidably, since browsers cannot speak
raw UDP. Pinning the fingerprint would verify that the gateway is the gateway.

This would be the right fix in a topology where the gateway only did signaling
and the DataChannel ran peer-to-peer. That is not this topology.

### Smaller, independent of the above

- **Surface the trust state.** GNS sets
  `k_nSteamNetworkConnectionInfoFlags_Unauthenticated` when it has no
  certificate for the peer. Nothing reads it. Logging it, or showing it in the
  UI, would at least make the current guarantee visible rather than implied.

## Known issues

- **Auto-generated key lands in the working directory.** With no
  `--gateway-auth-*` flag the server generates an Ed25519 key and logs a bare
  relative path, so it is written to the process CWD rather than the config
  directory `--gateway-register`'s help implies. This is where stray
  `webrtc_identity.pem` files at the repo root come from.
- **`WEBRTC_TRANSPORT.md` does not exist.** `tools/webrtc-gateway/main.go:8`
  and `:30` point readers at `examples/blam/cblam-testing/WEBRTC_TRANSPORT.md`
  for the topology overview and the "GNS-level signaling" section. The file is
  not in the repository.
- **A wasm host cannot sign.** The auth block sits inside the non-`COFFEE_WASM`
  branch of the `Networking` constructor, so a browser-hosted server has no
  keypair and issues no certificate. Clients reaching one therefore get the
  keyless, unauthenticated case above, whatever the join URL carries.
