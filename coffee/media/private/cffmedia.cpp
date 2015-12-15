#include <cffmedia.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

namespace Coffee{
namespace CFFMedia{

struct CFFStream
{
    AVStream* stream;

    AVCodecContext* context;
    AVCodec* codec;

    SwsContext* sws_ctxt;
    AVFrame* frame;

    AVMediaType type;
    int index;
};

struct CFFVideoPlayer
{
    AVFormatContext* fmtContext;

    CFFStream* audio;
    CFFStream* video;

    AVPacket tmp_packet;
};

void coffee_ffmedia_init()
{
    av_log_set_flags(AV_LOG_SKIP_REPEATED);
    avcodec_register_all();
    avdevice_register_all();
    av_register_all();
}

CFFStream* ff_open_stream(AVFormatContext* fCtxt, AVMediaType type)
{
    AVStream* strm = nullptr;
    unsigned int index = -1;
    for(unsigned int i=0;i<fCtxt->nb_streams;i++)
    {
        if(fCtxt->streams[i]->codec->codec_type==type)
        {
            strm = fCtxt->streams[i];
            index = i;
            break;
        }
    }
    if(!strm)
        return nullptr;

    AVCodecContext* orig = strm->codec;

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

    CFFStream* cstrm = new CFFStream;
    cstrm->codec = codec;
    cstrm->context = copy;
    cstrm->stream = strm;
    cstrm->type = type;
    cstrm->index = index;
    cstrm->frame = av_frame_alloc();

    return cstrm;
}

void ff_close_stream(CFFStream* strm)
{
    avcodec_free_context(&strm->context);
    av_frame_free(&strm->frame);
}

CFFVideoPlayer *coffee_ffmedia_create_player(const CResource &source)
{
    AVFormatContext* fmtCtxt = nullptr;

    if(avformat_open_input(&fmtCtxt,source.resource(),nullptr,nullptr)!=0)
        return nullptr;

    if(avformat_find_stream_info(fmtCtxt,nullptr)<0)
        return nullptr;

    av_dump_format(fmtCtxt,0,source.resource(),0);

    CFFVideoPlayer* video = new CFFVideoPlayer;
    video->fmtContext = fmtCtxt;

    video->audio = ff_open_stream(fmtCtxt,AVMEDIA_TYPE_AUDIO);
    video->video = ff_open_stream(fmtCtxt,AVMEDIA_TYPE_VIDEO);

    if(video->video)
    {
        ubyte_t* buffer = (ubyte_t*)av_malloc(coffee_ffmedia_video_framesize(video));
        av_image_fill_arrays(buffer,AV_PIX_FMT_BGRA,video->video->context->width,video->video->context->height);
    }

    return video;
}

size_t coffee_ffmedia_video_framesize(CFFVideoPlayer *video)
{
    return av_image_get_buffer_size(AV_PIX_FMT_BGRA,
                                    video->video->context->width,
                                    video->video->context->height,
                                    8);
}

size_t coffee_ffmedia_audio_framesize(CFFVideoPlayer *)
{
    return 0;
}

bool coffee_ffmedia_decode_frame(CFFVideoPlayer *video, const CFFVideoDestination& dest)
{
    CFFStream* strm = video->video;
    if(!strm->sws_ctxt)
    {
        strm->sws_ctxt = sws_getContext(
                    strm->context->width, strm->context->height, strm->context->pix_fmt,
                    dest.output_size.w, dest.output_size.h, AV_PIX_FMT_BGRA,
                    SWS_BILINEAR, NULL, NULL, NULL);
    }

    av_read_frame(video->fmtContext,&video->tmp_packet);
    if(video->tmp_packet.stream_index == strm->index)
    {
        int gotFrame = 0;
        avcodec_decode_video2(strm->context,strm->frame,&gotFrame,&video->tmp_packet);

//        sws_scale(strm->sws_ctxt,);
    }

    return true;
}

}
}
