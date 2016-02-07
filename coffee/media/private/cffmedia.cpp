#include <coffee/media/cffmedia.h>
#include <coffee/core/coffee_strings.h>

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

static thread_local bool cffmedia_context_created = false;

constexpr AVPixelFormat default_pixfmt = AV_PIX_FMT_RGBA;

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
    CFFStream* subtitles;
};

struct CFFDecodeContext
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

struct CFFEncodeContext
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

void coffee_ffmedia_init(CFFMessageCallback callback, bool silent)
{
    if(cffmedia_context_created)
        return;

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

    cffmedia_context_created = true;
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
    CMemCpy(buf,opaque,buf_size);
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
    probe_data.buf = (uint8*)Alloc(probe_data.buf_size);
    probe_data.mime_type = nullptr;

    CMemCpy(probe_data.buf,data,probe_data.buf_size);

    AVInputFormat* infmt = av_probe_input_format(&probe_data,1);

    if(!infmt)
        infmt = av_probe_input_format(&probe_data,0);

    CFree(probe_data.buf);
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
    if(vplayer->audio)
        ff_close_stream(vplayer->audio);
    if(vplayer->video)
        ff_close_stream(vplayer->video);

    avformat_close_input(&vplayer->fmtContext);

    avformat_free_context(vplayer->fmtContext);

    delete vplayer;
}

size_t coffee_ffmedia_video_framesize(const CFFVideoPlayer *video)
{
    return av_image_get_buffer_size(default_pixfmt,
                                    video->video->context->width,
                                    video->video->context->height,
                                    8);
}

size_t coffee_ffmedia_video_framesize(const CSize &video)
{
    return av_image_get_buffer_size(default_pixfmt,
                                    video.w,video.h,
                                    1);
}

size_t coffee_ffmedia_audio_samplesize(const CFFVideoPlayer *video)
{
    return av_samples_get_buffer_size(
                nullptr,
                video->audio->context->channels,
                1,
                video->audio->stream->codec->sample_fmt,
                1);
}

CFFDecodeContext* coffee_ffmedia_create_decodecontext(
        const CFFVideoPlayer *video, const CFFVideoDescriptor &fmt)
{
    CFFDecodeContext* dCtxt = new CFFDecodeContext;

    CMemClear(dCtxt,sizeof(CFFDecodeContext));

    if(video->video)
    {
        CFFStream* strm = video->video;
        dCtxt->v.sws_ctxt = sws_getContext(
                    strm->context->width, strm->context->height,
                    strm->context->pix_fmt,
                    fmt.video.size.width, fmt.video.size.height, default_pixfmt,
                    SWS_BILINEAR, NULL, NULL, NULL);

        dCtxt->v.frame = av_frame_alloc();
        dCtxt->v.tFrame = av_frame_alloc();

        dCtxt->v.tb = video->video->stream->time_base;
        dCtxt->v.frame_rate = av_guess_frame_rate(video->fmtContext,video->video->stream,
                                                  NULL);

        dCtxt->v.resolution.w = fmt.video.size.width;
        dCtxt->v.resolution.h = fmt.video.size.height;
    }
    if(video->audio)
    {
        dCtxt->a.swr_ctxt = nullptr;

        /* Magic magic to get sample format properties from enum */
        ff_get_sampleformat(fmt.audio.format,&dCtxt->a.chlayout,&dCtxt->a.sfmt,&dCtxt->a.channels,&dCtxt->a.bitdepth);

        dCtxt->a.srate = fmt.audio.samplerate;

        SwrContext* ctxt = swr_alloc();
        swr_alloc_set_opts(ctxt,
                           dCtxt->a.chlayout,
                           dCtxt->a.sfmt,dCtxt->a.srate,
                           video->audio->context->channel_layout,
                           video->audio->context->sample_fmt,
                           video->audio->context->sample_rate,
                           0,
                           NULL);
        int error_code = swr_init(ctxt);
        if(error_code!=0)
        {
            cLog(__FILE__,__LINE__,
                 CFStrings::Media_FF_Name,
                 CFStrings::Media_FF_SWR_InitError,
                 error_code);
        }else{
            dCtxt->a.swr_ctxt = ctxt;
        }
        int32 samples = av_rescale_rnd(1,dCtxt->a.srate,
                                       video->audio->context->sample_rate,
                                       AV_ROUND_UP);
        av_samples_alloc_array_and_samples(&dCtxt->a.data,&dCtxt->a.linesize,
                                           dCtxt->a.channels,samples,
                                           dCtxt->a.sfmt,0);

        dCtxt->a.frame = av_frame_alloc();
        dCtxt->a.tFrame = av_frame_alloc();
    }

    return dCtxt;
}

void coffee_ffmedia_free_decodecontext(CFFDecodeContext *dCtxt)
{
    if(dCtxt->v.sws_ctxt)
    {
        sws_freeContext(dCtxt->v.sws_ctxt);

        av_frame_free(&dCtxt->v.frame);
        av_frame_free(&dCtxt->v.tFrame);
    }

    if(dCtxt->a.swr_ctxt)
    {
        swr_close(dCtxt->a.swr_ctxt);
        swr_free(&dCtxt->a.swr_ctxt);

        av_freep(&dCtxt->a.data[0]);

        av_frame_free(&dCtxt->a.frame);
        av_frame_free(&dCtxt->a.tFrame);
    }

    delete dCtxt;
}

