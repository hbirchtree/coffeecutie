#include <oaf/wma/wma_decode.h>

#include <coffee/core/debug/formatting.h>

#include <libasf/asf.h>
/* wmadec.h is plain C without a linkage guard */
extern "C" {
#include <libwma/wmadec.h>
}

#include <algorithm>
#include <array>
#include <cstring>
#include <memory>

namespace oaf::decode::wma {

using libc_types::i16;
using libc_types::i32;
using libc_types::i64;
using libc_types::u16;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;

using namespace Coffee::Logging;

namespace {

/* ASF object GUIDs as laid out on disk */
constexpr std::array<u8, 16> header_guid = {
    0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11,
    0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};
constexpr std::array<u8, 16> file_properties_guid = {
    0xA1, 0xDC, 0xAB, 0x8C, 0x47, 0xA9, 0xCF, 0x11,
    0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65};
constexpr std::array<u8, 16> stream_properties_guid = {
    0x91, 0x07, 0xDC, 0xB7, 0xB7, 0xA9, 0xCF, 0x11,
    0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65};
constexpr std::array<u8, 16> audio_media_guid = {
    0x40, 0x9E, 0x69, 0xF8, 0x4D, 0x5B, 0xCF, 0x11,
    0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B};
constexpr std::array<u8, 16> data_guid = {
    0x36, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11,
    0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};

template<typename T>
T read_le(u8 const* at)
{
    T v;
    std::memcpy(&v, at, sizeof(T));
    return v;
}

bool is_guid(u8 const* at, std::array<u8, 16> const& guid)
{
    return std::memcmp(at, guid.data(), 16) == 0;
}

/* Memory-backed stdio for the packet reader */
struct memory_io_t
{
    u8 const* data;
    i64       size;
    i64       at;
};

size_t mem_read(void* ptr, size_t size, size_t nmemb, void* ctx)
{
    auto* io = static_cast<memory_io_t*>(ctx);
    i64   want = static_cast<i64>(size * nmemb);
    i64   have = std::clamp<i64>(io->size - io->at, 0, want);
    std::memcpy(ptr, io->data + io->at, static_cast<size_t>(have));
    io->at += have;
    return size ? static_cast<size_t>(have) / size : 0;
}

int mem_seek(void* ctx, int64_t offset, int whence)
{
    auto* io   = static_cast<memory_io_t*>(ctx);
    i64   base = whence == SEEK_SET ? 0 : whence == SEEK_CUR ? io->at : io->size;
    i64   to   = base + offset;
    if(to < 0 || to > io->size)
        return -1;
    io->at = to;
    return 0;
}

int64_t mem_tell(void* ctx)
{
    return static_cast<memory_io_t*>(ctx)->at;
}

int64_t mem_length(void* ctx)
{
    return static_cast<memory_io_t*>(ctx)->size;
}

/* Walks the header object for the file properties and the first audio
 * stream, then finds the data object; returns the offset of the first
 * packet, 0 if the file is not usable */
u64 parse_header(gsl::span<const char> const& data, asf_waveformatex_t& wfx)
{
    auto const* bytes = reinterpret_cast<u8 const*>(data.data());
    u64         size  = data.size();
    if(size < 30 || !is_guid(bytes, header_guid))
        return 0;
    u64 header_size = read_le<u64>(bytes + 16);
    u32 count       = read_le<u32>(bytes + 24);
    if(header_size > size)
        return 0;
    std::memset(&wfx, 0, sizeof(wfx));
    wfx.audiostream = -1;
    u64 at          = 30;
    for(u32 i = 0; i < count && at + 24 <= header_size; i++)
    {
        u8 const* obj      = bytes + at;
        u64       obj_size = read_le<u64>(obj + 16);
        if(obj_size < 24 || at + obj_size > header_size)
            return 0;
        if(is_guid(obj, file_properties_guid) && obj_size >= 104)
        {
            wfx.numpackets      = read_le<u64>(obj + 56);
            wfx.play_duration   = read_le<u64>(obj + 64);
            wfx.send_duration   = read_le<u64>(obj + 72);
            wfx.preroll         = read_le<u64>(obj + 80);
            wfx.flags           = read_le<u32>(obj + 88);
            wfx.packet_size     = read_le<u32>(obj + 92);
            wfx.max_packet_size = read_le<u32>(obj + 96);
        } else if(is_guid(obj, stream_properties_guid) && obj_size >= 78 && wfx.audiostream < 0)
        {
            if(is_guid(obj + 24, audio_media_guid))
            {
                u32 type_len = read_le<u32>(obj + 64);
                u16 flags    = read_le<u16>(obj + 72);
                if(type_len >= 18 && 78 + type_len <= obj_size)
                {
                    u8 const* wave     = obj + 78;
                    wfx.audiostream    = flags & 0x7F;
                    wfx.codec_id       = read_le<u16>(wave + 0);
                    wfx.channels       = read_le<u16>(wave + 2);
                    wfx.rate           = read_le<u32>(wave + 4);
                    wfx.bitrate        = read_le<u32>(wave + 8) * 8;
                    wfx.blockalign     = read_le<u16>(wave + 12);
                    wfx.bitspersample  = read_le<u16>(wave + 14);
                    wfx.datalen        = read_le<u16>(wave + 16);
                    u32 extra          = std::min<u32>(wfx.datalen, sizeof(wfx.data));
                    if(18 + extra <= type_len)
                        std::memcpy(wfx.data, wave + 18, extra);
                }
            }
        }
        at += obj_size;
    }
    if(wfx.audiostream < 0 || !wfx.packet_size)
        return 0;
    /* The data object follows the header: packets start after its 50-byte
     * head */
    at = header_size;
    if(at + 50 > size || !is_guid(bytes + at, data_guid))
        return 0;
    return at + 50;
}

} // namespace

bool decoder::decode_pcm(
    gsl::span<const char> const& data, std::vector<i16>& samples, format_t& fmt)
{
    asf_waveformatex_t wfx;
    u64                first_packet = parse_header(data, wfx);
    if(!first_packet)
    {
        cWarning("wma: no WMA audio stream in the ASF header");
        return false;
    }
    if(wfx.codec_id != ASF_CODEC_ID_WMAV1 && wfx.codec_id != ASF_CODEC_ID_WMAV2)
    {
        cWarning("wma: codec {:#x} is not WMA v1/v2", wfx.codec_id);
        return false;
    }
    auto context = std::make_unique<WMADecodeContext>();
    std::memset(context.get(), 0, sizeof(WMADecodeContext));
    if(wma_decode_init(context.get(), &wfx) < 0)
    {
        cWarning("wma: decoder init failed");
        return false;
    }
    memory_io_t mem{
        reinterpret_cast<u8 const*>(data.data()),
        static_cast<i64>(data.size()),
        static_cast<i64>(first_packet)};
    asf_io_t io{mem_read, mem_seek, mem_tell, mem_length, &mem};

    std::vector<u8> packet(std::max<u32>(wfx.packet_size, 32 * 1024) + 32);
    u32 const       channels = std::clamp<u32>(wfx.channels, 1, MAX_CHANNELS);
    samples.clear();
    for(u64 p = 0; p < wfx.numpackets; p++)
    {
        u8* audio   = packet.data();
        int audio_n = 0, packet_len = 0;
        int got     = asf_read_packet(&audio, &audio_n, &packet_len, &wfx, &io);
        if(got < 0)
            break;
        if(got == 0)
            continue;
        /* Each block of the packet is one superframe */
        for(int at = 0; at + wfx.blockalign <= audio_n; at += wfx.blockalign)
        {
            if(wma_decode_superframe_init(context.get(), audio + at, wfx.blockalign) <= 0)
                break;
            for(int f = 0; f < context->nb_frames; f++)
            {
                if(wma_decode_superframe_frame(context.get(), audio + at, wfx.blockalign) < 0)
                    break;
                int n = context->frame_len;
                for(int s = 0; s < n; s++)
                    for(u32 c = 0; c < channels; c++)
                        samples.push_back(static_cast<i16>(std::clamp<i32>(
                            -(context->frame_out[c][s] >> 14), -32768, 32767)));
            }
        }
    }
    /* The first frames are the codec's priming */
    size_t priming = static_cast<size_t>(4096) * channels;
    if(samples.size() > priming)
        samples.erase(samples.begin(), samples.begin() + static_cast<std::ptrdiff_t>(priming));
    fmt.format    = Format::pcm;
    fmt.frequency = wfx.rate;
    fmt.channels  = static_cast<u16>(channels);
    fmt.bits      = 16;
    cDebug(
        "wma: {} packets -> {} samples, {} Hz {}ch",
        wfx.numpackets,
        samples.size() / channels,
        wfx.rate,
        channels);
    return !samples.empty();
}

} // namespace oaf::decode::wma
