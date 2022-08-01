#pragma once

#include <openssl/hmac.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/types.h>

namespace net::hmac {

enum class digest_type
{
    sha1,
    sha256,
    sha384,
    sha512,
    md5,
};

template<typename T>
stl_types::String digest(
    semantic::Span<T> const& data,
    stl_types::String const& key,
    digest_type              type = digest_type::sha256)
{
    EVP_MD const* hashing = nullptr;

    switch(type)
    {
    case digest_type::sha1:
        hashing = EVP_sha1();
        break;
    case digest_type::sha256:
        hashing = EVP_sha1();
        break;
    case digest_type::sha384:
        hashing = EVP_sha384();
        break;
    case digest_type::sha512:
    default:
        hashing = EVP_sha512();
        break;
    }

    stl_types::String out;
    out.resize(C_FCAST<libc_types::u32>(EVP_MD_size(hashing)));
    unsigned int md_size = 0;

    if(!HMAC(
           hashing,
           key.data(),
           static_cast<int>(key.size()),
           C_RCAST<const unsigned char*>(data.data()),
           data.size(),
           C_RCAST<unsigned char*>(out.data()),
           &md_size))
        return {};

    return out;
}

} // namespace net::hmac
