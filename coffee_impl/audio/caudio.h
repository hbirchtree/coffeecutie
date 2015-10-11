#ifndef CAUDIO_FUN
#define CAUDIO_FUN

#include "coffee_types.h"
#include "coffee/cfiles.h"

namespace Coffee{
namespace CAudio{

/*!
 * \brief Format structure for audio
 */
struct CAudioFormat
{
    int samplerate  = 0; /*!< Samplerate, typically 44.1kHz*/
    int samples     = 0;
    int channels    = 0; /*!< Audio channels, typically 2 or 4*/
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

using namespace CResources;
using namespace CAudio;

/*!
 * \brief Import a Vorbis audio sample
 * \param smp Sample to load to
 * \param src Resource to load from
 */
extern void coffee_stb_audio_vorbis_load(CAudioSample* smp, CResource* src);

}

}
}

#endif
