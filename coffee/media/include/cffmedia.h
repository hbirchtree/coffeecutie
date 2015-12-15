#ifndef COFFEE_MEDIA_FFMPEG_FFMEDIA_H
#define COFFEE_MEDIA_FFMPEG_FFMEDIA_H

#include <coffee/core/CBase>
#include <coffee/core/CTypes>

namespace Coffee{
namespace CFFMedia{

using namespace CResources;

struct CFFVideoPlayer;

struct CFFVideoDestination
{
    CSize output_size;
};

extern void coffee_ffmedia_init();

extern CFFVideoPlayer* coffee_ffmedia_create_player(const CResource& source);

extern size_t coffee_ffmedia_video_framesize(CFFVideoPlayer* video);

extern size_t coffee_ffmedia_audio_framesize(CFFVideoPlayer* video);

extern bool coffee_ffmedia_decode_frame(
        CFFVideoPlayer* video, const CFFVideoDestination &dest);

}
}

#endif
