#ifndef CAUDIO_FUN
#define CAUDIO_FUN

#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"
#include "stb/stb.h"

#include "coffee_types.h"
#include "coffee/cfiles.h"

namespace Coffee{
namespace CStbAudio{

using namespace CResources;

struct CStbAudioFormat
{
    int samplerate  = 0;
    int samples     = 0;
    int channels    = 0;
};

struct CStbAudioSample
{
    CStbAudioFormat fmt;
    short* data = nullptr;
};

static void coffee_stb_audio_vorbis_load(CStbAudioSample* smp, CResource* src)
{
    smp->fmt.samples = stb_vorbis_decode_memory(
                (ubyte*)src->data,src->size,
                &smp->fmt.channels,&smp->fmt.samplerate,&smp->data);
}

}
}

#endif
