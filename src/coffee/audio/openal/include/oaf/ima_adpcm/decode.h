#pragma once

#include "../api.h"

#include <gsl/span>

namespace oaf::decode::ima_adpcm {

size_t LoadSamples(
    gsl::span<float>           dstSamples,
    gsl::span<const std::byte> src,
    const size_t               srcChan,
    const size_t               srcOffset,
    const size_t               srcStep,
    const size_t               samplesPerBlock);

struct decoder
{
    bool decode(
        gsl::span<const char> const& data,
        format_t const&              format,
        buffer_t&                    output)
    {
        std::vector<f32> buffer;
        buffer.resize(data.size() * 2);

        auto decoded = gsl::span(buffer.data(), buffer.size());

        size_t written1 = LoadSamples(
            decoded,
            gsl::span(
                reinterpret_cast<const std::byte*>(data.data()), data.size()),
            0,
            0,
            format.channels,
            65);
        size_t written2;
        if(format.channels == 2)
            written2 = LoadSamples(
                decoded.subspan(data.size()),
                gsl::span(
                    reinterpret_cast<const std::byte*>(data.data()),
                    data.size()),
                0,
                0,
                format.channels,
                65);

        if(format.channels == 2)
        {
            std::vector<f32> interleaved;
            interleaved.reserve(written1 * 2);
            for(size_t i = 0; i < written1; ++i)
            {
                f32 v1 = decoded[i], v2 = decoded[data.size() + i];
                interleaved.push_back(v1);
                interleaved.push_back(v2);
            }
            buffer = std::move(interleaved);
        } else
            buffer.resize(written1);
        decoded = gsl::span(buffer.data(), buffer.size());

        format_t fmt = format;
        fmt.format   = format_t::f32;
        output.upload(decoded, fmt);

        return true;
    }

    std::string_view extension() const
    {
        return {};
    }

    Format::format_t format() const
    {
        return Format::format_t::ima_adpcm;
    }
};

} // namespace oaf::decode::ima_adpcm
