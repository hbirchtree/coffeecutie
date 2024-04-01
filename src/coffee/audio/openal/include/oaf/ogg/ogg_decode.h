#pragma once

#include "../api.h"
#include <peripherals/concepts/sound_api.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

#include <vorbis/vorbisfile.h>

namespace oaf::decode::ogg {

struct decoder
{
    ~decoder();

    bool decode(
        gsl::span<const char> const&                       data,
        std::optional<std::chrono::system_clock::duration> start_time,
        std::optional<std::chrono::system_clock::duration> duration,
        buffer_t&                                          output);

    std::string_view extension() const
    {
        return "ogg";
    }

    semantic::concepts::sound::Format::format_t format() const
    {
        return semantic::concepts::sound::Format::format_t::pcm;
    }

    struct decode_data_t
    {
        gsl::span<const char> input;
        size_t                ptr{0};
    } decode_data;

    OggVorbis_File file{};
    vorbis_info*   file_info{nullptr};
};

} // namespace oaf::decode::ogg
