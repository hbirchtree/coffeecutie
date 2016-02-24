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
};

/*!
 * \brief Format structure for audio
 */
struct AudioFormat
{
    AudioFormat();

    union{
        int samplerate; /*!< Samplerate, typically 44.1kHz*/
        int frequency;
    };
    int channels; /*!< Audio channels, typically 2 or 4*/
    int bitdepth;
};

/*!
 * \brief An audio sample
 */
struct AudioSample
{
    short* data = nullptr; /*!< Pointer to audio data*/
    AudioFormat fmt; /*!< Format specification*/
    int samples;
};

extern bigscalar GetSampleLength(const AudioSample &smp);

extern szptr GetSampleDataSize(AudioFormat const& fmt, szptr samples);

namespace CStbAudio{

/*!
 * \brief Import a Vorbis audio sample
 * \param smp Sample to load to
 * \param src Resource to load from
 */
extern bool LoadVorbis(AudioSample* smp, CResources::CResource* src);

}

}
}

#endif
