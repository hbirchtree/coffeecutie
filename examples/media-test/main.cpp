#include <coffee/CCore>
#include <coffee/CFFMedia>

using namespace Coffee::CFFMedia;

int32 coffee_main(int32, byte_t**)
{
    coffee_ffmedia_init();

    CResource testfile("sample_data/test.mp4");

    CFFVideoPlayer* video = coffee_ffmedia_create_player(testfile);

    cDebug("Frame size: {0}",coffee_ffmedia_video_framesize(video));

    C_UNUSED(video);
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
