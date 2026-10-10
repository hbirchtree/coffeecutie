#include "webrtc_identity.h"

#if defined(USE_NETWORKING)

#include <coffee/core/debug/formatting.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <peripherals/stl/base64.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/string/hex.h>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#if __has_include(<sys/stat.h>)
#include <sys/stat.h>
#endif
#if !defined(_WIN32) && __has_include(<fcntl.h>) && __has_include(<unistd.h>)
#include <fcntl.h>
#include <unistd.h>
#define WEBRTC_IDENTITY_POSIX_OPEN 1
#endif

using namespace Coffee::Logging;

namespace webrtc_signaling {

namespace {

std::string base64_encode(std::vector<uint8_t> const& bytes)
{
    return b64::encode(
        semantic::Span<const uint8_t>(bytes.data(), bytes.size()));
}

nlohmann::json sort_json(nlohmann::json const& j)
{
    if(j.is_object())
    {
        std::map<std::string, nlohmann::json> sorted;
        for(auto const& [key, value] : j.items())
            sorted[key] = sort_json(value);
        return nlohmann::json(sorted);
    }
    if(j.is_array())
    {
        nlohmann::json out = nlohmann::json::array();
        for(auto const& item : j)
            out.push_back(sort_json(item));
        return out;
    }
    return j;
}

EVP_PKEY* generate_ed25519_key()
{
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
    if(!ctx)
        return nullptr;
    EVP_PKEY* pkey = nullptr;
    if(EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_keygen(ctx, &pkey) <= 0)
        pkey = nullptr;
    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

EVP_PKEY* load_or_generate_ed25519_key(std::string const& path)
{
    if(FILE* f = std::fopen(path.c_str(), "r"); f)
    {
        EVP_PKEY* pkey = PEM_read_PrivateKey(f, nullptr, nullptr, nullptr);
        std::fclose(f);
        if(pkey)
            return pkey;
    }

    cDebug("Generating new Ed25519 identity key: {}", path);
    EVP_PKEY* pkey = generate_ed25519_key();
    if(!pkey)
        return nullptr;

    FILE* f = nullptr;
#if defined(WEBRTC_IDENTITY_POSIX_OPEN)
    /* Owner-only from creation, no window with wider permissions */
    if(int fd = ::open(
           path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
       fd >= 0)
    {
        ::fchmod(fd, S_IRUSR | S_IWUSR);
        f = ::fdopen(fd, "w");
        if(!f)
            ::close(fd);
    }
#else
    f = std::fopen(path.c_str(), "w");
#endif
    if(f)
    {
        PEM_write_PrivateKey(f, pkey, nullptr, nullptr, 0, nullptr, nullptr);
        std::fclose(f);
    } else
    {
        cWarning(
            "Failed to persist Ed25519 identity key to {}: {}",
            path,
            std::strerror(errno));
    }
    return pkey;
}

std::vector<uint8_t> ed25519_public_key(EVP_PKEY* pkey)
{
    size_t len = 0;
    if(EVP_PKEY_get_raw_public_key(pkey, nullptr, &len) <= 0)
        return {};
    std::vector<uint8_t> pk(len);
    if(EVP_PKEY_get_raw_public_key(pkey, pk.data(), &len) <= 0)
        return {};
    return pk;
}

std::vector<uint8_t> ed25519_sign(EVP_PKEY* pkey, std::string_view data)
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if(!ctx)
        return {};
    if(EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, pkey) <= 0)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }
    /* Ed25519 is one-shot in OpenSSL: the Update/Final pair is unsupported
     * for it and fails at Update, so EVP_DigestSign() is the only way in. */
    auto const* msg     = reinterpret_cast<const unsigned char*>(data.data());
    size_t      sig_len = 0;
    if(EVP_DigestSign(ctx, nullptr, &sig_len, msg, data.size()) <= 0)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }
    std::vector<uint8_t> sig(sig_len);
    if(EVP_DigestSign(ctx, sig.data(), &sig_len, msg, data.size()) <= 0)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }
    sig.resize(sig_len);
    EVP_MD_CTX_free(ctx);
    return sig;
}

bool ed25519_verify(
    EVP_PKEY* pkey, std::string_view data, std::vector<uint8_t> const& sig)
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if(!ctx)
        return false;
    /* One-shot, for the same reason as ed25519_sign above. */
    bool ok = EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, pkey) > 0 &&
              EVP_DigestVerify(
                  ctx,
                  sig.data(),
                  sig.size(),
                  reinterpret_cast<const unsigned char*>(data.data()),
                  data.size()) == 1;
    EVP_MD_CTX_free(ctx);
    return ok;
}

EVP_PKEY* ed25519_public_key_from_bytes(std::vector<uint8_t> const& pk)
{
    return EVP_PKEY_new_raw_public_key(
        EVP_PKEY_ED25519, nullptr, pk.data(), pk.size());
}

} // namespace

ParsedWebRtcUrl parse_webrtc_url(std::string const& url)
{
    ParsedWebRtcUrl result;
    auto            hash = url.find('#');
    if(hash == std::string::npos)
    {
        result.gateway_url = url;
        return result;
    }
    result.gateway_url   = url.substr(0, hash);
    std::string fragment = url.substr(hash + 1);

    auto semi        = fragment.find(';');
    result.server_id = fragment.substr(0, semi);
    if(semi != std::string::npos)
        result.auth = parse_auth_param(fragment.substr(semi + 1));
    return result;
}

