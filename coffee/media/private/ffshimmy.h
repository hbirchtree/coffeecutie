#pragma once

#include <coffee/core/CTypes>
#include <coffee/CAudio>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

namespace Coffee{
namespace FFMedia{

using namespace CAudio;
using namespace CResources;

struct FFStream
{
    AVStream* stream;

    AVCodecContext* context;
    AVCodec* codec;

    AVMediaType type;
    int index;
};

struct FFVideoPlayer
{
    enum Streams
    {
        Video = 0,
        Audio = 1,
        Subtitle = 2
    };

    static const constexpr int num_streams = 3;

    AVFormatContext* fmtContext;
    FFStream* s[num_streams];
};

struct FFDecodeContext
{
    struct{
        AVRational tb;
        AVRational frame_rate;
        SwsContext* sws_ctxt;
        AVFrame* frame;
        AVFrame* tFrame;
        CSize resolution;
    } v;
    struct{
        AVFrame* frame;
        AVFrame* tFrame;
        SwrContext* swr_ctxt;

        AVSampleFormat sfmt;
        int32 srate;
        int16 bitdepth;
        int32 channels;
        int64 chlayout;

        uint8** data;
        int32 linesize;
    } a;
    struct{
        AVSubtitle subtitle;
    } s;

    AVPacket packet;

    bool eos; /*!< Flag for end of stream*/
};

struct FFEncodeContext
{
    struct{
        CSize resolution;
    } v;
    struct{

    } a;
};

inline C_FORCE_INLINE void ff_get_sampleformat(SampleFormat fmt,
                                               int64* ch_layout,
                                               AVSampleFormat* sfmt,
                                               int32* channels,
                                               int16* bits)
{
    switch(fmt)
    {
    case SampleFormat::U8M:{
        *ch_layout = AV_CH_LAYOUT_MONO;
        *channels = 1;
        *sfmt = AV_SAMPLE_FMT_U8;
        *bits = 8;
        break;
    }
    case SampleFormat::U8S:{
        *ch_layout = AV_CH_LAYOUT_STEREO;
        *channels = 2;
        *sfmt = AV_SAMPLE_FMT_U8;
        *bits = 8;
        break;
    }
    case SampleFormat::U8Q:{
        *ch_layout = AV_CH_LAYOUT_4POINT0;
        *channels = 4;
        *sfmt = AV_SAMPLE_FMT_U8;
        *bits = 8;
        break;
    }

    case SampleFormat::I16M:{
        *ch_layout = AV_CH_LAYOUT_MONO;
        *channels = 1;
        *sfmt = AV_SAMPLE_FMT_S16;
        *bits = 16;
        break;
    }
    case SampleFormat::I16S:{
        *ch_layout = AV_CH_LAYOUT_STEREO;
        *channels = 2;
        *sfmt = AV_SAMPLE_FMT_S16;
        *bits = 16;
        break;
    }
    case SampleFormat::I16Q:{
        *ch_layout = AV_CH_LAYOUT_4POINT0;
        *channels = 4;
        *sfmt = AV_SAMPLE_FMT_S16;
        *bits = 16;
        break;
    }

    case SampleFormat::F32M:{
        *ch_layout = AV_CH_LAYOUT_MONO;
        *channels = 1;
        *sfmt = AV_SAMPLE_FMT_FLT;
        *bits = 16;
        break;
    }
    case SampleFormat::F32S:{
        *ch_layout = AV_CH_LAYOUT_STEREO;
        *channels = 2;
        *sfmt = AV_SAMPLE_FMT_FLT;
        *bits = 32;
        break;
    }
    case SampleFormat::F32Q:{
        *ch_layout = AV_CH_LAYOUT_4POINT0;
        *channels = 4;
        *sfmt = AV_SAMPLE_FMT_FLT;
        *bits = 32;
        break;
    }
    }
}

int ff_get_num_streams(AVFormatContext* ctxt)
{
    return ctxt->nb_streams;
}

int ff_get_num_streams_type(AVFormatContext* ctxt, AVMediaType tp)
{
    uint32 count = 0;
    for(uint32 i=0;i<ctxt->nb_streams;i++)
    {
        if(ctxt->streams[i]->codec->codec_type == tp)
            count++;
    }
    return count;
}

AVStream* ff_get_stream(AVFormatContext* ctxt, AVMediaType tp, int index, int* real_index)
{
    uint32 tp_count = 0;
    for(uint32 i=0;i<ctxt->nb_streams;i++)
    {
        if(ctxt->streams[i]->codec->codec_type == tp)
            if(tp_count == index)
            {
                *real_index = i;
                return ctxt->streams[i];
            }else
                tp_count++;
    }
    return nullptr;
}

FFStream* ff_open_stream(AVStream* stream, int index)
{
    AVCodecContext* orig = stream->codec;

    AVCodec* codec = avcodec_find_decoder(orig->codec_id);
    if(!codec)
        return nullptr;

    AVCodecContext* copy = avcodec_alloc_context3(codec);
    if(avcodec_copy_context(copy,orig)!=0)
    {
        avcodec_free_context(&copy);
        return nullptr;
    }
    if(avcodec_open2(copy,codec,nullptr)<0)
    {
        avcodec_free_context(&copy);
        return nullptr;
    }

    FFStream* out_stream = new FFStream;
    out_stream->codec = codec;
    out_stream->context = copy;
    out_stream->stream = stream;
    out_stream->type = stream->codec->codec_type;
    out_stream->index = index;

    return out_stream;
}

FFStream* ff_fetch_stream(AVFormatContext* ctxt, AVMediaType tp, int index)
{
    int real_index = 0;
    AVStream* stream = ff_get_stream(ctxt,tp,index,&real_index);
    if(!stream)
        return nullptr;
    return ff_open_stream(stream,real_index);
}

void ff_close_stream(FFStream* strm)
{
    avcodec_close(strm->context);
    avcodec_free_context(&strm->context);
    delete strm;
}



}
}