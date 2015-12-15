#include <coffee/CCore>
#include <coffee/CFFMedia>

//#define FFMPEG_MEMLEAK_FULL_TEST
#define FFMPEG_MEMLEAK_DECODE_TEST

using namespace Coffee::CFFMedia;

int32 coffee_main(int32, byte_t**)
{
    coffee_ffmedia_init(nullptr,false);

    CResource testfile("sample_data/test.mp4");
    coffee_file_pull(testfile);

#ifdef FFMPEG_MEMLEAK_FULL_TEST
    while(true)
    {
#endif
    CFFVideoPlayer* video = coffee_ffmedia_create_player(testfile);

    if(!video)
        return 1;

    void* test_frame_data = c_alloc(coffee_ffmedia_video_framesize(video));

    CFFVideoTarget trg;
    trg.v.location = test_frame_data;

    CFFDecodeContext* dCtxt = coffee_ffmedia_create_decodecontext(
                video,CSize(1920,1080),CAudioFormat());

#if defined(FFMPEG_MEMLEAK_DECODE_TEST)&&!defined(FFMPEG_MEMLEAK_FULL_TEST)
    while(true)
    {
#endif
    coffee_ffmedia_decode_frame(video,dCtxt,&trg);
#if defined(FFMPEG_MEMLEAK_DECODE_TEST)&&!defined(FFMPEG_MEMLEAK_FULL_TEST)
    }
#endif

    coffee_ffmedia_free_decodecontext(dCtxt);
    coffee_ffmedia_free_player(video);
    c_free(test_frame_data);

#ifdef FFMPEG_MEMLEAK_FULL_TEST
    }
#endif
    coffee_file_free(testfile);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
