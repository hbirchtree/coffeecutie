#include <coffee/media/cffmedia.h>
#include <coffee/core/coffee_strings.h>

#include "ffshimmy.h"

namespace Coffee{
namespace FFMedia{

static thread_local bool cffmedia_context_created = false;

constexpr AVPixelFormat default_pixfmt = AV_PIX_FMT_RGBA;

void FFInit(CFFMessageCallback callback, bool silent)
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

//FFStream* ff_open_stream(AVFormatContext* fCtxt, AVMediaType type, int streamIndex = 0)
//{
//    AVStream* strm = nullptr;
//    unsigned int index = -1;
//    unsigned int t_index = 0;
//    for(unsigned int i=0;i<fCtxt->nb_streams;i++)
//    {
//        if(fCtxt->streams[i]->codec->codec_type==type)
//        {
//            if(t_index == streamIndex)
//            {
//                strm = fCtxt->streams[i];
//                index = i;
//                break;
//            }else
//                t_index++;
//        }
//    }
//    if(!strm)
//        return nullptr;

//    AVCodecContext* orig = strm->codec;

//    AVCodec* codec = avcodec_find_decoder(orig->codec_id);
//    if(!codec)
//        return nullptr;

//    AVCodecContext* copy = avcodec_alloc_context3(codec);
//    if(avcodec_copy_context(copy,orig)!=0)
//    {
//        avcodec_free_context(&copy);
//        return nullptr;
//    }

//    if(avcodec_open2(copy,codec,nullptr)<0)
//    {
//        avcodec_free_context(&copy);
//        return nullptr;
//    }

//    FFStream* cstrm = new FFStream;
//    cstrm->codec = codec;
//    cstrm->context = copy;
//    cstrm->stream = strm;
//    cstrm->type = type;
//    cstrm->index = index;

//    return cstrm;
//}

int ff_read_data(void* opaque, uint8* buf, int buf_size)
{
    MemCpy(buf,opaque,buf_size);
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

    MemCpy(probe_data.buf,data,probe_data.buf_size);

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

    int err;
    if((err = avformat_open_input(&ctxt,probe_data.filename,infmt,nullptr))!=0)
    {
        avformat_free_context(ctxt);
        return nullptr;
    }else
        return ctxt;
}

CFFStreamDescriptor GetStreamData(const Resource &source)
{
	return CFFStreamDescriptor();
}

FFVideoPlayer *CreatePlayer(const Resource &source)
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

    FFVideoPlayer* video = new FFVideoPlayer;
    video->fmtContext = fmtCtxt;

    int vindex = 0;
    int aindex = 0;

    video->s[0] = ff_fetch_stream(fmtCtxt,AVMEDIA_TYPE_VIDEO,vindex);
    video->s[1] = ff_fetch_stream(fmtCtxt,AVMEDIA_TYPE_AUDIO,aindex);
    video->s[2] = nullptr;

