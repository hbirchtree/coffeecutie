#ifndef CAUDIO_FUN
#define CAUDIO_FUN

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/base/files/cfiles.h>

namespace Coffee{
namespace CAudio{

enum class SampleFormat
{
    U8M,
    U8S,
    U8Q,

    I16M,
    I16S,
    I16Q,

    F32M,
    F32S,
    F32Q,

    /* M=mono, S=stereo, Q=quadrophonic */
};

/*!
 * \brief Format structure for audio
 */
struct AudioFormat
{
    AudioFormat();

    union{
        uint32 samplerate; /*!< Samplerate, typically 44.1kHz*/
        uint32 frequency;
    };
    uint32 channels; /*!< Audio channels, typically 2 or 4*/
    uint32 bitdepth;
};

/*!
 * \brief An audio sample
 */
struct AudioSample
{
    short* data = nullptr; /*!< Pointer to audio data*/
    AudioFormat fmt; /*!< Format specification*/
    uint32 samples;
};

extern bigscalar GetSampleLength(const AudioSample &smp);

extern szptr GetSampleDataSize(AudioFormat const& fmt, szptr samples);

namespace Stb{

/*!
 * \brief Import a Vorbis audio sample
 * \param smp Sample to load to
 * \param src Resource to load from
 */
extern bool LoadVorbis(AudioSample* smp, CResources::Resource* src);

}

}
}

#endif
