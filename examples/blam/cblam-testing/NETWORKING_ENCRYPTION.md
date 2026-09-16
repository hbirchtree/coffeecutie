# Networking encryption

How client/server game traffic is protected when a browser reaches a native
server through `webrtc-gateway`, or when two native builds connect directly, and
what each layer of authentication does and does not prove.

This describes what the code does today. Known weaknesses and the work to
address them live in [NETWORKING_TODO.md](NETWORKING_TODO.md).

## Topology

The gateway supports one topology:

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
(`networking.cpp:1312`).

Native builds can also skip the gateway entirely, with `--server` on the client
and `--listen` on the server. The same GNS-level authentication below applies
there, and it is the *only* protection on that path, since there is no DTLS hop.

## Two encryption layers

| Layer | Protects | Endpoints | Authenticated? |
| --- | --- | --- | --- |
| DTLS (WebRTC) | DataChannel messages | browser ↔ **gateway** | the gateway, by design |
| GNS session crypto | GNS packet payloads | client ↔ **server** | only with a pinned key (below) |

**DTLS** covers only the first hop. The gateway decrypts it — that is its job.

**GNS crypto** is the layer that spans client to server. Each side generates an
ephemeral Curve25519 key, signs it with its certificate's key, and the session
is encrypted with AES-256-GCM (`SetCryptoCipherList`, `FinishCryptoHandshake`).
The gateway relays these packets without inspecting them. Encryption cannot be
disabled by accident: `k_ESteamNetworkingConfig_Unencrypted` defaults to
`0: Not allowed`, nothing in this codebase sets it, and with that value the
local cipher list contains only AES-256-GCM — a peer offering NULL fails to
negotiate rather than downgrading.

Whether that encryption is *end to end* depends entirely on whether the client
can authenticate the server's certificate, which is what the rest of this
document is about.

## Server authentication

There are two mechanisms, and they protect different things. Both are keyed
from the same server keypair.

### Keys and modes

A web client joins with a single URL carrying gateway, server ID and key:

```
ws://gateway.example#serverId;auth=ed25519:<base64 public key>
ws://gateway.example#serverId;auth=hmac:<base64 shared secret>
```

The key lives in the URL **fragment**, which browsers never put on the wire, and
is parsed locally (`webrtc_identity.cpp:191`). The gateway never sees it, so it
cannot strip or swap the mode either.

A native client connecting without a gateway passes the server's Ed25519 public
key with `--server-key <base64>` alongside `--server <address>`
(`graphics.cpp:118`, `networking.cpp:905`).

| Mode | Server holds | Client holds | Metadata signature | GNS cert pinning |
| --- | --- | --- | --- | --- |
| `ed25519` | private key PEM | public key | Ed25519 | **yes** |
| `hmac` | shared secret | the same secret | HMAC-SHA256 | no |

A native server given `--gateway-register` with no key generates and persists an
Ed25519 key (`networking.cpp:842`), so `ed25519` is the default for a registered
server. `--gateway-auth-key` names the key explicitly and, despite its name,
also works with plain `--listen` and no gateway.

`hmac` cannot support certificate pinning, and deliberately does not try: every
client holding the join URL holds the secret, so any of them could mint the
server's certificate. It remains useful against the gateway, which never sees
the fragment, but not against other legitimate clients.

### 1. Signed metadata

The server publishes metadata about itself to the gateway
(`networking.cpp:479`), signed over a canonical JSON encoding with recursively
sorted keys so both sides hash identical bytes (`canonical_metadata_json`):

```json
{"map":"bloodgulch","map_type":"multiplayer","player_count":3,
 "player_count_max":16,"identity":"ed25519:Hx3…"}
```

The `auth` object is appended after signing; verification strips it and
recomputes. The client receives the payload in the gateway's `answer` and
verifies it **against the key from its own URL fragment**
(`networking.cpp:1256`). Ed25519 metadata also carries `auth.public_key`, but the
verify path ignores it (`webrtc_identity.cpp:366`) — trusting a key shipped
alongside its own signature would make the check meaningless. A failed or
missing signature, when the URL asked for auth, aborts the join before any GNS
connection is attempted.

What this proves is narrow: *a holder of the key produced this description of a
server at some point*. It carries no nonce or timestamp, so it can be replayed,
and it binds nothing about the connection that follows. On its own it cannot
stop a relay from substituting keys in the GNS handshake. That is what the
certificate is for.

### 2. Pinned GNS certificate

**Server side.** At startup a native server with an Ed25519 key issues itself a
GNS certificate (`install_self_signed_cert`, `networking.cpp:754`):

1. `SteamNetworkingSockets_GetSelfSignedCertBlobToSign` returns the serialized
   certificate body GNS would present: the instance's connection public key, its
   identity, the app ID, and a validity window (24 hours,
   `networking.cpp:742`).
