#ifndef CAUDIO_FUN
#define CAUDIO_FUN

#include <coffee/core/CTypes>
#include <coffee/core/base/cfiles.h>

namespace Coffee{
namespace CAudio{

/*!
 * \brief Format structure for audio
 */
struct CAudioFormat
{
    CAudioFormat();

    int samplerate; /*!< Samplerate, typically 44.1kHz*/
    int samples;
    int channels; /*!< Audio channels, typically 2 or 4*/
    int bitdepth;
};

/*!
 * \brief An audio sample
 */
struct CAudioSample
{
    CAudioFormat fmt; /*!< Format specification*/
    short* data = nullptr; /*!< Pointer to audio data*/
};

namespace CStbAudio{

/*!
 * \brief Import a Vorbis audio sample
 * \param smp Sample to load to
 * \param src Resource to load from
 */
extern void coffee_stb_audio_vorbis_load(CAudioSample* smp, CResources::CResource* src);

}

}
}

#endif
