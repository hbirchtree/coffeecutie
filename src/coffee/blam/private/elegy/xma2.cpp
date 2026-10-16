#include <blam/elegy/xma2.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/mem.h>
}

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>

namespace blam::elegy {

namespace {

struct codec_context_deleter
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

i16 to_s16(f32 v)
{
    return static_cast<i16>(std::clamp(std::lrintf(v * 32768.f), -32768l, 32767l));
}

} // namespace

bool decode_xma2(xma2_format_t const& format, Span<byte_t const> data, pcm_t& out)
{
    auto const* codec = avcodec_find_decoder(AV_CODEC_ID_XMA2);
    if(!codec || data.empty())
        return false;
    std::unique_ptr<AVCodecContext, codec_context_deleter> ctx(
        avcodec_alloc_context3(codec));
    if(!ctx)
        return false;
    u32 channels = from_le(format.channels);
    ctx->sample_rate = static_cast<int>(from_le(format.samples_per_sec));
    ctx->block_align = from_le(format.block_align);
    av_channel_layout_default(&ctx->ch_layout, static_cast<int>(channels));
    /* The decoder reads the XMA2WAVEFORMATEX tail past the WAVEFORMATEX head */
    constexpr size_t head = 18;
    size_t extra = sizeof(format) - head;
    ctx->extradata = static_cast<u8*>(av_mallocz(extra + AV_INPUT_BUFFER_PADDING_SIZE));
    if(!ctx->extradata)
        return false;
    std::memcpy(ctx->extradata, reinterpret_cast<u8 const*>(&format) + head, extra);
    ctx->extradata_size = static_cast<int>(extra);
    if(avcodec_open2(ctx.get(), codec, nullptr) < 0)
        return false;

    std::unique_ptr<AVPacket, packet_deleter> packet(av_packet_alloc());
    std::unique_ptr<AVFrame, frame_deleter>   frame(av_frame_alloc());
    if(!packet || !frame)
        return false;
    out.channels = channels;
    out.rate     = from_le(format.samples_per_sec);
    out.samples.clear();

    auto drain = [&]() {
        while(avcodec_receive_frame(ctx.get(), frame.get()) >= 0)
        {
            u32  frame_channels = static_cast<u32>(frame->ch_layout.nb_channels);
            bool planar = av_sample_fmt_is_planar(static_cast<AVSampleFormat>(frame->format));
            for(int s = 0; s < frame->nb_samples; s++)
                for(u32 c = 0; c < frame_channels; c++)
                {
                    auto const* plane = reinterpret_cast<f32 const*>(
                        frame->extended_data[planar ? c : 0]);
                    out.samples.push_back(
                        to_s16(plane[planar ? s : s * frame_channels + c]));
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
            return false;
        drain();
    }
    avcodec_send_packet(ctx.get(), nullptr);
    drain();

    /* Trim the decoder's tail padding to the stored length */
    size_t expected = size_t(from_le(format.samples_encoded)) * channels;
    if(expected && out.samples.size() > expected)
        out.samples.resize(expected);
    return !out.samples.empty();
}

} // namespace blam::elegy
