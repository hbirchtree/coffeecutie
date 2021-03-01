#pragma once

#include <openssl/evp.h>
#include <peripherals/semantic/chunk.h>

namespace net {
namespace pubkey {
namespace rsa {

semantic::Bytes encrypt(semantic::Bytes const& data)
{
    using namespace libc_types;

    auto context = EVP_CIPHER_CTX_new();
    auto cipher  = EVP_aes_128_cbc();
    auto iv      = semantic::mem_chunk<u8>::Alloc(EVP_CIPHER_iv_length(cipher));

    unsigned char* encrypt_key        = nullptr;
    int            encrypt_key_length = 0;

    EVP_PKEY* pkey = nullptr;

    if(!EVP_SealInit(
           context, cipher, &encrypt_key, &encrypt_key_length, iv.data, &pkey, 1))
    {
        EVP_PKEY_free(pkey);
        EVP_CIPHER_CTX_free(context);
        return {};
    }

    return {};
}

} // namespace rsa
} // namespace pubkey
} // namespace net
