#ifndef CAUDIO_FUN
#define CAUDIO_FUN

#include <coffee/core/CTypes>
#include <base/cfiles.h>

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

extern bigscalar sample_get_length(const AudioSample &smp);

extern szptr sample_get_datasize(AudioFormat const& fmt, szptr samples);

namespace CStbAudio{

/*!
 * \brief Import a Vorbis audio sample
 * \param smp Sample to load to
 * \param src Resource to load from
 */
extern void vorbis_load(AudioSample* smp, CResources::CResource* src);

}

}
}

#endif