2. The application signs those exact bytes with its Ed25519 private key
   (`sign_blob_ed25519`, `webrtc_identity.cpp:316`). The private key never
   crosses into GNS.
3. `SteamNetworkingSockets_InstallSelfSignedCert` attaches the signature and
   installs the result as the instance certificate.

GNS then presents that certificate on every accepted connection.

**Client side.** Before each outgoing connection the client sets
`k_ESteamNetworkingConfig_PinnedRootCertPublicKey` to the server's public key
(`add_pinned_root_config`, `networking.cpp:1092`). It is applied on all three
connect paths: plain UDP (`:1171`), and both the relayed-UDP and P2P DataChannel
shapes, set before they branch (`:1298`). It is deliberately *not* set on the
listen socket — clients carry no key, and a server pinning a root would lock
them all out.

**What GNS verifies** when a pin is set (`RecvCryptoHandshake`):

1. The peer's certificate **must** carry a CA signature, and that signature must
   verify against the pinned key and nothing else
   (`BCheckCertAgainstPinnedRoot`). An unsigned certificate, or one signed by any
   other key, ends the connection with `Remote_BadCert`. Neither the process-wide
   cert store nor `AllowRemoteUnsignedCert` — which returns an unconditional
   "allow" in open-source builds — is consulted.
2. The identity inside the certificate must match the identity the connection
   expects.
3. The peer's session crypt block, containing its ephemeral Curve25519 key, must
   be signed by the key inside that certificate.

That is a complete chain: pinned key → certificate → ephemeral key → session
keys. A relay can read and rewrite every packet of the handshake, but it cannot
produce a certificate the pinned key signed, so it cannot substitute its own
ephemeral key without the connection failing. The chain also makes the handshake
immune to replay: each session's crypt block carries a fresh ephemeral key whose
private half never leaves the server.

A pinned, verified connection also clears GNS's
`k_nSteamNetworkConnectionInfoFlags_Unauthenticated` flag.

The GNS side of this is not upstream. It is added by
`ports/gamenetworkingsockets/add-pinned-root-cert-auth.patch` in
[hbirchtree/vcpkg-ports](https://github.com/hbirchtree/vcpkg-ports), selected by
the registry baseline in `vcpkg-configuration.json`.

## The identity check

Every verified metadata payload yields an `identity` string, derived from the
server's key: a prefix plus 12 bytes of digest, base64 (`webrtc_identity.cpp:247`,
`:257`, `:296`), sized to fit a GNS generic-string identity of 31 characters.
The client installs it as the expected identity and, once GNS reports
`Connected`, closes the connection if the peer's identity differs
(`networking.cpp:1607`).

With a pinned certificate this string is backed by the certificate: the
identity arrives inside a signed cert, so the comparison is meaningful. Without
one it is a string the peer chose, and the check guards only against accidental
misrouting — `ed25519:` identities are a hash of a public key that is published
in every join URL, so anyone can compute and claim them.

## What each mode protects

| Scenario | Metadata verified | GNS session authenticated | Gateway can read game traffic |
| --- | --- | --- | --- |
| Join URL, `ed25519` | yes | **yes** | **no** |
| Join URL, `hmac` | yes | no | yes, by substituting keys |
| Join URL, no `auth=` | no | no | yes, by substituting keys |
| `--server` with `--server-key` | — | **yes** | — (no gateway) |
| `--server` without a key | — | no | — (any on-path host can) |

In the authenticated rows, the following hold regardless of what the gateway —
or anything else on the path — does:

- **Confidentiality and integrity of game traffic** between client and server.
  The gateway relays ciphertext it holds no keys for.
- **Server identity.** A client reaches only a process holding the server's
  Ed25519 private key. Redirecting it anywhere else, or presenting a replayed
  certificate from an attacker-controlled host, fails the handshake rather than
  succeeding quietly.
- **No downgrade.** The mode is chosen by the URL fragment, which never reaches
  the gateway, and the cipher list admits only AES-256-GCM.

In the unauthenticated rows, traffic is still encrypted against a passive
eavesdropper, but anything that can modify the handshake — the gateway above
all, since it relays it — can run two sessions and read and write plaintext.

## What this does not protect against

Even with `ed25519` pinning, the gateway can still:

- **Observe metadata.** Which client connects to which server, when, for how
  long, and packet sizes and timing. It also relays the signed metadata itself,
  so it knows the map and player counts.
- **Deny service.** Drop, delay or tear down sessions. The client fails closed,
  but it does fail.
- **Replay stale metadata.** A client may be shown an out-of-date description of
  a server. It cannot be turned into a working connection to anything else.
