#pragma once

#include "../api.h"

#include <vector>

namespace oaf::decode::xma2 {

using libc_types::i16;
using libc_types::u32;

/* What the container would say about an XMA2 stream: Halo 3 keeps this in
 * its sound tags rather than in a RIFF header */
struct stream_t
{
    u32 channels{0};
    u32 rate{0};
    u32 samples{0}; /*!< Per channel */
};

/* XMA2 (Xbox 360) through libavcodec, which is LGPL and therefore linked
 * as a shared library; only built with BUILD_OPENAL_XMA2 */
struct decoder
{
    /* Raw XMA2 packets (2 KiB each) as interleaved 16-bit samples, trimmed
     * to the stream's length */
    bool decode_pcm(
        gsl::span<const char> const& data,
        stream_t const&              stream,
        std::vector<i16>&            samples,
        format_t&                    fmt);

    bool decode(
        gsl::span<const char> const& data,
        stream_t const&              stream,
        buffer_t&                    output)
    {
        std::vector<i16> samples;
        format_t         fmt;
        if(!decode_pcm(data, stream, samples, fmt))
            return false;
        output.upload(gsl::span<const i16>(samples), fmt);
        return true;
    }

    std::string_view extension() const
    {
        return "xma";
    }
};

} // namespace oaf::decode::xma2
