#pragma once

#include "../api.h"

#include <gsl/span>

namespace oaf::decode::ima_adpcm {

struct decoder
{
    bool decode(
        gsl::span<const char> const& data,
        format_t const&              format,
        buffer_t&                    output);

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