- **Join as a player.** Authentication runs one way: the server is authenticated
  to the client, and clients are anonymous. Anyone, the gateway included, can
  connect to a server as an ordinary client.

Other limits:

- **Browser-hosted servers cannot be authenticated.** A Wasm host configures no
  server key (the Wasm branch of the `Networking` constructor,
  `networking.cpp:822`), so it neither signs metadata nor presents a
  certificate. A client pinning a key refuses it.
- **Cross-tenant session routing.** A registered server can address messages to
  another server's session by ID (`main.go:1231`, `:1234`). For a pinned session
  this can no longer read traffic — injected handshake data fails certificate
  verification — but tearing down another server's relay remains possible. See
  "Should be fixed" in [NETWORKING_TODO.md](NETWORKING_TODO.md).

### Is this end-to-end encryption?

**In `ed25519` mode, yes** — end-to-end, server-authenticated encryption. The
client authenticates the server's key before any game data flows, the session
keys are derived from that authenticated exchange, and the gateway in the middle
holds none of them. It is authenticated in one direction only; clients remain
anonymous.

**In `hmac` mode or without `auth=`, no.** Traffic is encrypted in transit on
both hops, but nothing stops the relay from supplying both sides' keys, so the
gateway operator is inside the trust boundary.

## Current limitations of the implementation

Found reviewing the certificate work; both should be fixed.

- **Certificates are not renewed, so servers stop accepting authenticated
  clients after 24 hours.** The certificate is issued once, in the `Networking`
  constructor (`networking.cpp:888`), which runs once per process. GNS's own
  renewal is compiled only for Steam builds (`STEAMNETWORKINGSOCKETS_CAN_REQUEST_CERT`).
  Once the certificate expires, both places GNS attaches a local certificate —
  `BThinkCryptoReady` and the accept path the patch adds to
  `RecvCryptoHandshake` — require `GetSecondsUntilCertExpiry() > 0`, so the
  server falls back to an unsigned certificate. Every pinning client is then
  refused with "Peer presented a cert with no CA signature". This fails closed:
  it costs availability, not security.
- **Clients do not check certificate expiry.** The patch documents that "peers
  reject expired certs", but the pinned branch verifies only the signature. The
  one remote-certificate expiry check in GNS lives in `CertStore_CheckCert`,
  which that branch skips, and `CheckRemoteCert` returns early for pinned
  certificates. A certificate therefore remains acceptable indefinitely, and
  the 24-hour window bounds nothing on the verifying side. The practical
  exposure is small — abusing an old certificate also requires the per-process
  connection private key it was issued for — but the documented property does
  not hold.

## How this is tested

`x86_64 <-🔒-> x86_64` in `.github/workflows/10-compile.yml` runs
`.github/tests/net/run_pinned_cert_test.sh`: a native server with an Ed25519 key
and a native client over plain UDP, three times.

1. Client pins the right key → must connect.
2. Client pins a different key → must not connect.
3. Client pins nothing → must still connect, unauthenticated.

Gaps in that coverage:

- Case 2 asserts only that no connection was established, not why, so a server
  that failed to start would pass it too.
- No case pins a key against a server presenting an **unsigned** certificate —
  the branch that overrides open-source GNS's allow-everything default, and the
  one a malicious relay would most naturally hit.
- No CI job joins a browser client through the gateway with an `ed25519` URL.
  The metadata check and pinning on the DataChannel paths are covered by code
  review, not by a test.

## Code map

| Concern | Location |
| --- | --- |
| URL fragment parsing | `webrtc_identity.cpp:191` |
| Canonical JSON, signing, verification | `webrtc_identity.cpp:234`–`:389` |
| Identity derivation | `webrtc_identity.cpp:247`, `:257`, `:296` |
| Key load / auto-generation | `webrtc_identity.cpp:80`, `networking.cpp:842` |
| Certificate body signing | `webrtc_identity.cpp:316` |
| Server metadata build/publish | `networking.cpp:479`, `:527` |
| Server certificate issuance | `networking.cpp:742`, `:754`, `:888` |
| `--server-key` | `graphics.cpp:118`, `networking.cpp:905` |
| Client pinning | `networking.cpp:1092`, `:1171`, `:1298` |
| Client metadata verification | `networking.cpp:1256` |
| Identity comparison on connect | `networking.cpp:1607` |
| Relay and transport modes | `tools/webrtc-gateway/main.go:1`, `:108` |
| Pinned-certificate test | `.github/tests/net/run_pinned_cert_test.sh` |

Paths above are relative to this directory unless they start with `tools/` or
`.github/`. Function names such as `RecvCryptoHandshake`,
`BCheckCertAgainstPinnedRoot` and `CertStore_CheckCert`, and every
`k_ESteamNetworking*` name, are in the patched GameNetworkingSockets source
rather than this repository.
