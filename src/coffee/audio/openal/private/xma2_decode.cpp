#include <oaf/xma2/xma2_decode.h>

#include <coffee/core/debug/formatting.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/log.h>
#include <libavutil/mem.h>
}

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstring>
#include <memory>

namespace oaf::decode::xma2 {

using libc_types::u16;
using libc_types::u8;

using namespace Coffee::Logging;

namespace {

struct context_deleter
{
    void operator()(AVCodecContext* ctx) const
    {
        avcodec_free_context(&ctx);
    }
};

struct frame_deleter
{
    void operator()(AVFrame* frame) const
    {
        av_frame_free(&frame);
    }
};

struct packet_deleter
{
    void operator()(AVPacket* packet) const
    {
        av_packet_free(&packet);
    }
};

/* XMA packets are a fixed 2 KiB */
constexpr size_t packet_size = 2048;

/* XMA2WAVEFORMATEX past its WAVEFORMATEX head, little-endian: what the
 * xma2 decoder takes as extradata. Every two channels make one stream */
std::array<u8, 34> extradata_of(stream_t const& stream, size_t bytes)
{
    std::array<u8, 34> out{};
    size_t             at  = 0;
    auto               put = [&](auto value) {
        auto le = value;
        if constexpr(std::endian::native == std::endian::big)
            le = std::byteswap(value);
        std::memcpy(out.data() + at, &le, sizeof(le));
        at += sizeof(le);
    };
    u32 mask = stream.channels == 1   ? 0x4
               : stream.channels == 2 ? 0x3
               : stream.channels == 4 ? 0x33
                                      : 0x3F;
    put(static_cast<u16>((stream.channels + 1) / 2)); /* streams */
    put(mask);
    put(stream.samples);            /* samples encoded */
    put(static_cast<u32>(bytes));   /* bytes per block */
    put(u32{0});                    /* play begin */
    put(stream.samples);            /* play length */
    put(u32{0});                    /* loop begin */
    put(u32{0});                    /* loop length */
    put(u8{0});                     /* loop count */
    put(u8{4});                     /* encoder version */
    put(u16{1});                    /* block count */
    return out;
}

i16 to_s16(float v)
{
    return static_cast<i16>(std::clamp(std::lrintf(v * 32768.f), -32768l, 32767l));
}

} // namespace

bool decoder::decode_pcm(
    gsl::span<const char> const& data,
    stream_t const&              stream,
    std::vector<i16>&            samples,
    format_t&                    fmt)
{
    /* Raw packets carry no timestamps, which libavcodec warns about per
     * frame */
    av_log_set_level(AV_LOG_ERROR);
    auto const* codec = avcodec_find_decoder(AV_CODEC_ID_XMA2);
    if(!codec || data.empty() || !stream.channels || !stream.rate)
        return false;
    std::unique_ptr<AVCodecContext, context_deleter> ctx(avcodec_alloc_context3(codec));
    if(!ctx)
        return false;
    ctx->sample_rate = static_cast<int>(stream.rate);
    ctx->block_align = static_cast<int>(packet_size);
    av_channel_layout_default(&ctx->ch_layout, static_cast<int>(stream.channels));
    auto extra     = extradata_of(stream, data.size());
    ctx->extradata = static_cast<u8*>(av_mallocz(extra.size() + AV_INPUT_BUFFER_PADDING_SIZE));
    if(!ctx->extradata)
        return false;
    std::memcpy(ctx->extradata, extra.data(), extra.size());
    ctx->extradata_size = static_cast<int>(extra.size());
    if(avcodec_open2(ctx.get(), codec, nullptr) < 0)
    {
        cWarning("xma2: decoder init failed");
        return false;
    }

    std::unique_ptr<AVPacket, packet_deleter> packet(av_packet_alloc());
    std::unique_ptr<AVFrame, frame_deleter>   frame(av_frame_alloc());
    if(!packet || !frame)
        return false;
    samples.clear();
    auto drain = [&]() {
        while(avcodec_receive_frame(ctx.get(), frame.get()) >= 0)
        {
            int  channels = frame->ch_layout.nb_channels;
            bool planar   = av_sample_fmt_is_planar(static_cast<AVSampleFormat>(frame->format));
            for(int s = 0; s < frame->nb_samples; s++)
                for(int c = 0; c < channels; c++)
                {
                    auto const* plane =
                        reinterpret_cast<float const*>(frame->extended_data[planar ? c : 0]);
                    samples.push_back(to_s16(plane[planar ? s : s * channels + c]));
                }
            av_frame_unref(frame.get());
        }
    };

    std::vector<u8> buffer(packet_size + AV_INPUT_BUFFER_PADDING_SIZE);
    for(size_t at = 0; at + packet_size <= data.size(); at += packet_size)
    {
        std::memcpy(buffer.data(), data.data() + at, packet_size);
        packet->data = buffer.data();
        packet->size = static_cast<int>(packet_size);
        if(avcodec_send_packet(ctx.get(), packet.get()) < 0)
        {
            cWarning("xma2: bad packet at {}", at);
            return false;
        }
        drain();
    }
    avcodec_send_packet(ctx.get(), nullptr);
    drain();

    /* The decoder pads its last frame */
    size_t expected = size_t(stream.samples) * stream.channels;
    if(expected && samples.size() > expected)
        samples.resize(expected);
    fmt.format    = Format::pcm;
    fmt.frequency = stream.rate;
    fmt.channels  = static_cast<u16>(stream.channels);
    fmt.bits      = 16;
    return !samples.empty();
}

} // namespace oaf::decode::xma2
