#include <coffee/audio/caudio.h>

#define STB_VORBIS_HEADER_ONLY
#include <stb/stb_vorbis.c>
#include <stb/stb.h>

namespace Coffee{
namespace CAudio{
namespace CStbAudio{

bool LoadVorbis(CAudio::AudioSample *smp, CResources::CResource *src)
{
    /* C is not so good with uint64 used by our resource format*/
    int data_size = src->size;

    smp->samples = stb_vorbis_decode_memory(
                (ubyte_t*)src->data,data_size,
                &smp->fmt.channels,&smp->fmt.samplerate,&smp->data);
    if(smp->samples<=0)
        return false;
    smp->fmt.bitdepth = smp->samples/smp->fmt.samplerate*smp->fmt.channels;
    return true;
}

}

AudioFormat::AudioFormat():
    samplerate(0),
    channels(0),
    bitdepth(0)
{
}

bigscalar GetSampleLength(AudioSample const& smp)
{
    return (bigscalar)smp.samples/(bigscalar)smp.fmt.samplerate;
}

szptr GetSampleDataSize(const AudioFormat &fmt, szptr samples)
{
    return fmt.samplerate*fmt.channels*samples*CMath::max(1,fmt.bitdepth/8);
}

}
}
