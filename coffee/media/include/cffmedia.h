#ifndef COFFEE_MEDIA_FFMPEG_FFMEDIA_H
#define COFFEE_MEDIA_FFMPEG_FFMEDIA_H

#include <coffee/core/CBase>
#include <coffee/core/CTypes>
#include <coffee/CAudio>
#include <coffee/graphics_apis/COpenGL>

namespace Coffee{
namespace CFFMedia{

using namespace CAudio;
using namespace CResources;

using CFFMessageCallback = void(*)(void*,int,cstring,va_list);

struct CFFDecodeContext;
struct CFFVideoPlayer;

struct CFFVideoFormat
{
    CTextureSize size;
};

struct CFFVideoDescriptor
{
    CAudioFormat audio;
    CFFVideoFormat video;
};

struct CFFStreamTarget
{
    void* location;
    size_t max_size;
    bool updated;
    int64 pts;
};

struct CFFVideoTarget
{
    CFFStreamTarget a;
    CFFStreamTarget v;
    CFFStreamTarget s;
};

extern void coffee_ffmedia_init(CFFMessageCallback callback = nullptr, bool silent = true);

extern CFFVideoPlayer* coffee_ffmedia_create_player(const CResource& source);

extern void coffee_ffmedia_free_player(CFFVideoPlayer* vplayer);

extern size_t coffee_ffmedia_video_framesize(const CFFVideoPlayer* video);
extern size_t coffee_ffmedia_video_framesize(const CSize& video);

extern size_t coffee_ffmedia_audio_samplesize(const CFFVideoPlayer* video);

extern CFFDecodeContext *coffee_ffmedia_create_decodecontext(
        const CFFVideoPlayer* video, const CFFVideoDescriptor& fmt);

extern void coffee_ffmedia_free_decodecontext(CFFDecodeContext* dCtxt);

extern bool coffee_ffmedia_decode_frame(
        const CFFVideoPlayer* video, CFFDecodeContext* dCtxt, CFFVideoTarget *dTrgt);

extern bool coffee_ffmedia_decode_is_eos(const CFFDecodeContext* dCtxt);

}
}

#endif
