#include <cffmedia.h>

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
namespace CFFMedia{

struct CFFStream
{
    AVStream* stream;

    AVCodecContext* context;
    AVCodec* codec;

    AVMediaType type;
    int index;
};

struct CFFVideoPlayer
{
    AVFormatContext* fmtContext;

    CFFStream* audio;
    CFFStream* video;

};

struct CFFDecodeContext
{
    struct{
        AVRational tb;
        AVRational frame_rate;
        SwsContext* sws_ctxt;
        AVFrame* frame;
    } v;
    struct{
        AVFrame* frame;
    } a;

    AVPacket packet;
};

void coffee_ffmedia_init(CFFMessageCallback callback, bool silent)
{
    if(callback)
        av_log_set_callback(callback);
    av_log_set_flags(AV_LOG_SKIP_REPEATED);
    if(!silent)
    {
        av_log_set_level(AV_LOG_INFO);
    }
    else
    {
        av_log_set_level(AV_LOG_FATAL);
    }
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

    return cstrm;
}

void ff_close_stream(CFFStream* strm)
{
    avcodec_close(strm->context);
    avcodec_free_context(&strm->context);
    delete strm;
}

int ff_read_data(void* opaque, uint8* buf, int buf_size)
{
    memcpy(buf,opaque,buf_size);
    return buf_size;
}

AVFormatContext* ff_open_data(
        void* data, szptr data_size,
        uint8* scratch_buffer = nullptr,
        szptr probe_size = 4096)
{
    AVFormatContext* ctxt = avformat_alloc_context();
    ctxt->pb = avio_alloc_context(
                scratch_buffer,data_size,0,data,
                ff_read_data,nullptr,nullptr);

    if(!ctxt->pb)
    {
        avformat_free_context(ctxt);
        return nullptr;
    }

    AVProbeData probe_data;
    probe_data.buf_size = (data_size < probe_size) ? data_size : probe_size;
    probe_data.filename = "stream";
    probe_data.buf = (uint8*)c_alloc(probe_data.buf_size);
    probe_data.mime_type = nullptr;

    memcpy(probe_data.buf,data,probe_data.buf_size);

    AVInputFormat* infmt = av_probe_input_format(&probe_data,1);

    if(!infmt)
        infmt = av_probe_input_format(&probe_data,0);

    free(probe_data.buf);
    probe_data.buf = nullptr;

    if(!infmt)
    {
        avformat_free_context(ctxt);
        return nullptr;
    }

//    infmt->flags |= AVFMT_NOFILE;

    int err;
    if((err = avformat_open_input(&ctxt,probe_data.filename,infmt,nullptr))!=0)
    {
        avformat_free_context(ctxt);
        return nullptr;
    }else
        return ctxt;
}

CFFVideoPlayer *coffee_ffmedia_create_player(const CResource &source)
{
    AVFormatContext* fmtCtxt = nullptr;

    if(!(fmtCtxt = ff_open_data(source.data,source.size,(uint8*)source.data)))
        return nullptr;

    if(avformat_find_stream_info(fmtCtxt,nullptr)<0)
    {
        avformat_close_input(&fmtCtxt);
        return nullptr;
    }

//    av_dump_format(fmtCtxt,0,nullptr,0);

    CFFVideoPlayer* video = new CFFVideoPlayer;
    video->fmtContext = fmtCtxt;

    video->audio = ff_open_stream(fmtCtxt,AVMEDIA_TYPE_AUDIO);
    video->video = ff_open_stream(fmtCtxt,AVMEDIA_TYPE_VIDEO);

    return video;
}

void coffee_ffmedia_free_player(CFFVideoPlayer *vplayer)
{
    ff_close_stream(vplayer->audio);
    ff_close_stream(vplayer->video);

    avformat_close_input(&vplayer->fmtContext);

    avformat_free_context(vplayer->fmtContext);

    delete vplayer;
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

CFFDecodeContext* coffee_ffmedia_create_decodecontext(
        const CFFVideoPlayer *video, const CSize &videores,
        const CAudioFormat &)
{
    CFFDecodeContext* dCtxt = new CFFDecodeContext;

    {
        CFFStream* strm = video->video;
        dCtxt->v.sws_ctxt = sws_getContext(
                    strm->context->width, strm->context->height, strm->context->pix_fmt,
                    videores.w, videores.h, AV_PIX_FMT_BGRA,
                    SWS_BILINEAR, NULL, NULL, NULL);
    }

    dCtxt->v.frame = av_frame_alloc();
    dCtxt->a.frame = av_frame_alloc();

    dCtxt->v.tb = video->video->stream->time_base;
    dCtxt->v.frame_rate = av_guess_frame_rate(video->fmtContext,video->video->stream,
                                              NULL);

    return dCtxt;
}

void coffee_ffmedia_free_decodecontext(CFFDecodeContext *dCtxt)
{
    sws_freeContext(dCtxt->v.sws_ctxt);

    av_frame_free(&dCtxt->v.frame);
    av_frame_free(&dCtxt->a.frame);
    delete dCtxt;
}

bool coffee_ffmedia_decode_frame(const CFFVideoPlayer* video,
                                 CFFDecodeContext* dCtxt,
                                 CFFVideoTarget* dTrgt)
{
//    cDebug("Framerate: {0}, time_base: {1}",dCtxt->v.frame_rate.num,dCtxt->v.tb.num);

    av_read_frame(video->fmtContext,&dCtxt->packet);
    if(dCtxt->packet.stream_index == video->video->index)
    {
        int gotFrame = 0;
        avcodec_decode_video2(video->video->context,
                              dCtxt->v.frame,
                              &gotFrame,
                              &dCtxt->packet);

        sws_scale(dCtxt->v.sws_ctxt,
                  (uint8 const* const*)dCtxt->v.frame->data,
                  dCtxt->v.frame->linesize,
                  0,
                  video->video->context->height,
                  (uint8* const*)dTrgt->v.location,
                  dCtxt->v.frame->linesize);

        av_frame_unref(dCtxt->v.frame);
    }
    av_packet_unref(&dCtxt->packet);

    return true;
}

}
}
