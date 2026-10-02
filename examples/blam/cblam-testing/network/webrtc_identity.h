#pragma once

#if defined(USE_NETWORKING)

#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct evp_pkey_st;

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

/*! Parse one "auth=hmac:<base64>" / "auth=ed25519:<base64>" join parameter.
 * AuthType::None if it is not one. */
WebrtcAuth parse_auth_param(std::string_view param);

/*! Inverse of parse_auth_param. Empty for AuthType::None. */
std::string format_auth_param(WebrtcAuth const& auth);

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

/*! An Ed25519 private key, held in memory. Empty on failure. */
class Ed25519Key
{
  public:
    /*! Fresh key that lives only as long as this process */
    static Ed25519Key generate();
    /*! Load from PEM, generating and persisting one if the file does not
     *  exist */
    static Ed25519Key load_or_generate(std::string const& pem_path);

    explicit operator bool() const
    {
        return static_cast<bool>(m_key);
    }

    std::vector<uint8_t> public_key() const;
    /*! Empty on failure */
    std::vector<uint8_t> sign(std::string_view data) const;

  private:
    std::shared_ptr<evp_pkey_st> m_key;
};

/*! Sign metadata with Ed25519 */
nlohmann::json sign_metadata_ed25519(
    nlohmann::json meta, Ed25519Key const& key);

/*! Verify metadata signed with Ed25519. */
bool verify_metadata_ed25519(
    nlohmann::json const& meta, std::vector<uint8_t> const& public_key);

} // namespace webrtc_signaling

#endif
