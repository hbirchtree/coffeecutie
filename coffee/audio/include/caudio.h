#ifndef CAUDIO_FUN
#define CAUDIO_FUN

#include <coffee/core/CTypes>
#include <base/cfiles.h>

namespace Coffee{
namespace CAudio{

/*!
 * \brief Format structure for audio
 */
struct CAudioFormat
{
    CAudioFormat();

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
struct CAudioSample
{
    short* data = nullptr; /*!< Pointer to audio data*/
    CAudioFormat fmt; /*!< Format specification*/
    int samples;
};

extern bigscalar sample_get_length(const CAudioSample &smp);

extern szptr sample_get_datasize(CAudioFormat const& fmt, szptr samples);

namespace CStbAudio{

/*!
 * \brief Import a Vorbis audio sample
 * \param smp Sample to load to
 * \param src Resource to load from
 */
extern void vorbis_load(CAudioSample* smp, CResources::CResource* src);

}

}
}

#endif
