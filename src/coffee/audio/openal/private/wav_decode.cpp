#include <oaf/wav/wav_decode.h>

#include <coffee/core/debug/formatting.h>

#include <cstring>

#if __has_include(<AL/alext.h>)
#include <AL/alext.h>
#endif

namespace oaf::decode::wav {

using libc_types::u16;
using libc_types::u32;
using libc_types::u64;

using namespace Coffee::Logging;

namespace {

// Raw wire structs, read via memcpy only — never reinterpret_cast'd over the
// input span, since it isn't guaranteed to be 4-byte aligned.
struct riff_header_t
{
    enum class riff_id_t : u32
    {
        RIFF = 0x46464952,
    } id{riff_id_t::RIFF};
    u32 riff_size{};
    enum class format_id_t : u32
    {
        WAVE = 0x45564157,
    } format{format_id_t::WAVE};
};

struct chunk_header_t
{
    enum class id_t : u32
    {
        fmt  = 0x20746d66,
        data = 0x61746164,
    } id{};
    u32 size{};
};

struct fmt_chunk_t
{
    enum class audio_format_t : u16
    {
        pcm        = 0x0001,
        adpcm      = 0x0002, // Microsoft ADPCM -> AL_SOFT_MSADPCM
        ieee_float = 0x0003,
        ima_adpcm  = 0x0011, // IMA ADPCM -> AL_EXT_IMA4
    } audio_format{audio_format_t::pcm};
    u16 num_channels{};
    u32 sample_rate{};
    u32 bytes_per_sec{};
    u16 bytes_per_block{};
    u16 bits_per_sample{};
    // Extension tail follows for non-PCM tags: cbSize(u16), then a
    // format-specific block. For both adpcm and ima_adpcm the first
    // extension field is wSamplesPerBlock(u16) at byte offset 18 from the
    // start of this chunk's data — that's all we need; the driver
    // (AL_SOFT_MSADPCM / AL_EXT_IMA4) does the actual decode, using its own
    // standard coefficient table for MS ADPCM.
};

constexpr size_t fmt_samples_per_block_offset = 18;

template<class T>
bool read(gsl::span<const char> const& data, size_t offset, T& out)
{
    if(offset + sizeof(T) > static_cast<size_t>(data.size()))
        return false;
    std::memcpy(&out, data.data() + offset, sizeof(T));
    return true;
}

} // namespace

bool decoder::decode(
    gsl::span<const char> const&                       data,
    std::optional<std::chrono::system_clock::duration> start_time,
    std::optional<std::chrono::system_clock::duration> duration,
    buffer_t&                                          output)
{
    riff_header_t riff{};
    if(!read(data, 0, riff) || riff.id != riff_header_t::riff_id_t::RIFF ||
       riff.format != riff_header_t::format_id_t::WAVE)
    {
        cDebug("WAV: not a RIFF/WAVE stream");
        return false;
    }

    std::optional<fmt_chunk_t> fmt{};
    size_t                     fmt_offset{};
    size_t                     fmt_size{};
    size_t                     data_offset{};
    size_t                     data_size{};

    size_t offset = sizeof(riff_header_t);
    while(offset < static_cast<size_t>(data.size()))
    {
        chunk_header_t chdr{};
        if(!read(data, offset, chdr))
            break;
        offset += sizeof(chdr);

        size_t chunk_size = chdr.size;
        if(offset + chunk_size > static_cast<size_t>(data.size()))
            chunk_size = data.size() - offset; // truncated file, best effort

        if(chdr.id == chunk_header_t::id_t::fmt)
        {
            fmt_chunk_t f{};
            if(chunk_size < sizeof(f) || !read(data, offset, f))
            {
                cDebug("WAV: fmt chunk too small ({}B)", chunk_size);
                return false;
            }
            fmt        = f;
            fmt_offset = offset;
            fmt_size   = chunk_size;
        } else if(chdr.id == chunk_header_t::id_t::data)
        {
            data_offset = offset;
            data_size   = chunk_size;
            break; // sample data found; ignore any trailing chunks
        }
        // else: unknown chunk (LIST/fact/JUNK/PEAK/cue /...) — skip over it.

        offset += chunk_size;
        if(chunk_size & 1)
            ++offset; // RIFF chunks are word-aligned; skip the pad byte
    }

    if(!fmt.has_value())
    {
        cDebug("WAV: no fmt chunk found before data (or before EOF)");
        return false;
    }
    if(data_size == 0)
    {
        cDebug("WAV: no data chunk found");
        return false;
    }
    if(fmt->num_channels == 0 || fmt->bytes_per_block == 0 ||
       fmt->sample_rate == 0)
    {
        cDebug("WAV: degenerate fmt chunk");
        return false;
    }

    format_t out_fmt{};
    out_fmt.frequency = fmt->sample_rate;
    out_fmt.channels  = fmt->num_channels;
    out_fmt.bits      = fmt->bits_per_sample;

    // Frames per nBlockAlign-sized block. 1 for PCM/float (nBlockAlign is a
    // single interleaved sample frame there); wSamplesPerBlock for ADPCM,
    // read from the fmt chunk's extension tail below.
    u32 samples_per_block = 1;

    switch(fmt->audio_format)
    {
    case fmt_chunk_t::audio_format_t::pcm:
        out_fmt.format = Format::format_t::pcm;
        break;
    case fmt_chunk_t::audio_format_t::ieee_float:
        out_fmt.format = Format::format_t::f32;
        break;
    case fmt_chunk_t::audio_format_t::ima_adpcm:
    case fmt_chunk_t::audio_format_t::adpcm: {
        // The driver (AL_EXT_IMA4 / AL_SOFT_MSADPCM) decodes these formats
        // itself; we just hand the raw blocks through. Still need
        // wSamplesPerBlock so we can tell it the block alignment and so
        // duration/seek math below is correct.
        u16 samples_per_block_16{};
        if(fmt_size < fmt_samples_per_block_offset + sizeof(u16) ||
           !read(
               data,
               fmt_offset + fmt_samples_per_block_offset,
               samples_per_block_16))
        {
            cDebug("WAV: adpcm fmt chunk missing wSamplesPerBlock");
            return false;
        }
        samples_per_block = samples_per_block_16;
        out_fmt.format =
            fmt->audio_format == fmt_chunk_t::audio_format_t::ima_adpcm
                ? Format::format_t::ima_adpcm
                : Format::format_t::ms_adpcm;
        break;
    }
    default:
        // Covers WAVE_FORMAT_EXTENSIBLE and anything else we don't
        // recognize.
        cDebug(
            "WAV: unsupported audio_format=0x{:x}",
            static_cast<u16>(fmt->audio_format));
        return false;
    }

    using namespace std::chrono_literals;

    auto total_samples =
        (data_size / fmt->bytes_per_block) * u64(samples_per_block);
    auto audio_duration = std::chrono::milliseconds(
        (u64(total_samples) * 1000) / fmt->sample_rate);

    auto time_offset   = start_time.value_or(0ms);
    auto time_duration = duration.value_or(audio_duration);

    // If we're trying to play a region outside the file, stop
    if(time_offset > audio_duration)
        return false;
    // If we're kind of within the file, but kind of outside, cap it
    if((time_offset + time_duration) > audio_duration)
        time_duration = audio_duration - time_offset;

    auto ms_offset = static_cast<u64>(
        std::chrono::duration_cast<std::chrono::milliseconds>(time_offset)
            .count());
    auto ms_duration = static_cast<u64>(
        std::chrono::duration_cast<std::chrono::milliseconds>(time_duration)
            .count());

    u64 byte_offset = (ms_offset * fmt->bytes_per_sec) / 1000;
    u64 byte_count  = (ms_duration * fmt->bytes_per_sec) / 1000;

    byte_offset = std::min<u64>(byte_offset, data_size);
    byte_count  = std::min<u64>(byte_count, data_size - byte_offset);

    // Keep block-aligned: for PCM a block is one sample frame, for ADPCM
    // it's a whole coded block carrying its own predictor state, so slicing
    // mid-block isn't just a rounding error, it's a garbage/misaligned
    // buffer handed to the driver's ADPCM decoder.
    byte_offset -= byte_offset % fmt->bytes_per_block;
    byte_count -= byte_count % fmt->bytes_per_block;

    cDebug(
        "WAV stream: rate={}, channels={}, bits={}, format={}, "
        "total={} samples, duration={}ms",
        fmt->sample_rate,
        fmt->num_channels,
        fmt->bits_per_sample,
        static_cast<u16>(fmt->audio_format),
        total_samples,
        audio_duration.count());

    if(samples_per_block != 1)
    {
        // ADPCM block size varies per file (nBlockAlign); tell the driver
        // so it decodes at the right cadence instead of assuming a default.
#if defined(AL_UNPACK_BLOCK_ALIGNMENT_SOFT)
        if(alIsExtensionPresent("AL_SOFT_block_alignment"))
        {
            alBufferi(
                output.m_handle,
                AL_UNPACK_BLOCK_ALIGNMENT_SOFT,
                static_cast<ALint>(samples_per_block));
            detail::check_error("alBufferi(AL_UNPACK_BLOCK_ALIGNMENT_SOFT)");
        } else
#endif
        {
            cDebug(
                "WAV: driver lacks AL_SOFT_block_alignment, ADPCM playback "
                "may be misaligned if this file doesn't use the driver's "
                "default block size");
        }
    }

    output.upload(
        gsl::span<const char>(
            data.data() + data_offset + byte_offset, byte_count),
        out_fmt);
    return true;
}

} // namespace oaf::decode::wav
