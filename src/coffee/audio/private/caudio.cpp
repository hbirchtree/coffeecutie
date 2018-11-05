#include <coffee/audio/caudio.h>

#define STB_VORBIS_HEADER_ONLY
#include <stb.h>
#include <stb_vorbis.c>

#include <coffee/core/plat/memory.h>
#include <coffee/core/CMath>
#include <coffee/core/CProfiling>

#define STB_API "STB_VORBIS::"

namespace Coffee {
namespace CAudio {
namespace Stb {

bool LoadVorbis(AudioSample* smp, Bytes const& src)
{
    using audio_data = AudioSample::short_data;

    DProfContext _(STB_API "Decoding audio");

    /* C is not so good with uint64 used by our resource format*/
    i32 data_size = C_CAST<i32>(src.size);

    i32 samples = stb_vorbis_decode_memory(
        C_CAST<ubyte_t*>(src.data),
        data_size,
        reinterpret_cast<int*>(&smp->fmt.channels),
        reinterpret_cast<int*>(&smp->fmt.samplerate),
        &smp->data);

    if(samples < 0)
        return false;

    smp->samples = C_FCAST<u32>(samples);

    smp->container = audio_data::From(
        smp->data, smp->samples * smp->fmt.channels * smp->fmt.samplerate);

    audio_data::SetDestr(smp->container, [](audio_data& data)
    {
        Mem::CFree(data.data);
    });

    smp->fmt.bitdepth = smp->samples / smp->fmt.samplerate * smp->fmt.channels;
    return true;
}

} // namespace Stb

AudioFormat::AudioFormat() : samplerate(0), channels(0), bitdepth(0)
{
}

bigscalar GetSampleLength(AudioSample const& smp)
{
    return (bigscalar)smp.samples / (bigscalar)smp.fmt.samplerate;
}

szptr GetSampleDataSize(const AudioFormat& fmt, szptr samples)
{
    return fmt.samplerate * fmt.channels * samples *
           CMath::max(C_CAST<u32>(1), fmt.bitdepth / 8);
}

} // namespace CAudio
} // namespace Coffee
