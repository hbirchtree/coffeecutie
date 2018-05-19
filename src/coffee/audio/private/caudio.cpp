#include <coffee/audio/caudio.h>

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <stb.h>

#include <coffee/core/CProfiling>
#include <coffee/core/CMath>

#define STB_API "STB_VORBIS::"

namespace Coffee{
namespace CAudio{
namespace Stb{

bool LoadVorbis(CAudio::AudioSample *smp, Bytes const& src)
{
    DProfContext _(STB_API "Decoding audio");

    /* C is not so good with uint64 used by our resource format*/
    int32 data_size = C_CAST<int32>(src.size);

    smp->samples = C_CAST<uint32>(stb_vorbis_decode_memory(
                C_CAST<ubyte_t*>(src.data),data_size,
                reinterpret_cast<int*>(&smp->fmt.channels),
                reinterpret_cast<int*>(&smp->fmt.samplerate),
                &smp->data));
    if(smp->samples<=0)
        return false;
    smp->fmt.bitdepth = smp->samples / smp->fmt.samplerate*smp->fmt.channels;
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
    return fmt.samplerate*fmt.channels*samples*CMath::max(C_CAST<uint32>(1),fmt.bitdepth/8);
}

}
}