bool coffee_ffmedia_decode_frame(const CFFVideoPlayer* video,
                                 CFFDecodeContext* dCtxt,
                                 CFFVideoTarget* dTrgt)
{
    //If we have come to the end of the stream, just stop.
    if(dCtxt->eos)
        return false;
    //We query FFMPEG for a new frame in the form of a packet
    if(av_read_frame(video->fmtContext,&dCtxt->packet)<0)
    {
        dCtxt->eos = true;
        return false;
    }else
        dCtxt->eos = false;

    int gotFrame = 0;
    if(dCtxt->packet.stream_index == video->video->index
            && dTrgt->v.location
            && dCtxt->v.sws_ctxt)
    {
        //Decode the video packet in this case
        avcodec_decode_video2(video->video->context,
                              dCtxt->v.frame,
                              &gotFrame,
                              &dCtxt->packet);

        if(gotFrame)
        {
            //We got a frame! Now process it!

            //We redirect the video stream to our determined location
            //This location is most likely GL memory
            av_image_fill_arrays(dCtxt->v.tFrame->data,
                                 dCtxt->v.tFrame->linesize,
                                 (uint8*)dTrgt->v.location,
                                 default_pixfmt,
                                 dCtxt->v.resolution.w,dCtxt->v.resolution.h,
                                 8);

            //Scale the video frame
            sws_scale(dCtxt->v.sws_ctxt,
                      dCtxt->v.frame->data,
                      dCtxt->v.frame->linesize,
                      0,
                      video->video->context->height,
                      dCtxt->v.tFrame->data,
                      dCtxt->v.tFrame->linesize);

            dTrgt->v.pts = dCtxt->packet.pts;
            dTrgt->v.updated = true;

            //Free the frame
            av_frame_unref(dCtxt->v.frame);
        }
    }else if(dCtxt->packet.stream_index == video->audio->index
             && dCtxt->a.swr_ctxt)
    {
        //Packet might contain several audio frames, loop over it so we can capture them all
        int pkt_size = dCtxt->packet.size;
        int pkt_offset = 0;
        uint8* pkt_data = dCtxt->packet.data;
        while(pkt_size > 0)
        {
            //Decode audio packet
            int len = avcodec_decode_audio4(video->audio->context,
                                  dCtxt->a.frame,
                                  &gotFrame,
                                  &dCtxt->packet);

            if(len < 0)
            {
                break;
            }

            if(gotFrame&&dCtxt->a.swr_ctxt)
            {
                dTrgt->a.queue_mutex.lock();

                dTrgt->a.packet_queue.push(CFFAudioPacket());

                CFFAudioPacket &p = dTrgt->a.packet_queue.back();

                p.pts = dCtxt->a.frame->pts;
                p.channels = dCtxt->a.channels;
                p.frequency = dCtxt->a.srate;
                p.bitdepth = dCtxt->a.bitdepth;

                /* av_samples_get_buffer_size(nullptr,
                                                           2,
                                                           dCtxt->a.frame->nb_samples,
                                                           AV_SAMPLE_FMT_S16,
                                                           1);
                 */

                uint32 nb_samples = av_rescale_rnd(swr_get_delay(dCtxt->a.swr_ctxt,
                                                               video->audio->context->sample_rate)
                                                   +video->audio->context->sample_rate,
                                                   dCtxt->a.srate,
                                                   video->audio->context->sample_rate,
                                                   AV_ROUND_UP);

                av_freep(&dCtxt->a.data[0]);
                int32 data_size = av_samples_alloc(dCtxt->a.data,
                                                    &dCtxt->a.linesize,
                                                    dCtxt->a.channels,
                                                    nb_samples,dCtxt->a.sfmt,1);

                p.data = (int16*)Alloc(data_size);

                nb_samples = swr_convert(dCtxt->a.swr_ctxt,dCtxt->a.data,nb_samples,
                            (const uint8**)dCtxt->a.frame->data,
                            dCtxt->a.frame->nb_samples);

                p.samples = nb_samples;

                CMemCpy(p.data,dCtxt->a.data[0],data_size);

                dTrgt->a.queue_mutex.unlock();

                //Free the audio frame
                av_frame_unref(dCtxt->a.frame);
            }

            pkt_offset += len;
            pkt_data += len;
            pkt_size -= len;
        }
    }else if(dCtxt->packet.stream_index == video->subtitles->index)
    {
        //TODO: Implement FFMPEG subtitles
        avcodec_decode_subtitle2(video->subtitles->context,
                                 &dCtxt->s.subtitle,
                                 &gotFrame,
                                 &dCtxt->packet);
        if(gotFrame)
        {
            for(szptr i=0;i<dCtxt->s.subtitle.num_rects;i++)
                cDebug("Subtitle text: {0}",dCtxt->s.subtitle.rects[i]->text);
        }
    }
    //Free the packet
    av_packet_unref(&dCtxt->packet);

    return true;
}

bool coffee_ffmedia_decode_is_eos(const CFFDecodeContext *dCtxt)
{
    return dCtxt->eos;
}

}
}
