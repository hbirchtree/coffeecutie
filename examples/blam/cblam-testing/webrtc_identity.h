#pragma once

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace webrtc_signaling {

enum class AuthType
{
    None,
    HmacSha256,
    Ed25519,
};

struct WebrtcAuth
{
    AuthType type = AuthType::None;
    /* HMAC key, base64-decoded. Kept secret. */
    std::vector<uint8_t> hmac_key;
    /* Ed25519 public key, base64-decoded. */
    std::vector<uint8_t> ed25519_public_key;
};

struct ParsedWebRtcUrl
{
    std::string gateway_url;
    std::string server_id;
    WebrtcAuth  auth;
};

/*! Parse a WebRTC gateway URL of the forms:
 *   ws://host#serverId
 *   ws://host#serverId;auth=hmac:base64secret
 *   ws://host#serverId;auth=ed25519:base64publickey
 * The fragment is never sent to the gateway; it is parsed locally. */
ParsedWebRtcUrl parse_webrtc_url(std::string const& url);

/*! Produce a deterministic, compact JSON representation suitable for
 * signing/verification. Object keys are sorted recursively. */
std::string canonical_metadata_json(nlohmann::json const& meta);

/*! Derive a stable GNS identity string from an HMAC secret. */
std::string derive_identity_hmac(std::vector<uint8_t> const& key);

/*! Sign metadata (cleartext) with HMAC-SHA256. The returned JSON is the
 * original metadata plus an "auth" object containing the hex HMAC. */
nlohmann::json sign_metadata_hmac(
    nlohmann::json meta, std::vector<uint8_t> const& key);

/*! Verify metadata signed with HMAC-SHA256. Returns true if the embedded
 * auth.hmac matches a freshly computed HMAC of the metadata fields. */
bool verify_metadata_hmac(
    nlohmann::json const& meta, std::vector<uint8_t> const& key);

/*! Derive a stable GNS identity string from an Ed25519 public key. */
std::string derive_identity_ed25519(std::vector<uint8_t> const& public_key);

/*! Load an Ed25519 private key from PEM, generating and persisting one if
 * the file does not exist. Returns the corresponding public key. */
std::vector<uint8_t> load_or_generate_ed25519_public_key(
    std::string const& private_key_pem_path);

/*! Sign metadata with Ed25519. The private key is supplied as a PEM file
 * path; the function will generate and persist a key if the file does not
 * exist. */
nlohmann::json sign_metadata_ed25519(
    nlohmann::json meta, std::string const& private_key_pem_path);

/*! Verify metadata signed with Ed25519. */
bool verify_metadata_ed25519(
    nlohmann::json const& meta, std::vector<uint8_t> const& public_key);

} // namespace webrtc_signaling

#endif