    return video;
}

void FreePlayer(FFVideoPlayer *vplayer)
{
    for(int i=0;i<vplayer->num_streams;i++)
        if(vplayer->s[i])
            ff_close_stream(vplayer->s[i]);

    avformat_close_input(&vplayer->fmtContext);
    avformat_free_context(vplayer->fmtContext);

    delete vplayer;
}

size_t GetVideoFramesize(const FFVideoPlayer *video)
{
    return av_image_get_buffer_size(
                default_pixfmt,
                video->s[0]->context->width,
                video->s[0]->context->height,
                8);
}

size_t GetVideoFramesize(const CSize &video)
{
    return av_image_get_buffer_size(default_pixfmt,
                                    video.w,video.h,
                                    1);
}

size_t GetAudioSampleSize(const FFVideoPlayer *video)
{
    return av_samples_get_buffer_size(
                nullptr,
                video->s[1]->context->channels,
                1,
                video->s[1]->stream->codec->sample_fmt,
                1);
}

FFDecodeContext* CreateDecodeContext(
        const FFVideoPlayer *video, const CFFVideoDescriptor &fmt)
{
    FFDecodeContext* dCtxt = new FFDecodeContext;

    MemClear(dCtxt,sizeof(FFDecodeContext));

    if(video->s[0])
    {
        FFStream* strm = video->s[0];
        dCtxt->v.sws_ctxt = sws_getContext(
                    strm->context->width, strm->context->height,
                    strm->context->pix_fmt,
                    fmt.video.size.w, fmt.video.size.h, default_pixfmt,
                    SWS_BILINEAR, NULL, NULL, NULL);

        dCtxt->v.frame = av_frame_alloc();
        dCtxt->v.tFrame = av_frame_alloc();

        dCtxt->v.tb = strm->stream->time_base;
        dCtxt->v.frame_rate = av_guess_frame_rate(video->fmtContext,strm->stream,
                                                  NULL);

        dCtxt->v.resolution.w = fmt.video.size.w;
        dCtxt->v.resolution.h = fmt.video.size.h;
    }
    if(video->s[1])
    {
        FFStream* strm = video->s[1];

        dCtxt->a.swr_ctxt = nullptr;

        /* Magic magic to get sample format properties from enum */
        ff_get_sampleformat(fmt.audio.format,&dCtxt->a.chlayout,&dCtxt->a.sfmt,&dCtxt->a.channels,&dCtxt->a.bitdepth);

        dCtxt->a.srate = fmt.audio.samplerate;

        SwrContext* ctxt = swr_alloc();
        swr_alloc_set_opts(ctxt,
                           dCtxt->a.chlayout,
                           dCtxt->a.sfmt,dCtxt->a.srate,
                           strm->context->channel_layout,
                           strm->context->sample_fmt,
                           strm->context->sample_rate,
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
                                       strm->context->sample_rate,
                                       AV_ROUND_UP);
        av_samples_alloc_array_and_samples(&dCtxt->a.data,&dCtxt->a.linesize,
                                           dCtxt->a.channels,samples,
                                           dCtxt->a.sfmt,0);

        dCtxt->a.frame = av_frame_alloc();
        dCtxt->a.tFrame = av_frame_alloc();
    }

    return dCtxt;
}

void FreeDecodeContext(FFDecodeContext *dCtxt)
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

bool DecodeFrame(const FFVideoPlayer* video,
                                 FFDecodeContext* dCtxt,
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

    FFStream* vstream = video->s[0];
    FFStream* astream = video->s[1];
    FFStream* sstream = video->s[2];

    int gotFrame = 0;
    if(dCtxt->packet.stream_index == vstream->index
            && dTrgt->v.location
            && dCtxt->v.sws_ctxt)
    {
        //Decode the video packet in this case
        avcodec_decode_video2(vstream->context,
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
                      vstream->context->height,
                      dCtxt->v.tFrame->data,
                      dCtxt->v.tFrame->linesize);

            dTrgt->v.pts = dCtxt->packet.pts;
            dTrgt->v.updated = true;

            //Free the frame
            av_frame_unref(dCtxt->v.frame);
        }
    }else if(dCtxt->packet.stream_index == astream->index
             && dCtxt->a.swr_ctxt)
    {
        //Packet might contain several audio frames, loop over it so we can capture them all
        int pkt_size = dCtxt->packet.size;
        int pkt_offset = 0;
        uint8* pkt_data = dCtxt->packet.data;
        while(pkt_size > 0)
        {
            //Decode audio packet
            int len = avcodec_decode_audio4(astream->context,
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
                                                               astream->context->sample_rate)
                                                   +astream->context->sample_rate,
                                                   dCtxt->a.srate,
                                                   astream->context->sample_rate,
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

                MemCpy(p.data,dCtxt->a.data[0],data_size);

                dTrgt->a.queue_mutex.unlock();

                //Free the audio frame
                av_frame_unref(dCtxt->a.frame);
            }

            pkt_offset += len;
            pkt_data += len;
            pkt_size -= len;
        }
    }else if(dCtxt->packet.stream_index == sstream->index)
    {
        //TODO: Implement FFMPEG subtitles
        avcodec_decode_subtitle2(sstream->context,
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

bool DecodeIsEOS(const FFDecodeContext *dCtxt)
{
    return dCtxt->eos;
}

}
}
