#pragma once

#include "../api.h"
#include <chrono>

namespace oaf::decode::wav {

struct decoder
{
    bool decode(
        gsl::span<const char> const&                       data,
        std::optional<std::chrono::system_clock::duration> start_time,
        std::optional<std::chrono::system_clock::duration> duration,
        buffer_t&                                          output);

    std::string_view extension() const
    {
        return "wav";
    }
};

} // namespace oaf::decode::wav
