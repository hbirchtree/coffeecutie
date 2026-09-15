# Networking TODO

Outstanding work on the WebRTC gateway transport. For what the current scheme
does and what it is worth, see
[NETWORKING_ENCRYPTION.md](NETWORKING_ENCRYPTION.md).

## Future improvement

Strengthening the cryptography protecting client/server traffic.

The problem to solve, in one line: **the gateway is currently inside the trust
boundary.** It terminates DTLS by design, it relays GNS's rendezvous handshake,
and GNS open-source builds accept unsigned certs — so it can terminate both GNS
sessions and read plaintext. The signed metadata does not prevent this, because
it describes the server rather than binding the connection.

Items are ordered by value. (1) is the real fix; (2) is cheap and independent;
(3) only matters if (1) proves impossible.

### What the gateway can do with the GNS rendezvous

The concrete shape of the problem, since "inside the trust boundary" is vague
about what is actually at stake. Item (1) below is the fix.

The rendezvous the gateway relays is not incidental traffic — it is the GNS
crypto handshake. `CMsgSteamNetworkingP2PRendezvous` carries `ConnectRequest`
and `ConnectOK`, each with a `cert` and a signed `crypt` block
(`steamnetworkingsockets_messages.proto:264`, `:277`), and the crypt block holds
the ephemeral Curve25519 public key, a nonce, the protocol version and the
cipher list.

**Reading it is not enough to decrypt.** Every value in there is public by
design. The Diffie–Hellman private key never leaves the endpoint — GNS wipes it
as soon as the shared secret is derived
(`steamnetworkingsockets_connections.cpp:1647`). A gateway that forwards
faithfully learns identities, the negotiated cipher, timings, packet sizes and
the connection graph. That is metadata, not content, and the AES-256-GCM stream
stays opaque to it.

**Rewriting it gives up everything.** This is the case that matters, because the
gateway is an active relay already — modifying costs it nothing. It runs two
handshakes instead of one, presenting each side its own cert and its own
ephemeral key and signing its own crypt block with its own cert key. The result
is internally consistent, and both endpoints accept it because
`AllowRemoteUnsignedCert()` returns `k_EUnsignedCert_Allow` unconditionally in
open-source builds. Two sessions, both keys held by the gateway, full plaintext
read *and* write, including injecting traffic neither side sent. The identity
check does not interfere: the forged cert simply carries the expected identity
string, which is `sha256(server public key)` truncated and therefore computable
by anyone holding the join URL.

So rendezvous access is exactly what converts the gateway from *relaying
ciphertext it cannot read* into *reading and writing plaintext*. Without it, the
DTLS hop still terminates at the gateway but the inner GNS layer is beyond
reach. That is why (1) is the load-bearing item: it does not remove the gateway
from the path, it makes the certs unforgeable so that being on the path stops
being enough.

**One property does hold.** Cipher downgrade is not available. A re-signing
attacker could rewrite the `ciphers` list, but with `Unencrypted = 0` — the
default, and nothing sets it — the local list contains only AES-256-GCM
(`:1243`), and negotiation walks the *local* list requiring membership in the
remote's (`:1606`). A peer offering NULL fails to intersect and the connection
ends with "Failed to negotiate mutually-agreeable cipher". Neither endpoint can
be talked down to plaintext on the wire, independently of the broken signature
chain.

### 1. Authenticate the GNS session against the key in the join URL

The inner GNS layer is the only one spanning client to server, and it already
does ephemeral ECDH with AES-256-GCM. It lacks a trust anchor — but we ship one.
The client holds the server's Ed25519 public key from the URL fragment before it
sends a byte.

Proposal: let the server's Ed25519 key act as a single-server certificate
authority. It issues itself a GNS cert binding its identity and connection key;
the client installs that key as its trusted CA and validates the peer cert
against it. This is GNS's own intended model — `SteamNetAuthenticationStatus_t`
describes readiness for "authenticated, encrypted communication" as needing
"the list of trusted CA certificates" plus "a valid certificate issued by a CA".

Effect: a relay cannot forge a cert for a key it does not hold, so this survives
the gateway seeing every byte. It would make the transport genuinely end-to-end
encrypted, and would make the identity-string truncation question moot, since
the cert would do the binding the identity string currently only gestures at.

