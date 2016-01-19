#include "caudio.h"

#define STB_VORBIS_HEADER_ONLY
#include <stb/stb_vorbis.c>
#include <stb/stb.h>

namespace Coffee{
namespace CAudio{
namespace CStbAudio{

void vorbis_load(CAudio::AudioSample *smp, CResources::CResource *src)
{
    smp->samples = stb_vorbis_decode_memory(
                (ubyte_t*)src->data,src->size,
                &smp->fmt.channels,&smp->fmt.samplerate,&smp->data);
    smp->fmt.bitdepth = smp->samples/smp->fmt.samplerate*smp->fmt.channels;
}

}

AudioFormat::AudioFormat():
    samplerate(0),
    channels(0),
    bitdepth(0)
{
}

bigscalar sample_get_length(AudioSample const& smp)
{
    return (bigscalar)smp.samples/(bigscalar)smp.fmt.samplerate;
}

szptr sample_get_datasize(const AudioFormat &fmt, szptr samples)
{
    return fmt.samplerate*fmt.channels*samples*CMath::max(1,fmt.bitdepth/8);
}

}
}
