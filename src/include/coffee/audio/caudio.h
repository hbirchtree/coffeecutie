#ifndef CAUDIO_FUN
#define CAUDIO_FUN

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/base/files/cfiles.h>

namespace Coffee{
/*!
 * \brief General audio rendering functions
 */
namespace CAudio{

/*!
 * \brief Basic audio sample formats
 * Designation is as follows:
 *
 * [F][N][C]
 *
 * F = data format
 * N = data precision
 * C = channel specification
 *
 * M=mono, S=stereo, Q=quadrophonic
 */
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
        u32 samplerate; /*!< Samplerate, typically 44.1kHz*/
        u32 frequency;
    };
    u32 channels; /*!< Audio channels, typically 2 or 4*/
    u32 bitdepth;
};

/*!
 * \brief An audio sample
 */
struct AudioSample
{
    using short_data = _cbasic_data_chunk<short>;

    short_data container;
    short* data = nullptr; /*!< Pointer to audio data*/
    AudioFormat fmt; /*!< Format specification*/
    u32 samples;
};

extern bigscalar GetSampleLength(const AudioSample &smp);

extern szptr GetSampleDataSize(AudioFormat const& fmt, szptr samples);

namespace Stb{

/*!
 * \brief Import a Vorbis audio sample
 * \param smp Sample to load to
 * \param src Resource to load from
 */
extern bool LoadVorbis(AudioSample* smp, Bytes const& src);

}

}
}

#endif
