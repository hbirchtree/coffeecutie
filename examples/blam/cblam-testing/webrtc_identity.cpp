#include "webrtc_identity.h"

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <coffee/core/debug/formatting.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <peripherals/stl/base64.h>
#include <peripherals/stl/string/hex.h>

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#if __has_include(<sys/stat.h>)
#include <sys/stat.h>
#endif

using namespace Coffee::Logging;

namespace webrtc_signaling {

namespace {

std::string hex_encode(std::vector<uint8_t> const& bytes)
{
    std::string out;
    out.reserve(bytes.size() * 2);
    for(auto b : bytes)
        out += fmt::format("{:02x}", b);
    return out;
}

std::string base64_encode(std::vector<uint8_t> const& bytes)
{
    return b64::encode(
        semantic::Span<const uint8_t>(bytes.data(), bytes.size()));
}

std::vector<uint8_t> hmac_sha256(
    std::vector<uint8_t> const& key, std::string_view data)
{
    std::vector<uint8_t> result(SHA256_DIGEST_LENGTH);
    HMAC(
        EVP_sha256(),
        key.data(),
        static_cast<int>(key.size()),
        reinterpret_cast<const unsigned char*>(data.data()),
        data.size(),
        result.data(),
        nullptr);
    return result;
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
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
    if(!ctx)
        return nullptr;
    EVP_PKEY* pkey = nullptr;
    if(EVP_PKEY_keygen_init(ctx) <= 0 ||
       EVP_PKEY_keygen(ctx, &pkey) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return nullptr;
    }
    EVP_PKEY_CTX_free(ctx);

    if(FILE* f = std::fopen(path.c_str(), "w"); f)
    {
        PEM_write_PrivateKey(f, pkey, nullptr, nullptr, 0, nullptr, nullptr);
        std::fclose(f);
#    if defined(S_IRUSR) && defined(S_IWUSR)
        if(::chmod(path.c_str(), S_IRUSR | S_IWUSR) != 0)
            cWarning(
                "Failed to set restrictive permissions on Ed25519 identity "
                "key {}: {}",
                path,
                std::strerror(errno));
#    endif
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
    if(EVP_DigestSignUpdate(
           ctx, reinterpret_cast<const unsigned char*>(data.data()),
           data.size()) <= 0)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }
    size_t sig_len = 0;
    if(EVP_DigestSignFinal(ctx, nullptr, &sig_len) <= 0)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }
    std::vector<uint8_t> sig(sig_len);
    if(EVP_DigestSignFinal(ctx, sig.data(), &sig_len) <= 0)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }
    EVP_MD_CTX_free(ctx);
    return sig;
}

bool ed25519_verify(
    EVP_PKEY* pkey, std::string_view data, std::vector<uint8_t> const& sig)
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if(!ctx)
        return false;
    bool ok =
        EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, pkey) > 0 &&
        EVP_DigestVerifyUpdate(
            ctx, reinterpret_cast<const unsigned char*>(data.data()),
            data.size()) > 0 &&
        EVP_DigestVerifyFinal(ctx, sig.data(), sig.size()) == 1;
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
    result.gateway_url = url.substr(0, hash);
    std::string fragment = url.substr(hash + 1);

    auto semi = fragment.find(';');
    result.server_id = fragment.substr(0, semi);
    if(semi != std::string::npos)
    {
        std::string auth_part = fragment.substr(semi + 1);
        if(auth_part.starts_with("auth="))
        {
            std::string auth_value = auth_part.substr(5);
            auto        colon      = auth_value.find(':');
            if(colon != std::string::npos)
            {
                std::string auth_type = auth_value.substr(0, colon);
                std::string auth_data = auth_value.substr(colon + 1);
                if(auth_type == "hmac")
                {
                    result.auth.type     = AuthType::HmacSha256;
                    result.auth.hmac_key = b64::decode(auth_data);
                } else if(auth_type == "ed25519")
                {
                    result.auth.type = AuthType::Ed25519;
                    result.auth.ed25519_public_key = b64::decode(auth_data);
                } else
                {
                    cWarning("Unknown WebRTC auth type: {}", auth_type);
                }
            }
        }
    }
    return result;
}

std::string canonical_metadata_json(nlohmann::json const& meta)
{
    return sort_json(meta).dump();
}

std::string derive_identity_hmac(std::vector<uint8_t> const& key)
{
    auto hash = hmac_sha256(key, "coffee-webrtc-identity-v1");
    return "hmac-sha256:" + base64_encode(hash);
}

nlohmann::json sign_metadata_hmac(
    nlohmann::json meta, std::vector<uint8_t> const& key)
{
    meta["identity"]      = derive_identity_hmac(key);
    std::string canonical = canonical_metadata_json(meta);
    auto        hmac      = hmac_sha256(key, canonical);
    meta["auth"]          = nlohmann::json{
        {"type", "hmac-sha256"},
        {"hmac", hex_encode(hmac)},
    };
    return meta;
}

bool verify_metadata_hmac(
    nlohmann::json const& meta, std::vector<uint8_t> const& key)
{
    if(!meta.contains("auth"))
        return false;
    auto auth = meta["auth"];
    if(auth.value("type", std::string()) != "hmac-sha256")
        return false;
    auto expected_hex = auth.value("hmac", std::string());
    if(expected_hex.empty())
        return false;

    nlohmann::json stripped = meta;
    stripped.erase("auth");
    std::string canonical   = canonical_metadata_json(stripped);
    auto        computed    = hmac_sha256(key, canonical);
    auto        computed_hex = hex_encode(computed);
    return computed_hex == expected_hex;
}

std::string derive_identity_ed25519(std::vector<uint8_t> const& public_key)
{
    return "ed25519:" + base64_encode(public_key);
}

std::vector<uint8_t> load_or_generate_ed25519_public_key(
    std::string const& private_key_pem_path)
{
    EVP_PKEY* pkey = load_or_generate_ed25519_key(private_key_pem_path);
    if(!pkey)
        return {};
    auto pk = ed25519_public_key(pkey);
    EVP_PKEY_free(pkey);
    return pk;
}

nlohmann::json sign_metadata_ed25519(
    nlohmann::json meta, std::string const& private_key_pem_path)
{
    EVP_PKEY* pkey = load_or_generate_ed25519_key(private_key_pem_path);
    if(!pkey)
    {
        cWarning("Failed to load or generate Ed25519 key");
        return meta;
    }
    auto pk = ed25519_public_key(pkey);
    if(pk.empty())
    {
        EVP_PKEY_free(pkey);
        cWarning("Failed to extract Ed25519 public key");
        return meta;
    }

    meta["identity"]      = derive_identity_ed25519(pk);
    std::string canonical = canonical_metadata_json(meta);
    auto        sig       = ed25519_sign(pkey, canonical);
    EVP_PKEY_free(pkey);

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