WebrtcAuth parse_auth_param(std::string_view param)
{
    WebrtcAuth auth;
    if(!param.starts_with("auth="))
        return auth;
    auto value = param.substr(5);
    auto colon = value.find(':');
    if(colon == std::string_view::npos)
        return auth;
    auto type = value.substr(0, colon);
    auto data = std::string(value.substr(colon + 1));
    auto key  = b64::decode(data);
    /* Re-encoding rejects what the decoder forgives, like the unused bits of
     * the last character, so an edited key never passes as the original */
    if(key.empty() || base64_encode(key) != data)
    {
        cWarning("Malformed key in auth parameter: {}", data);
        auth.type = AuthType::Invalid;
        return auth;
    }
    if(type == "ed25519" && key.size() == 32)
    {
        auth.type               = AuthType::Ed25519;
        auth.ed25519_public_key = std::move(key);
    } else
    {
        cWarning("Unusable auth parameter: {}", param);
        auth.type = AuthType::Invalid;
    }
    return auth;
}

std::string format_auth_param(WebrtcAuth const& auth)
{
    switch(auth.type)
    {
    case AuthType::Ed25519:
        return "auth=ed25519:" + base64_encode(auth.ed25519_public_key);
    default:
        return {};
    }
}

std::string canonical_metadata_json(nlohmann::json const& meta)
{
    return sort_json(meta).dump();
}

/* A GNS generic-string identity holds 31 characters (k_cchMaxGenericString is
 * 32 with the terminator), and SetGenericString simply fails for anything
 * longer. A full 32-byte digest is 44 base64 characters, so an untruncated
 * identity never fit: it was rejected, the identity stayed invalid and fell
 * back to localhost, which in turn made GNS treat every connection as
 * anonymous and send an unsigned certificate. 96 bits leaves the prefix
 * comfortably inside the limit, and the signature -- not the identity string
 * -- is what authenticates a peer. */
constexpr size_t k_identity_digest_bytes = 12;

static std::string truncated_identity(
    std::string_view prefix, std::vector<uint8_t> const& digest)
{
    auto bytes = digest;
    bytes.resize(std::min(bytes.size(), k_identity_digest_bytes));
    return std::string(prefix) + base64_encode(bytes);
}

std::string derive_identity_ed25519(std::vector<uint8_t> const& public_key)
{
    /* Hashed rather than truncating the key itself, so the identity stays a
     * fingerprint of the whole key instead of a prefix of it. */
    std::vector<uint8_t> digest(SHA256_DIGEST_LENGTH);
    SHA256(public_key.data(), public_key.size(), digest.data());
    return truncated_identity("ed25519:", digest);
}

Ed25519Key Ed25519Key::generate()
{
    Ed25519Key key;
    key.m_key.reset(generate_ed25519_key(), EVP_PKEY_free);
    return key;
}

Ed25519Key Ed25519Key::load_or_generate(std::string const& pem_path)
{
    Ed25519Key key;
    key.m_key.reset(load_or_generate_ed25519_key(pem_path), EVP_PKEY_free);
    return key;
}

std::vector<uint8_t> Ed25519Key::public_key() const
{
    return m_key ? ed25519_public_key(m_key.get()) : std::vector<uint8_t>{};
}

std::vector<uint8_t> Ed25519Key::sign(std::string_view data) const
{
    return m_key ? ed25519_sign(m_key.get(), data) : std::vector<uint8_t>{};
}

nlohmann::json sign_metadata_ed25519(
    nlohmann::json meta, Ed25519Key const& key)
{
    auto pk = key.public_key();
    if(pk.empty())
    {
        cWarning("Failed to extract Ed25519 public key");
        return meta;
    }

    meta["identity"]      = derive_identity_ed25519(pk);
    std::string canonical = canonical_metadata_json(meta);
    auto        sig       = key.sign(canonical);

    if(sig.empty())
    {
        cWarning("Failed to sign metadata with Ed25519");
        return meta;
    }

    meta["auth"] = nlohmann::json{
        {"type", "ed25519"},
        {"public_key", base64_encode(pk)},
        {"signature", base64_encode(sig)},
    };
    return meta;
}

bool verify_metadata_ed25519(
    nlohmann::json const& meta, std::vector<uint8_t> const& public_key)
{
    if(!meta.contains("auth"))
        return false;
    auto auth = meta["auth"];
    if(auth.value("type", std::string()) != "ed25519")
        return false;

    auto sig = b64::decode(auth.value("signature", std::string()));
    if(sig.empty())
        return false;

    nlohmann::json stripped = meta;
    stripped.erase("auth");
    std::string canonical = canonical_metadata_json(stripped);

    EVP_PKEY* pkey = ed25519_public_key_from_bytes(public_key);
    if(!pkey)
        return false;
    bool ok = ed25519_verify(pkey, canonical, sig);
    EVP_PKEY_free(pkey);
    return ok;
}

} // namespace webrtc_signaling

#endif
