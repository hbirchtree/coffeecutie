#pragma once

#include "../api.h"

#include <vector>

namespace oaf::decode::wma {

using libc_types::i16;

/* WMA v1/v2 in an ASF container (Halo 2's "codec" sounds), through the
 * libwma port: fixed-point decode to 16-bit PCM */
struct decoder
{
    /* The whole file as interleaved 16-bit samples; `fmt` gets the rate
     * and channel count. False when the ASF has no WMA audio stream */
    bool decode_pcm(
        gsl::span<const char> const& data,
        std::vector<i16>&            samples,
        format_t&                    fmt);

    bool decode(gsl::span<const char> const& data, buffer_t& output)
    {
        std::vector<i16> samples;
        format_t         fmt;
        if(!decode_pcm(data, samples, fmt))
            return false;
        output.upload(gsl::span<const i16>(samples), fmt);
        return true;
    }

    std::string_view extension() const
    {
        return "wma";
    }
};

} // namespace oaf::decode::wma