**Why one anchor is enough.** The rest of the chain already exists and is sound.
GNS signs the session crypt block with the cert's key —
`CMsgSteamDatagramSessionCryptInfoSigned.signature` is documented as the
"Signature of encryption_key_data generated using the public key from the
CMsgSteamDatagramCertificate". So the ephemeral Curve25519 key is already bound
to the cert, and the session key is already bound to the ephemeral key. The only
unanchored link is the cert itself. Fix that one and the whole handshake becomes
authenticated, with no need to sign the rendezvous separately or to change the
GNS wire format. An authenticated key exchange is safe to relay through a party
you do not trust; that is the property being restored.

This also removes replay as a concern for the connection itself: each session's
signed crypt block carries a fresh ephemeral key, so a replayed block leaves the
attacker without the matching private half. (2) remains worth doing for the
freshness of the *metadata* — map, player counts — not for connection security.

**Two limits worth stating.** First, this authenticates the server to the
client, not the client to the server; clients stay anonymous, since nothing
distributes client keys. A malicious gateway could therefore still connect to
the server *as a player*. That is a lesser harm than reading other players'
traffic, but it is not nothing.

Second, it only works in `ed25519` mode. In `hmac` mode the join URL carries a
symmetric secret, so every client holding the link can produce anything the
server can. That is still sound against the gateway, which never sees the
fragment — but it means `hmac` cannot give server authentication against another
legitimate client. If this route is taken, `ed25519` should become the only
supported mode for untrusted clients, and `hmac` documented as a
shared-trust-group convenience.

**Blocker: half of this needs non-public API.** The two halves are not in the
same state.

The *server* half is public. `ISteamNetworkingSockets::SetCertificate` and
`GetCertificateRequest` are both in `include/steam/isteamnetworkingsockets.h`
(`:871`, `:867`), so installing a cert on the server needs no patching.

The *client* half has no public entry point. Telling GNS which CA to trust means
`CertStore_AddCertFromBase64`, declared in
`src/steamnetworkingsockets/steamnetworkingsockets_certstore.h:112` and absent
from `include/steam/` entirely. Worse, it has **no callers anywhere in the GNS
source** outside the cert store's own implementation — in an open-source build
the store is never populated, so `CertStore_CheckCert` always fails and the
result falls through to `AllowRemoteUnsignedCert`. The machinery is complete and
simply unwired.

That makes the server half useless on its own: a cert nobody can validate is
ignored exactly like an unsigned one.

So this route requires a patch to the vcpkg port exposing a CA-install entry
point and calling the existing function. That is wiring rather than cryptography
— the verification path (`CertStore_CheckCASignature`, `CertStore_CheckCert`,
expiry and revocation handling) is already written. Upstream looks receptive:
immediately above the `#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE` branch that
returns `k_EUnsignedCert_Allow` unconditionally sits the comment *"We don't have
a cert authority.  We probably ought to make this customizable"*
(`steamnetworkingsockets_connections.cpp:1806`, in the GameNetworkingSockets
source rather than this repository).

Do not try to route around this by parsing `GetDetailedConnectionStatus` for
cert material. It is a human-readable diagnostic string with no stability
guarantee, and basing an authentication decision on it would be worse than the
status quo, which at least fails visibly.

### 2. Add freshness to the signed metadata

Independent of (1), and small. The signed payload currently has no nonce,
timestamp or session binding, and the gateway caches it on a persistent
`/server-signal` connection, so a captured blob stays valid forever.

Have the client generate a random nonce, send it with its `/signal` request, and
have the server echo it into the signed payload. The client rejects a payload
whose nonce is not its own.

This costs a protocol change: the nonce has to reach the server, so `/signal`
and `/server-signal` both grow a field, and `publish_server_metadata`'s
publish-once-and-cache model becomes per-session signing.

Worth noting this does not by itself stop the gateway — it can forward a fresh
nonce and get a fresh signature. It closes replay by anyone else, and it is a
prerequisite for (3).

### 3. Fallback if the GNS patch is rejected: authenticate above GNS

Only if (1) turns out to be unreachable — but note this does not depend on any
GNS internals, so it is a genuine alternative rather than a consolation prize.

Run an authenticated Diffie–Hellman at the application layer, over the
established GNS connection, and encrypt game payloads with the resulting key.
The server signs its ephemeral public key with the same Ed25519 key it already
signs metadata with; the client verifies against the key from its URL fragment.

This needs no channel binding, because it does not try to prove anything about
the transport underneath — it builds its own authenticated channel over one it
assumes is hostile. A relay that reads and rewrites every GNS packet still
cannot produce a signature over its own ephemeral key.

Cost: a second crypto layer to own, with its own nonce discipline, replay window
and rekeying, plus double encryption on every packet. That is real work and easy
to get subtly wrong, which is why (1) is preferred — it reuses a handshake that
is already correct.

