#ifndef COFFEE_MEDIA_FFMPEG_FFMEDIA_H
#define COFFEE_MEDIA_FFMPEG_FFMEDIA_H

#include "ff_pubformats.h"

namespace Coffee{
namespace FFMedia{

extern void FFInit(CFFMessageCallback callback = nullptr, bool silent = true);

extern CFFStreamDescriptor GetStreamData(const CResource& source);

extern FFVideoPlayer* CreatePlayer(const CResource& source);

extern void FreePlayer(FFVideoPlayer* vplayer);

extern size_t GetVideoFramesize(const FFVideoPlayer* video);
extern size_t GetVideoFramesize(const CSize& video);

extern size_t GetAudioSampleSize(const FFVideoPlayer* video);

extern FFDecodeContext *CreateDecodeContext(
        const FFVideoPlayer* video, const CFFVideoDescriptor& fmt);

extern void FreeDecodeContext(FFDecodeContext* dCtxt);

extern bool DecodeFrame(
        const FFVideoPlayer* video, FFDecodeContext* dCtxt, CFFVideoTarget *dTrgt);

extern bool DecodeIsEOS(const FFDecodeContext* dCtxt);

}
}

#endif