**What does not work**, recorded so it is not attempted: a simple
challenge-response after connect, where the client sends a nonce and the server
signs it. A gateway relaying both legs forwards the nonce one way and the
signature back, and both sides see a valid exchange. Defeating that needs a
channel-binding value unique to each GNS leg, which GNS does not expose
publicly. The distinction from the scheme above is that signing a *nonce* proves
only liveness, whereas signing the *ephemeral key* binds the signature to the
channel being established.

### Speculative: keep the rendezvous off the gateway when neither side is a browser

Unvalidated — recorded as a direction, not a plan.

The gateway has to carry the GNS handshake for a browser client, because a
browser cannot open a UDP socket and something must bridge. Two native peers
have no such constraint. If a native client and a native server used the gateway
only for discovery and NAT punch coordination, and then ran GNS's handshake
directly between themselves, the gateway would never see a cert or an ephemeral
key — and the exposure described above would simply not exist for that topology,
without needing the port patch (1) depends on.

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
client toward a host of its choosing, and could replay signed metadata until (2)
lands. The gain is reducing it from *able to read plaintext* to *able to
misdirect or deny*, which is worth having but is not the same as removing it
from the trust boundary.

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
- **Identity field width.** `derive_identity_*` truncates to 96 bits to fit a GNS
  generic *string* (31 characters). `SetGenericBytes` holds exactly 32 raw bytes
  and would fit a full digest with no truncation, at the cost of hex-decoding at
  the call sites and less readable logs. The truncation is not a security
  boundary — the signature is the authenticator — but it is a question a reader
  has to be talked out of. Moot if (1) lands.

## Should be fixed

Points where one registered server can act on another server's sessions. These
are missing authorization checks rather than design trade-offs, and the fix is
small.

### `gns-rendezvous` and `gns-connected` are routed by session ID alone

In the `/server-signal` handler, two message types take the session ID from the
sending server's own message and look it up in the global session map, with no
check that the session belongs to that server:

```go
case "gns-rendezvous":
    relayRendezvousToClient(m.SessionID, m.Data)   // main.go:1248
case "gns-connected":
    relayGNSConnected(m.SessionID)                 // main.go:1251
```

Every other case in the same switch — `challenge-response`, `metadata` — uses
the `myID` / `myEntry` pair bound to the connection at registration. These two
are the exceptions. The opposite direction is already correct: `handleSignal`
passes `serverID` and `sessionID` from its own closure and lets the client
supply only the opaque payload (`main.go:968`).

What the gap allows, for a server that knows another session's ID:

- **Injection into the GNS key exchange.** The rendezvous channel carries GNS's
  ConnectRequest/ConnectOK, so this is a position on the key-exchange path. The
  two things that would otherwise stop it do not: GNS accepts unsigned certs,
  and the expected identity string is derivable from the server's public key,
  which is published in the join URL. In `hmac` mode the URL carries the shared
  secret outright.
- **Denial of service, with no cryptography needed.** `relayGNSConnected` tears
  down the session's relay. A session ID alone disconnects someone else's
  clients.

**Not currently exploitable**, and it is worth recording why. Session IDs are 16
bytes from `crypto/rand` (`newSessionID`, `main.go:342`), so they cannot be
enumerated, and they are deliberately kept off the observable surface — logs,
the journal and the admin panel all use the separate 3-byte `trackingID`s
instead. A registered server has no channel through which to learn another
session's ID.

The reason to fix it anyway is that the property holding it shut is secrecy of
an identifier rather than an ownership check. Any future change that surfaces a
session ID — a debug endpoint, an admin field, an error echoed back to a peer —
turns it into a live vulnerability with no other change and no obvious signal
that anything happened.

**Fix:** `clientSession` already records `serverID` at creation
(`main.go:640`). Pass the sending server's ID into both functions and drop the
message when it does not match, mirroring what the client direction already
does.

Note that server *registration* is not affected and does not need changing:
`beginRegistration` rejects an ID already held by a different connection
(`main.go:1296`), so a server cannot squat another's ID to reach its clients.

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
- **Stale comment in the web client test.** The `AUTH_MODE` / `SERVER_ROLE=wasm`
  guard in `.github/tests/web/run_webrtc_web_client_test.sh` explains the
  restriction in terms of `handleHostSignal` not forwarding metadata. That was a
  misdiagnosis. The actual reasons are that a duplicate JSON tag dropped the
  metadata field for every topology (since fixed), and that a wasm host never
  configures auth at all — the auth block sits inside the non-`COFFEE_WASM`
  branch of the `Networking` constructor, so a browser host cannot sign.
